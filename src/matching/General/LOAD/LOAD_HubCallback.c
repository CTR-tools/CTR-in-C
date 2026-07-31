// LOAD_HubCallback @ 0x80031b14  (vs SCUS_944.26.exe)
// SCORE 0. Async hub-load done cb: clear load_inProgress, run PatchMem, set
// gGT->level2 = ptrLevelFile (gGT+0x164), then MEMPACK_SwapPacks(gGT->activeMempackIndex).
// (in-repo decomp is stale here: it shows visMem2=lev->visMem; retail calls SwapPacks.)
// Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"

extern char *D_8008D2AC;   // sdata->gGT
int D_8008D0A4 = 0;        // sdata->load_inProgress (gp+0x138)
void *D_8008D08C = 0;      // sdata->ptrLevelFile (gp+0x120)

extern void func_80031AA4(void *);   // LOAD_Callback_PatchMem
extern void func_8003E80C(int);      // MEMPACK_SwapPacks

void LOAD_HubCallback(void *lqs)
{
    char *gGT;

    D_8008D0A4 = 0;
    func_80031AA4(lqs);

    gGT = D_8008D2AC;
    *(void **)(gGT + 0x164) = D_8008D08C;
    func_8003E80C(*(short *)(gGT + 0x254a));
}
