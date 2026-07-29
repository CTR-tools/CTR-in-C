#include "HOWL.h"

// UpdateChannelVol_OtherFX @ 0x8002AD04 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 345 (behavior byte-for-byte correct; the
//   whole ~26-instruction function matches except one caller-saved register-assignment swap, a ~6-line
//   diff). The residual: because the attr pointer arrives in $a1 but $a1 is later reused for the vol
//   result, attr must be preserved; retail preemptively moves it into $t0 in the flags-branch delay
//   slot (`beq ...; move $t0,$a1`) — an EXTRA, less-optimal move that keeps otherFX in $a0 for its two
//   reads and pushes the multiply product into $t1. gcc instead schedules a single late move, landing
//   attr in $a0/$a1 and the product in $t0. The explicit `attrCopy = attr` (kept below) forces the
//   early preservation retail does (675 -> 345) but gcc still picks $a0 for it (swapping otherFX to
//   $t0) rather than retail's $t0. 9 manual structural levers can't force attr into $t0, and the
//   permuter's only sub-345 forms are semantically broken (volatile-u_char truncation of the vol
//   product; uninitialized attrCopy). USER-GRANTED exception. Sibling #59 (no branch) matched at 0.
//
// Set one SPU channel's volume for an OtherFX sound: base = g_bVolEngineFX, or g_bVolOtherFX when
// OtherFX.flags bit2 (voice) is set; scaled by OtherFX.volume(+1) * vol >> 10, via Channel_SetVolume.
//
// Build note: g_bVolEngineFX (gp+0x840) / g_bVolOtherFX (gp+0x850) are GP-relative, DEFINED so gcc
// emits %gp_rel (GP_VALUE=0x8008CF6C). Callee Channel_SetVolume (func_8002B540). Polarity inverted
// (OtherFX inline) to match retail's beq-bit2==0->EngineFX.

extern void func_8002B540(struct ChannelAttr* attr, uint vol, uint pan);  // Channel_SetVolume

u_char D_8008D7AC = 0;   // g_bVolEngineFX (gp+0x840)
u_char D_8008D7BC = 0;   // g_bVolOtherFX  (gp+0x850)

void UpdateChannelVol_OtherFX(u_char* otherFX, struct ChannelAttr* attr, int vol, int LR)
{
    u_char bVar1;
    struct ChannelAttr* attrCopy;

    attrCopy = attr;
    if ((*otherFX & 4) != 0)
    {
        bVar1 = D_8008D7BC;
    }
    else
    {
        bVar1 = D_8008D7AC;
    }
    func_8002B540(attrCopy, (uint)bVar1 * (uint)otherFX[1] * vol >> 10, LR);
}
