/*
 * MATH_Cos @ 0x8003d1c0  (SCUS_944.26 / NTSC-U 926)
 *
 * Cosine of a 12-bit angle (0x1000 == 360 deg) sampled from the packed
 * quarter-wave table data.trigApprox (D_800845A0). Each table entry is a
 * 32-bit word holding a {cos:hi16, sin:lo16} pair; bit 0x400 selects which
 * half to read (clear -> hi/cos half via arithmetic >>16, set -> lo/sin half
 * via sign-extended low short) and bit 0x800 flips the sign, giving the
 * cosine quadrant offset relative to MATH_Sin. Returns a signed ~1.12 fixed.
 *
 * decomp-permuter: base score = 0 (byte-exact; Branch/Stack/Register 0).
 */
#include "CTR.h"

extern int D_800845A0[];   /* data.trigApprox — quarter-wave {cos,sin} packed table */

int MATH_Cos(u32 angle)
{
    int entry;
    int cosine;

    entry = D_800845A0[angle & 0x3ff];

    if ((angle & 0x400) != 0) {
        cosine = entry << 16 >> 16;
        if ((angle & 0x800) == 0)
            cosine = -cosine;
    } else {
        cosine = entry >> 16;
        if ((angle & 0x800) != 0)
            cosine = -cosine;
    }
    return cosine;
}
