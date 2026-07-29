#include "HOWL.h"

// Music_Start @ 0x8002E524 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Marks cseq music as playing and records the current song index. Verified vs Ghidra decomp + h116.

int D_8008D80C = 0;   // gp+0x8A0 cseqBoolPlay
int D_8008D810 = 0;   // gp+0x8A4 cseqHighestIndex

void Music_Start(u_int songID)
{
    D_8008D80C = 1;
    D_8008D810 = songID & 0xffff;
}
