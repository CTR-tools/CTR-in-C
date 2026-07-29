#include "HOWL.h"

// Bank_ClearInRange @ 0x80029730 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 740. USER-GRANTED time-boxed
//   exception after a multi-fire deterministic + permuter effort (1790 -> 740). Behavior is
//   byte-for-byte correct; the residual is a gcc-2.8.1 register-allocation / scheduling quirk
//   spread across the whole ~30-instruction function:
//     * Retail computes `spuAddrStart + range` FIRST (before the header/guard), so a0
//       (spuAddrStart) is free after the lower-bound mask -- no copy. gcc-2.8.1 schedules that
//       add LATE and therefore emits an extra `move` to preserve spuAddrStart, which cascades
//       into a systematic register-name shift (header, table, bounds all land in different
//       registers than retail).
//     * Legitimate C forms tried and rejected (all >= 740): computing the sum before the guard
//       (spills to a saved reg, 1365/1525), first-in-guard (760), an h2 header-copy (1125),
//       int range (same), direct D_8008D7C0 reference (1050, re-loads each iteration), and an
//       explicit lower-bound mask (790).
//     * The permuter only scores below 740 by BREAKING correctness: one variant reads the fixed
//       `*D_8008D7DC` instead of the iterating `*sae`; another declares `spuAddrStart` as
//       `unsigned int`/`unsigned char`, which drops retail's 16-bit `andi 0xffff` mask (retail
//       masks it, so the parameter is genuinely 16-bit). None are deliverable.
//   The two legit improvements that ARE in this form: the loop increments are ordered
//   `sae += 2;` before `i += 1;`, and `upper` is computed inside the guard (both permuter-found,
//   both correct).
//
// Zeroes any cached SPU sample-address entries whose spuAddr falls within
// [spuAddrStart, spuAddrStart + range), so freed bank memory isn't reused stale. Iterates the
// sample-address table (each entry 4 bytes: spuAddr@0, spuSize@2); entry count is at header+0x10.

extern struct HowlHeader* D_8008D7C0;   // ptrHowlHeader (sample count @ +0x10)
extern u_short* D_8008D7DC;             // g_pHowlSampleAddrTable

void Bank_ClearInRange(u_short spuAddrStart, short range)
{
    struct HowlHeader* header;
    u_short* sae;
    uint i;
    u_short upper;

    header = D_8008D7C0;
    if (*(int*)((int)header + 0x10) != 0)
    {
        i = 0;
        upper = spuAddrStart + range;
        sae = D_8008D7DC;
        do
        {
            if ((spuAddrStart <= *sae) && (*sae < upper))
            {
                *sae = 0;
            }
            sae = sae + 2;
            i = i + 1;
        } while (i < *(uint*)((int)header + 0x10));
    }
}
