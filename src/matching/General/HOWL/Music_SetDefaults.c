#include "HOWL.h"

// Music_SetDefaults @ 0x8002E338 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH.
// Resets the cseq music defaults (no music playing). Verified vs Ghidra decomp + h110.

int D_8008D80C = 0;   // gp+0x8A0 cseqBoolPlay
int D_8008D810 = 0;   // gp+0x8A4 cseqHighestIndex
int D_8008D814 = 0;   // gp+0x8A8 cseqTempo

void Music_SetDefaults(void)
{
    D_8008D80C = 0;
    D_8008D810 = -1;
    D_8008D814 = 0;
}
