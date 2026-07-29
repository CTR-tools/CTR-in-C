#include "HOWL.h"

// howl_InstrumentPitch @ 0x8002991C (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 match against SCUS_944.26.exe (926/NTSC-U).
//
// Computes an SPU note frequency: looks up the base note-frequency table at
// [pitchIndex + (distort>>6) - 2], scales by basePitch (>>12); if the low 6 bits of distort are
// set, applies a fine-tune multiplier from the distort-const table. Returns the 16-bit pitch.
// Leaf function (no stack frame). Both tables are absolute (%hi/%lo).
//
// MATCH NOTES (all match-necessary temps):
//  - `shifted = (distort>>6) - 2` computed first: retail evaluates (distort>>6)-2 before adding
//    pitchIndex; without the temp gcc re-associates to (pitchIndex + distort>>6) - 2.
//  - `fineTune = table[...] + 0x100000` computed before the multiply: retail does the table lookup
//    first, then masks uVar1; inlining flips that order.

extern u_short D_80082EAC[];   // base note-frequency table
extern u_short D_80082F84[];   // distort-const fine-tune table

uint howl_InstrumentPitch(int basePitch, int pitchIndex, uint distort)
{
    uint uVar1;
    uint distortLow;
    uint fineTune;
    int shifted;

    shifted = ((int)distort >> 6) - 2;
    uVar1 = (uint)D_80082EAC[pitchIndex + shifted] * basePitch >> 0xc;
    distortLow = distort & 0x3f;
    if (distortLow != 0)
    {
        fineTune = D_80082F84[distortLow] + 0x100000;
        uVar1 = (uVar1 & 0xffff) * fineTune >> 0x14;
    }
    return uVar1 & 0xffff;
}
