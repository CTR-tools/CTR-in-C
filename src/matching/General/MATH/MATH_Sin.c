/*
 * MATH_Sin @ 0x8003D184  (SCUS_944.26.exe, NTSC-U 926)
 * Matching status: SCORE 0 (Branch/Stack/Register diffs all 0) vs SCUS_944.26.exe.
 *
 * Sine of a 12-bit angle (0x1000 = 360 degrees) via the quarter-wave lookup
 * table at 0x800845A0. Each table entry is a 32-bit word packing two signed
 * 16-bit samples { sin:lo16, cos:hi16 }, indexed by (angle & 0x3FF):
 *   - (angle & 0x400) == 0  -> take the low half  (sin), sign-extended
 *   - (angle & 0x400) != 0  -> take the high half (cos), sign-extended
 *   - (angle & 0x800) != 0  -> negate (lower half of the circle)
 * Returns the signed sine in ~1.12 fixed point. MATH_Cos is the same with a
 * +0x400 phase offset.
 */
#include "CTR.h"

/* Trig approximation table: 0x400 packed entries, each a 32-bit word holding
 * two signed 16-bit samples (low half = sin, high half = cos). */
extern int D_800845A0[];

int MATH_Sin(u32 angle)
{
    int trig = D_800845A0[angle & 0x3FF];

    if ((angle & 0x400) == 0)
        trig <<= 16;

    trig >>= 16;

    if (angle & 0x800)
        trig = -trig;

    return trig;
}
