/*
 * MATH_FastSqrt @ 0x8003D214 (SCUS_944.26 / NTSC-U 926)
 *
 * Byte-exact match: score 0, Branch/Stack/Register differences all 0
 * (20/20 instructions identical vs retail).
 *
 * Integer fixed-point square root by digit-by-digit (restoring) extraction,
 * two bits per iteration. `shift` selects the fractional scaling: the loop
 * runs (shift >> 1) + 0xF + 1 times (do-while post-decrement). Each pass pulls
 * the top two bits of `value` into the running `remainder`, forms the trial
 * divisor test = root*4 + 1 (expressed as root*=2; test = root*2+1), and if it
 * fits, subtracts it and sets the low result bit.
 *
 * Verified equivalent to saphi game/MATH/MATH_2_FastSqrt.c (Phase B: CLEAN).
 */
#include "CTR.h"

s32 MATH_FastSqrt(u32 value, s32 shift)
{
    s32 root = 0;
    u32 remainder = 0;
    s32 iterations = (shift >> 1) + 0xf;
    u32 test;

    do
    {
        remainder = (remainder << 2) | (value >> 30);
        value <<= 2;

        root = root * 2;
        test = root * 2 + 1;

        if (test <= remainder)
        {
            remainder -= test;
            root++;
        }
    } while (iterations-- != 0);

    return root;
}
