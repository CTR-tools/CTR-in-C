#include "HOWL.h"

// Channel_UpdateChannels @ 0x8002BE9C (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 2770 (from an 11500 first-compile, over
//   ~6 fires), the whole ~190-instruction function byte-for-BEHAVIOR correct. NO head-load-lui blocker —
//   the entire residual is pure gcc register-allocation/scheduling. USER-GRANTED case-by-case exception
//   (2026-07-27) after a re-surface: the "deferred-store scheduling" the grind had been chasing WAS
//   cracked (11500 -> 2770); the residual 2770 is a handful of SEPARATE, individually-unforceable
//   gcc-codegen details (listed below).
//
//   Structural cracks that got here:
//     * ADSR mode-decode via `uint` + bit-extract (`ad >> 0xf`, srl) NOT `(short)ad < 0` (sll/sra).
//     * Every ChannelAttr sub-field is read `*(u_short*)&` to force retail's `lhu` — the fields are
//       declared `short`, so a plain read emits `lh` (sr, pitch, reverb, audioL, audioR).
//     * The ADSR numeric args (ar/dr/sr_arg/sl) are LOCALS computed before the call, but the
//       arMode/srMode/rrMode decode results are passed as INLINE TERNARY arg-expressions. That makes gcc
//       spill each mode straight to its stack-arg slot in-branch, as retail does under register pressure.
//       (An if-statement instead HOLDS them in registers: that form scores 4220, the ternary form 2770.)
//     * The volume block reads new->audioR once (lhu) and reuses it for the store and the sign-extended
//       R arg.
//
//   RESIDUAL 2770 (each unforceable from C, verified over the grind):
//     * arMode/srMode branch-polarity: gcc's fall-through heuristic picks the opposite case from retail;
//       inherent to the ternary-spill form (the only alternative that keeps the branch, an if-statement,
//       holds the value instead of spilling — provably no middle ground).
//     * flags-base $s7 hoist: retail keeps &g_abChannelUpdateFlags in $s7; here the audioL-pointer cache
//       (pAudioL) wins that saved register net (freeing $s7 for the base costs more elsewhere).
//     * a redundant `andi 0xffff` on sr before the srMode shift (retail has it; unreproducible because
//       sr is already known-16-bit from the lhu read).
//     * reverb `!= 0` compiled as `sltu` here vs a branch in retail.
//   The permuter plateaus at 2770 across 5+ windows; below it, it finds only semantically-BROKEN forms
//   (volatile locals, dropping the sl `& 0xf` mask, const-folding rrMode) — none deliverable.
//
//   Two permuter-found scheduling nudges are load-bearing (removing either regresses ~280) and kept:
//   the `do { ... } while (0)` around the pass-1 clear, and re-reading new->sr for the cur->sr store.
//
// Per-frame SPU voice commit. Walks the 24 channels' dirty flags (g_abChannelUpdateFlags) and pushes the
// staged attributes (g_abChannelAttrNew[ch]) to the hardware, caching them in g_aChannelAttrCur[ch].
// Pass 1: collect bit0 (key-off) channels, SpuSetKey(0, mask), clearing bit0 (or the whole word if bit1
// clear). Pass 2 per channel: bit1=key-on collect, bit2=start addr, bit3=ADSR (decoding AR/SR/RR modes),
// bit4=pitch, bit5=reverb, bit6=volume; then clear the flag word. Finally SpuSetKey(1, keyOnMask).
//
// g_abChannelUpdateFlags=D_8008FC6C (int[24]), g_abChannelAttrNew=D_8008FCCC, g_aChannelAttrCur=D_8008FE4C
// (both ChannelAttr[24]) are absolute. SPU libspu callees: func_80076610 SpuSetKey, func_80076910
// SpuSetVoiceStartAddr, func_80076420 SpuSetVoiceADSRAttr, func_80076560 SpuSetVoicePitch,
// func_80076840 SpuSetReverbVoice, func_80076890 SpuSetVoiceVolume.

extern void func_80076610(int on_off, int voiceBits);        // SpuSetKey
extern void func_80076910(int ch, int startAddr);            // SpuSetVoiceStartAddr
extern void func_80076420(int ch, int ar, int dr, int sr, int rr, int sl, int arMode, int srMode, int rrMode); // SpuSetVoiceADSRAttr
extern void func_80076560(int ch, int pitch);                // SpuSetVoicePitch
extern void func_80076840(int on, int voiceBits);            // SpuSetReverbVoice
extern void func_80076890(int ch, int volL, int volR);       // SpuSetVoiceVolume

