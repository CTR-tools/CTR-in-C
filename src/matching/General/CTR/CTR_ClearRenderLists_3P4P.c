// CTR_ClearRenderLists_3P4P @ 0x80021c2c  (vs SCUS_944.26.exe)
//
// BEHAVIOR-EXACT best-match (Branch Differences = 0, Stack Differences = 0).
// Residual score = 600 (Register Differences = 8): gcc-2.8.1 rebases the gGT
// pointer to gGT+0x1828 and counts down, whereas retail addresses each slot
// directly off the incoming gGT base (gGT+0x1808..+0x1828); this plus the
// resulting s-register naming shift is a pure register-allocation/scheduling
// difference the permuter cannot bridge (same wall as CTR_ClearRenderLists_1P2P).
//
// Per player (stride 0x30), 3P/4P uses 4 render-list slots instead of 5:
//   slot 0..3: bspListStart = 0 ; ptrQuadBlocksRendered = D_80081B30[i]
//   slot 4   : ptrQuadBlocksRendered = 0   (bspListStart untouched)
// D_80081B30 = per-player rendered-quadblock destination array (3P/4P variant,
// distinct from the 1P/2P array D_80081B20).

#include "CTR.h"

extern void *D_80081B30[];   // per-player rendered-quadblock destination pointers (3P/4P)

void CTR_ClearRenderLists_3P4P(char *gGT, int numPlyrCurrGame)
{
    void **v;
    int i;

    if (numPlyrCurrGame <= 0) return;

    v = D_80081B30;
    i = 0;
    do
    {
        *(int *)(gGT + 0x180c) = 0;
        *(void **)(gGT + 0x1808) = *v;
        *(int *)(gGT + 0x1814) = 0;
        *(void **)(gGT + 0x1810) = *v;
        i++;
        *(int *)(gGT + 0x181c) = 0;
        *(void **)(gGT + 0x1818) = *v;
        *(int *)(gGT + 0x1824) = 0;
        *(int *)(gGT + 0x1828) = 0;
        *(void **)(gGT + 0x1820) = *v;
        v++;
        gGT += 0x30;
    } while (i < numPlyrCurrGame);
}
