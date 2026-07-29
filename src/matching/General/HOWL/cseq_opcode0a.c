#include "HOWL.h"

// cseq_opcode0a @ 0x8002A4A8 (SCUS_944.26.exe, 926/NTSC-U). (Ghidra: howl_SetSoundPitch)
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 10, with 76/76 INSTRUCTION PARITY and a
//   byte-EXACT body (the ENTIRE function differs by exactly one 2-instruction pair). Behavior is
//   byte-for-byte correct. The whole residual is the initial list-head load: retail computes the
//   %hi(g_pChannelTakenFirst) into a caller-saved scratch (`lui $v0`) and loads the head via it
//   (`lw $s0,%lo(..)($v0)`); gcc COALESCES the %hi-temp and the (callee-saved) load destination into
//   one register and emits `lui $s0,%hi; lw $s0,%lo(..)($s0)`. This is the IDENTICAL #34/#39/#42/#46-
//   class register-COALESCING tie-break on the IDENTICAL global (g_pChannelTakenFirst) — the 4th time
//   it has appeared (unforceable in #39/#42/#46; the permuter never cracks it). Unforceable from C
//   (head-split-from-for-init stays 10; the coalescing decision is in the allocator, not statement
//   structure). Melodic/percussive + distort polarities and all pitch math match exactly.
//
// CSEQ "set sound pitch" opcode (table index 10): latches the note operand into seq->distort, then
// for every MUSIC(2)-type channel playing this sequence's soundID, recomputes the channel's SPU
// pitch (g_abChannelAttrNew[channelID].pitch) and sets its update flag (|= 0x10). MELODIC channels
// (flags bit2 clear) pitch via howl_InstrumentPitch(g_pCseqLongSamples[instrumentID].basePitch,
// drumIndex_pitchIndex, distort); PERCUSSIVE (bit2 set) read g_pCseqShortSamples[drumIndex].pitch,
// scaled by g_anDistortConstOtherFX[distort] unless distort==0x80 (raw). Sibling of #41's builder.
//
// Build note: g_pCseqShortSamples/g_pCseqLongSamples are GP-relative (gp=0x8008CF6C), DEFINED so gcc
// emits %gp_rel (GP_VALUE=0x8008CF6C). The list head / attr / update-flag / distort tables are
// absolute (%hi/%lo). Callee howl_InstrumentPitch (func_8002991C, #30). Polarities match #41:
// percussive inline (flags&4 != 0), scaled inline (distort != 0x80).

extern uint func_8002991C(int basePitch, int pitchIndex, uint distort);  // howl_InstrumentPitch (#30)

extern struct ChannelStats* D_8009056C;   // g_pChannelTakenFirst
extern struct ChannelAttr   D_8008FCCC[];  // g_abChannelAttrNew (0x10 bytes each, pitch@8)
extern int                  D_8008FC6C[];  // g_abChannelUpdateFlags (one int per channel)
extern int                  D_800829AC[];  // g_anDistortConstOtherFX

int D_8008D7C4 = 0;   // g_pCseqShortSamples (gp+0x858)
int D_8008D7E8 = 0;   // g_pCseqLongSamples  (gp+0x87C)

void cseq_opcode0a(struct SongSeq* seq)
{
    struct ChannelStats* channel;
    struct SampleDrums* drum;
    struct SampleInstrument* longSample;

    seq->distort = seq->currNote[1];
    for (channel = D_8009056C; channel != 0; channel = channel->next)
    {
        if ((channel->type == 2) && (channel->soundID == seq->soundID))
        {
            if ((seq->flags & 4) != 0)
            {
                drum = (struct SampleDrums*)(D_8008D7C4 + channel->drumIndex_pitchIndex * 8);
                if (seq->distort != 0x80)
                {
                    D_8008FCCC[channel->channelID].pitch =
                        (short)((uint)drum->pitch * D_800829AC[seq->distort] >> 0x10);
                }
                else
                {
                    D_8008FCCC[channel->channelID].pitch = drum->pitch;
                }
            }
            else
            {
                longSample = (struct SampleInstrument*)(D_8008D7E8 + seq->instrumentID * 0xc);
                D_8008FCCC[channel->channelID].pitch =
                    (short)func_8002991C(longSample->basePitch, channel->drumIndex_pitchIndex, seq->distort);
            }
            D_8008FC6C[channel->channelID] |= 0x10;
        }
    }
}
