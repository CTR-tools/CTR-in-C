#include "HOWL.h"

// Music_LowerVolume @ 0x8002E418 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Lowers cseq music volume during "FINAL LAP!": 150 (50%) normally, 90 (25%) for songs outside {1,2}.
// Verified vs Ghidra decomp + h112. CseqMusic_ChangeVolume = func_80028E5C.
// KEY CRACK (705->0): retail does NOT if-convert the volume choice. Writing it as `setVolume = X;
// if(cond) setVolume = Y; call(id,setVolume,8)` lets gcc collapse to a compact `li/li/one-lhu` form.
// Retail is the branchy two-call structure: each branch independently reloads `lhu a0` (cseqHighestIndex)
// and sets its own `li a1`, then gcc cross-jumping merges ONLY the shared `jal` tail. Split into two
// distinct func_80028E5C() calls to reproduce the duplicated lhu + `j` to the shared tail.

extern void func_80028E5C(u_short songID, int volume, int a2);   // CseqMusic_ChangeVolume

int D_8008D80C = 0;   // gp+0x8A0 cseqBoolPlay
int D_8008D810 = 0;   // gp+0x8A4 cseqHighestIndex

void Music_LowerVolume(void)
{
    if (D_8008D80C != 0)
    {
        if ((u_int)(D_8008D810 - 1) < 2)
            func_80028E5C(D_8008D810, 0x96, 8);
        else
            func_80028E5C(D_8008D810, 0x5A, 8);
    }
}
