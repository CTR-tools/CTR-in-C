#include "HOWL.h"

// Garage_LerpFX @ 0x800304B8 (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH -- score 220, NOT score-0 (auto-granted register/scheduling tie-break, 2026-07-29).
// Behavior byte-EXACT (119 insns, control flow identical -- Branch/Stack diffs = 0). Per-frame animator
// for the 8 garage SFX slots: for each slot, pick per-state targets (gsp_curr 0=CENTER vol 0xFF/LR 0x80,
// 1=LEFT 0x64/0x3C, 2=RIGHT 0x64/0xC3, else GONE vol 0/LR=current); if already at target, skip. Else
// lerp volume by +/-8 and LR by +/-2 toward target (clamp on overshoot), then (if the slot has a
// configured soundID D_8008D080[i]) push the new volume/LR to the live FX via OtherFX_RecycleNew
// (func_8002E690). When a slot has fully settled, copy gsp_curr->gsp_prev and, if GONE(3),
// OtherFX_RecycleMute (func_8002E724). pool = D_800962E4 (GarageFX, stride 0xC).
// Verified vs Ghidra decomp + in-repo h141 (the asm early-exit `continue` when already settled, and the
// gsp_prev=gsp_curr write, are both absent from h141 -- matched to the disassembly, not the decomp).
//
// CRACKS 3700->220: (1) re-read gsp_curr in the settle (don't reuse the dispatch value) so it isn't live
// across the calls -> no extra callee-saved reg (frame -40, was -48). (2) FULLY-INDEXED D_800962E4[i]
// everywhere so gcc strength-reduces the pool IV to s0 and &.audioPtr IV to s3 like retail (walking-ptr
// forms swap s0<->s3). (3) `int` (not short) targetVol/targetLR so the GONE-case `targetLR=LR` loads
// signed (lh) once instead of lhu + re-sign-extend cascade. (4) `u_char cur` in the settle to emit the
// `andi 0xff` + sb-before-bne order. (5) group the flag word `(LR|0x8000) | (volume<<16)`.
//
// MATCH NOTE -- residual 220 = two #34-class tie-breaks, both behavior-neutral: (a) the flag word --
// gcc canonicalizes the OR-tree so the volume<<16 term becomes the a2 arg accumulator with LR|0x8000 in
// a temp; retail seeds a2 with LR|0x8000 and keeps volume<<16 in v0 (`or a2,v0,a2`). Identical coupling
// to Garage_PlayFX (#140): retail loads volume first yet makes LR the accumulator, which gcc-2.8.1
// cannot reproduce (every load-order-correct form folds the const into volume). (b) gcc hoists the
// `lui %hi(D_8008D080)` into the LR-clamp branch delay slot where retail leaves a nop. The flag VALUE is
// identical. 4 manual flag variants + ~24 permuter iterations all plateau at 220 (nothing lower).

struct GarageFX { u_char gsp_curr; char gsp_prev; short volume; short LR; int audioPtr; };

extern void func_8002E690(int* slot, u_int soundID, u_int flags);   // OtherFX_RecycleNew
extern void func_8002E724(int* slot);                               // OtherFX_RecycleMute
extern struct GarageFX D_800962E4[];   // garageSoundPool[8]
extern u_char          D_8008D080[];   // per-slot soundID table

void Garage_LerpFX(void)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        int targetVol, targetLR;
        int c = D_800962E4[i].gsp_curr;

        if (c == 0)      { targetVol = 0xFF; targetLR = 0x80; }
        else if (c == 1) { targetVol = 0x64; targetLR = 0x3C; }
        else if (c == 2) { targetVol = 0x64; targetLR = 0xC3; }
        else             { targetVol = 0;    targetLR = D_800962E4[i].LR; }

        if (targetLR == D_800962E4[i].LR && targetVol == D_800962E4[i].volume)
            continue;

        if (targetVol != D_800962E4[i].volume)
        {
            int over;
            if (D_800962E4[i].volume < targetVol) { D_800962E4[i].volume += 8; over = D_800962E4[i].volume > targetVol; }
            else                                  { D_800962E4[i].volume -= 8; over = D_800962E4[i].volume < targetVol; }
            if (over) D_800962E4[i].volume = targetVol;
        }

        if (targetLR != D_800962E4[i].LR)
        {
            int over;
            if (D_800962E4[i].LR < targetLR) { D_800962E4[i].LR += 2; over = D_800962E4[i].LR > targetLR; }
            else                             { D_800962E4[i].LR -= 2; over = D_800962E4[i].LR < targetLR; }
            if (over) D_800962E4[i].LR = targetLR;
        }

        if (D_8008D080[i] != 0)
            func_8002E690(&D_800962E4[i].audioPtr, D_8008D080[i],
                          ((int)D_800962E4[i].LR | 0x8000) | ((int)D_800962E4[i].volume << 16));

        if (targetLR == D_800962E4[i].LR && targetVol == D_800962E4[i].volume)
        {
            u_char cur = D_800962E4[i].gsp_curr;
            D_800962E4[i].gsp_prev = cur;
            if (cur == 3)
                func_8002E724(&D_800962E4[i].audioPtr);
        }
    }
}
