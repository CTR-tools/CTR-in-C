#include "HOWL.h"

// Music_GetHighestSongPlayIndex @ 0x8002E550 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH.
// Returns the current cseq song index (0xffff = none, 0 = level music, 1 = game aku). Verified vs h118.

int D_8008D810 = 0;   // gp+0x8A4 cseqHighestIndex

int Music_GetHighestSongPlayIndex(void)
{
    return D_8008D810;
}
