// LibraryOfModels_Clear @ 0x800314c0  (vs SCUS_944.26.exe)
//
// SCORE 0 exact match.
//
// Clears all 226 entries (indices 0..0xE1) of the GameTracker's modelPtr lookup
// table (gGT+0x2160). The retail emits a decrementing loop (idx 0xE1 down to 0);
// the natural incrementing for-loop reverses to the identical code under
// gcc-2.8.1. NOTE: the in-repo decomp's `< 0xe1` (225) is an off-by-one artifact
// — the retail clears 226 entries (< 0xe2), which saphi also has correctly.
#include "CTR.h"

struct GameTracker { char pad0[0x2160]; void *modelPtr[1]; };

void LibraryOfModels_Clear(struct GameTracker *gGT)
{
    int i;

    for (i = 0; i < 0xe2; i++)
        gGT->modelPtr[i] = 0;
}
