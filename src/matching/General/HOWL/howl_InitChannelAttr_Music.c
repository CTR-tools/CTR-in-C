#include "HOWL.h"

// howl_InitChannelAttr_Music @ 0x80029F80 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 340, with 122/122 INSTRUCTION PARITY
//   and a byte-EXACT structure (diff = 16 lines). USER-GRANTED exception. Behavior is byte-for-byte
//   correct; the entire residual is 2 gcc-2.8.1 register/instruction choices that no C form can
//   force and the permuter reaches only via SEMANTICALLY-BROKEN forms (a `volatile` spill + an
//   `andi $s0,$v0,0xff` that masks the pitch to 8 bits instead of the target's 16):
//     (a) pitch: retail emits `andi $s0,$v0,0xffff` (mask the howl_InstrumentPitch return to 16
//         bits); gcc emits `move $s0,$v0` because the following `sh` already truncates to 16 bits,
//         so the mask is provably redundant and gcc drops it. u_short / int / `& 0xffff` all still
//         yield `move`; only masking to a u_char yields `andi 0xff` (wrong value).
//     (b) the sampleVol `>>10` shift + its two mults use $a0 (this compile) vs $a1 (retail) — both
//         reuse a dead argument register; a pure allocator tie-break. scaledVol / index-reuse /
//         volume-first / drum-do-while all failed to flip it.
//   Everything else matches exactly: the flags&4 dispatch (inverted polarity), both sample paths,
//   the spuStartAddr/ad/sr stores, the per-path channelVol fold, and the pitch/reverb tail.
//   Manual C drove this 4655 -> 2005 -> 1360 -> 1080 -> 460 -> 340. Match-necessary forms below.
//
// Builds the SPU ChannelAttr for one CSEQ music note from its SongSeq. Two paths by seq->flags bit2:
//   instrument (clear): reads g_pCseqLongSamples[instrumentID] and pitches via
//     howl_InstrumentPitch(basePitch, index, distort).
//   drum (set): reads g_pCseqShortSamples[index], applying g_anDistortConstOtherFX[distort] unless
//     distort==0x80, with fixed ad=0x80ff / sr=0x1fc2.
// spuStartAddr = g_pHowlSampleAddrTable[spuIndex] << 3. Per-sample volume folds g_bVolMusic *
// g_aSongPool[songPoolIndex].vol_Curr * seq->vol_Curr >> 10 * sample.volume, then Channel_SetVolume
// with (sampleVol*channelVol)>>15 and pan seq->LR. Finally attr->pitch and attr->reverb.
//
// Match-necessary forms (each verified to lower the score; all sane/readable):
//   * `if ((flags & 4) != 0) {drum} else {instrument}` — inverted from the reference `if(==0)` so the
//     drum block is inline and the instrument block is out-of-line, matching retail's `beqz v1,...`.
//   * `song = &g_aSongPool[songPoolIndex]` hoisted before the branch (retail keeps it in one reg,
//     used by both paths); the inner drum distort-if is also inverted to match retail's `beq`.
//   * per-path `* channelVol` folded into each path's sampleVol (retail does `mult t0,s4` in each
//     path's branch-delay slot, NOT once after the join), then `Channel_SetVolume(.., sampleVol>>15)`.
//   * `scaledVol` temp for the `>>10` (moves the shift into an argument register, matching retail).
//   * `do { ... } while (0)` around the instrument-path body forces the load scheduling that removes
//     an otherwise-emitted `nop` after the pitch move (this is the 440 -> 340 step).
//   * attr->ad = 0x80ff (a u_short field) so gcc emits `ori` for the fixed drum ad, matching retail.
//
// Build note: g_pCseq*/SampleAddrTable/g_bVolMusic are GP-relative (gp=0x8008CF6C), DEFINED so gcc
// emits %gp_rel (GP_VALUE=0x8008CF6C). g_aSongPool (D_80095D84) and g_anDistortConstOtherFX
// (D_800829AC) are absolute (%hi/%lo), left extern.

extern uint func_8002991C(int basePitch, int pitchIndex, uint distort);   // howl_InstrumentPitch (#30)
extern void func_8002B540(struct ChannelAttr* attr, uint vol, uint pan);  // Channel_SetVolume

extern struct Song D_80095D84[];   // g_aSongPool
extern int         D_800829AC[];   // g_anDistortConstOtherFX

int    D_8008D7C4 = 0;   // g_pCseqShortSamples    (gp+0x858)
u_char D_8008D7B8 = 0;   // g_bVolMusic            (gp+0x84C)
int    D_8008D7DC = 0;   // g_pHowlSampleAddrTable (gp+0x870)
int    D_8008D7E8 = 0;   // g_pCseqLongSamples     (gp+0x87C)

void howl_InitChannelAttr_Music(struct SongSeq* seq, struct ChannelAttr* attr, int index, int channelVol)
{
    uint instrPitch;
    short pitch;
    struct SampleInstrument* longSample;
    struct SampleDrums* drumSample;
    int sampleVol;
    int scaledVol;
    struct Song* song;

    song = &D_80095D84[seq->songPoolIndex];
    if ((seq->flags & 4) != 0)
    {
        drumSample = (struct SampleDrums*)(D_8008D7C4 + index * 8);
        if (seq->distort != 0x80)
        {
            pitch = (short)((uint)drumSample->pitch * D_800829AC[seq->distort] >> 0x10);
        }
        else
        {
            pitch = drumSample->pitch;
        }
        attr->spuStartAddr = (void*)((uint)*(u_short*)(drumSample->spuIndex * 4 + D_8008D7DC) << 3);
        attr->ad = 0x80ff;
        attr->sr = 0x1fc2;
        scaledVol = (int)((uint)D_8008D7B8 * (uint)song->vol_Curr * (uint)seq->vol_Curr) >> 10;
        sampleVol = scaledVol * (uint)drumSample->volume * channelVol;
    }
    else
    {
        do
        {
            longSample = (struct SampleInstrument*)(D_8008D7E8 + seq->instrumentID * 0xc);
            instrPitch = func_8002991C(longSample->basePitch, index, seq->distort);
            attr->spuStartAddr = (void*)((uint)*(u_short*)(longSample->spuIndex * 4 + D_8008D7DC) << 3);
            attr->ad = longSample->ad;
            attr->sr = longSample->sr;
            scaledVol = (int)((uint)D_8008D7B8 * (uint)song->vol_Curr * (uint)seq->vol_Curr) >> 10;
            sampleVol = scaledVol * (uint)longSample->volume * channelVol;
        } while (0);
        pitch = (short)instrPitch;
    }
    func_8002B540(attr, (uint)sampleVol >> 0xf, (uint)seq->LR);
    attr->pitch = pitch;
    attr->reverb = (u_short)seq->reverb;
}
