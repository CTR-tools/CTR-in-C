#include "HOWL.h"

// Music_Restart @ 0x8002E4C0 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// If cseq music is playing, restart the current song. Verified vs Ghidra decomp + h114.
// CseqMusic_Restart = func_80028F34.

extern void func_80028F34(u_short songID, int a1);   // CseqMusic_Restart

int D_8008D80C = 0;   // gp+0x8A0 cseqBoolPlay
int D_8008D810 = 0;   // gp+0x8A4 cseqHighestIndex

void Music_Restart(void)
{
    if (D_8008D80C != 0)
        func_80028F34(D_8008D810, 8);
}
