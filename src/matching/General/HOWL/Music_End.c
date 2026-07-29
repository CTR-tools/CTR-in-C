#include "HOWL.h"

// Music_End @ 0x8002E53C (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Marks cseq music as stopped: clear play flag, set current-song index to -1. Verified vs Ghidra + h117.

int D_8008D80C = 0;   // gp+0x8A0 cseqBoolPlay
int D_8008D810 = 0;   // gp+0x8A4 cseqHighestIndex

void Music_End(void)
{
    D_8008D80C = 0;
    D_8008D810 = -1;
}
