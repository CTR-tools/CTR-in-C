#include "HOWL.h"

// howl_InitChannelAttr_OtherFX @ 0x8002C424 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best score = 90 (6 register diffs + 1 reordering, 0 insertions,
//   0 deletions), byte-BEHAVIOR-correct. AUTO-GRANTED (2026-07-27) per the <300 regalloc/scheduling-plateau
//   policy. The residual is the same unforceable gcc-2.8.1 class as its sibling #84 (howl_InitChannelAttr_
//   EngineFX, granted at 50): a caller-saved register swap plus one linked scheduling tie-break, neither
//   forceable from C nor by the permuter:
//     (1) SWAP: retail assigns distort->$a0 (the freed 1st-arg reg) and bVolScale->$v1; gcc here picks
//         distort->$v1 and bVolScale->$a0. distort has the higher use-count (beq + sll*4) and by gcc's
//         local-alloc priority "should" win $a0, but the allocator flips it. 6 register-name diffs, no
//         instruction/structure differences.
//     (2) LINKED mflo GAP-FILL (the 1 reordering): retail emits `mflo $t0` immediately after the 2nd
//         `mult $t0,$a2` and then `li 0x80`; gcc fills the mult->mflo latency slot with the `li 0x80`
//         (the distort==0x80 compare constant), pushing the mflo one slot later. This is downstream of
//         the register swap (which reg holds the distort compare operand changes the scheduler's choice).
//   Structural crack that got here: the `int spuAddr` local read EARLY (before the ad/sr stores) so the
//   SPU sample address is materialized before the ADSR writes, matching retail's ordering — 600 -> 90
//   (the identical fix that took #84 360 -> 50). Manual regalloc/scheduling levers that did NOT beat 90:
//   volMulTmp temp, reading distort into an early `int` local (CSE'd away), reversing the multiply operand
//   order (gcc normalizes the symmetric mult), and a do{}while(0) around the pitch branch (fractured the
//   structure -> 1305). Permuter plateaus at 90 over 170+ iterations (fluctuates 90..1785, never below 90).
//
// Builds a ChannelAttr for a non-engine FX voice. Like howl_InitChannelAttr_EngineFX but the volume scale
// is vol_Voice (D_8008D7BC) when OtherFX.flags&4, else vol_FX (D_8008D7AC); uses the OtherFX distort table
// (D_800829AC). pitch is computed before Channel_SetVolume (held across the call), stored after; fixed ADSR
// (ad=0x80ff, sr=0x1fc2); spuStartAddr = SampleAddrTable[spuIndex] << 3.
//
// GP-rel DEFINED (GP_VALUE=0x8008CF6C): vol_FX=D_8008D7AC(0x840,u8), vol_Voice=D_8008D7BC(0x850,u8),
// sampleAddrTable=D_8008D7DC(0x870,int). ABS extern distortConst_OtherFX=D_800829AC(int[]). Callee
// func_8002B540 Channel_SetVolume(#72).

extern void func_8002B540(struct ChannelAttr* attr, uint vol, uint pan);  // Channel_SetVolume

extern int D_800829AC[];   // g_anDistortConstOtherFX

u_char D_8008D7AC = 0;   // vol_FX                 (gp+0x840)
u_char D_8008D7BC = 0;   // vol_Voice              (gp+0x850)
int    D_8008D7DC = 0;   // g_pHowlSampleAddrTable (gp+0x870)

void howl_InitChannelAttr_OtherFX(struct OtherFX* otherFX, struct ChannelAttr* attr,
                                  int vol, int LR, int distort)
{
    u_char bVolScale;
    uint volArg;
    short pitch;

    if ((otherFX->flags & 4) != 0)
        bVolScale = D_8008D7BC;
    else
        bVolScale = D_8008D7AC;
    volArg = ((uint)bVolScale * otherFX->volMul * vol) >> 10;

    if (distort != 0x80)
        pitch = (short)((uint)otherFX->pitch * D_800829AC[distort] >> 0x10);
    else
        pitch = otherFX->pitch;

    func_8002B540(attr, volArg, LR);
    attr->pitch = pitch;
    {
        int spuAddr = *(u_short*)(otherFX->spuIndex * 4 + D_8008D7DC);
        attr->ad = 0x80ff;
        attr->sr = 0x1fc2;
        attr->spuStartAddr = (void*)((uint)spuAddr << 3);
    }
}