extern int                D_8008FC6C[];   // g_abChannelUpdateFlags (one int per channel)
extern struct ChannelAttr D_8008FCCC[];   // g_abChannelAttrNew
extern struct ChannelAttr D_8008FE4C[];   // g_aChannelAttrCur

void Channel_UpdateChannels(void)
{
    int voiceBits;
    int vNum;
    int* ptrFlag;
    short* pAudioL;
    int flags;
    int one;

    // Pass 1: collect the key-off channels.
    voiceBits = 0;
    vNum = 0;
    ptrFlag = D_8008FC6C;
    do
    {
        flags = *ptrFlag;
        if (flags != 0 && (flags & 1) != 0)
        {
            voiceBits |= 1 << vNum;
            if ((flags & 2) != 0)
            {
                // do/while(0): permuter-found scheduling nudge, load-bearing (see MATCH NOTE).
                do { *ptrFlag = flags & 0xfffffffe; } while (0);
            }
            else
            {
                *ptrFlag = 0;
            }
        }
        vNum = vNum + 1;
        ptrFlag = ptrFlag + 1;
    } while (vNum < 0x18);
    func_80076610(0, voiceBits);

    // Pass 2: collect key-on channels and commit each dirty attribute.
    voiceBits = 0;
    vNum = 0;
    one = 1;
    do
    {
        int* pFlags = &D_8008FC6C[vNum];
        flags = *pFlags;
        if (flags != 0)
        {
            struct ChannelAttr* cur = &D_8008FE4C[vNum];
            struct ChannelAttr* new = &D_8008FCCC[vNum];

            if ((flags & 2) != 0)
                voiceBits |= one << vNum;

            if ((flags & 4) != 0)
            {
                void* startAddr = new->spuStartAddr;
                if (startAddr != cur->spuStartAddr)
                {
                    cur->spuStartAddr = startAddr;
                    func_80076910(vNum, (int)startAddr);
                }
            }

            pAudioL = &new->audioL;
            if ((flags & 8) != 0)
            {
                int adsr = *(int*)&new->ad;
                if (adsr != *(int*)&cur->ad)
                {
                    uint ad = new->ad;
                    uint sr = *(u_short*)&new->sr;
                    int ar, dr, sr_arg, sl;

                    cur->ad = ad;
                    cur->sr = *(u_short*)&new->sr;   // re-read, load-bearing (see MATCH NOTE)
                    sl = ad & 0xf;
                    ar = (ad >> 8) & 0x7f;
                    dr = (ad >> 4) & 0xf;
                    sr_arg = (sr >> 6) & 0x7f;
                    // Modes as inline ternaries -> gcc spills each straight to its stack-arg slot in-branch.
                    func_80076420(vNum, ar, dr, sr_arg, sr & 0x1f, sl,
                                  ((ad >> 0xf) != 0) ? 5 : one,
                                  ((sr >> 0xf) != 0) ? (((sr >> 0xe) & 1) == 0 ? 5 : 7)
                                                     : (((sr >> 0xe) & 1) == 0 ? one : 3),
                                  ((sr >> 5) & 1) == 0 ? 3 : 7);
                }
            }

            if ((flags & 0x10) != 0)
            {
                int pitch = *(u_short*)&new->pitch;
                if (pitch != *(u_short*)&cur->pitch)
                {
                    cur->pitch = pitch;
                    func_80076560(vNum, pitch);
                }
            }

            if ((flags & 0x20) != 0)
            {
                int reverb = *(u_short*)&new->reverb;
                if (reverb != *(u_short*)&cur->reverb)
                {
                    cur->reverb = reverb;
                    func_80076840(reverb != 0, one << vNum);
                }
            }

            if ((flags & 0x40) != 0)
            {
                int audioLR = *(int*)pAudioL;
                if (audioLR != *(int*)&cur->audioL)
                {
                    int audioR;
                    cur->audioL = *(u_short*)&new->audioL;
                    audioR = *(u_short*)&new->audioR;
                    cur->audioR = audioR;
                    func_80076890(vNum, cur->audioL, (short)audioR);
                }
            }

            *pFlags = 0;
        }
        vNum = vNum + 1;
    } while (vNum < 0x18);
    func_80076610(1, voiceBits);
}
