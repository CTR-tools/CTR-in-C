#include "CTR.h"

// CTR_ClearRenderLists_1P2P @ 0x80021BBC (SCUS_944.26.exe, 926/NTSC-U).
// BEHAVIOR-EXACT best match (Branch/Stack 0; ~545/485 residual = gcc rebases the gGT loop pointer
// (+0x1828 w/ small offsets) vs retail direct gGT+0x1808.. + param reg naming; unforceable). Logic verified.
// For each of numPlyr players (gGT.LevRenderLists[i], stride 0x30): clear the 5 render-list slots to
// {ptrQuadBlocksRendered=D_80081B20[i], bspListStart=0}, and the FullDynamic pair to 0. Value re-read per slot.

extern void *D_80081B20[];   // per-player rendered-quadblock destination pointers

void CTR_ClearRenderLists_1P2P(char *gGT, int numPlyrCurrGame)
{
    void **v;
    int i;

    if (numPlyrCurrGame <= 0) return;

    v = D_80081B20;
    i = 0;
    do
    {
        *(int *)(gGT + 0x180c) = 0;
        *(void **)(gGT + 0x1808) = *v;
        *(int *)(gGT + 0x1814) = 0;
        *(void **)(gGT + 0x1810) = *v;
        *(int *)(gGT + 0x181c) = 0;
        *(void **)(gGT + 0x1818) = *v;
        *(int *)(gGT + 0x1824) = 0;
        *(void **)(gGT + 0x1820) = *v;
        i++;
        *(int *)(gGT + 0x182c) = 0;
        *(int *)(gGT + 0x1830) = 0;
        *(int *)(gGT + 0x1834) = 0;
        *(void **)(gGT + 0x1828) = *v;
        v++;
        gGT += 0x30;
    } while (i < numPlyrCurrGame);
}
