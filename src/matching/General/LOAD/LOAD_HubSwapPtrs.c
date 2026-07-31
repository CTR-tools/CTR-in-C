// LOAD_HubSwapPtrs @ 0x80031bdc  (vs SCUS_944.26.exe)
// SCORE 0. Swaps the primary/secondary hub level+visMem pointers (level1<->level2
// @0x160/0x164, visMem1<->visMem2 @0x1A38/0x1A3C) and sets gGT->boolHubSwapped=1
// (gGT+0x2548). No-op if level2==0. NOTE: the in-repo decomp OMITS the
// boolHubSwapped=1 store; the retail (and saphi) include it.
#include "CTR.h"

struct GameTracker {
    char pad0[0x160];
    void *level1;                 // 0x160
    void *level2;                 // 0x164
    char pad1[0x1A38 - 0x168];
    void *visMem1;                // 0x1A38
    void *visMem2;                // 0x1A3C
    char pad2[0x2548 - 0x1A40];
    s16 boolHubSwapped;            // 0x2548
};

void LOAD_HubSwapPtrs(struct GameTracker *gGT)
{
    void *oldLev1;
    void *oldVisMem1;
    void *oldVisMem2;

    if (gGT->level2 == 0)
        return;

    oldLev1 = gGT->level1;
    oldVisMem1 = gGT->visMem1;
    oldVisMem2 = gGT->visMem2;

    gGT->level1 = gGT->level2;
    gGT->boolHubSwapped = 1;
    gGT->level2 = oldLev1;
    gGT->visMem1 = oldVisMem2;
    gGT->visMem2 = oldVisMem1;
}
