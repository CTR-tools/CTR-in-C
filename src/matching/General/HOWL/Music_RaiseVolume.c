#include "HOWL.h"

// Music_RaiseVolume @ 0x8002E46C (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Restores cseq music volume after "FINAL LAP!": 255 (100%) normally, 190 (75%) for songs outside {1,2}.
// Sibling of Music_LowerVolume (#112) — identical structure, only the volume immediates differ.
// Verified vs Ghidra decomp + h113. CseqMusic_ChangeVolume = func_80028E5C.
// KEY CRACK (same as #112): retail does NOT if-convert the volume choice; it is the branchy two-call
// form where each branch independently reloads `lhu a0` (cseqHighestIndex) and sets its own `li a1`,
// and gcc cross-jumping merges ONLY the shared `jal` tail. Split into two distinct func_80028E5C() calls.

extern void func_80028E5C(u_short songID, int volume, int a2);   // CseqMusic_ChangeVolume

int D_8008D80C = 0;   // gp+0x8A0 cseqBoolPlay
int D_8008D810 = 0;   // gp+0x8A4 cseqHighestIndex

void Music_RaiseVolume(void)
{
    if (D_8008D80C != 0)
    {
        if ((u_int)(D_8008D810 - 1) < 2)
            func_80028E5C(D_8008D810, 0xFF, 8);
        else
            func_80028E5C(D_8008D810, 0xBE, 8);
    }
}
