// LOAD_GlobalModelPtrs_MPK @ 0x80031b50  (vs SCUS_944.26.exe)
// SCORE 0. Registers the 3 driver-model-extras (data.driverModelExtras, abs
// D_80083A10) into gGT->modelPtr[id] (skip NULL / id==-1), then
// LibraryOfModels_Store(gGT, -1, PLYROBJECTLIST). NOTE: the retail COMPILES the
// driver-model loop; the in-repo decomp wrongly #if 0`d it as dead code.
// Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"

struct Model { char pad0[0x10]; s16 id; };
struct GameTracker { char pad0[0x2160]; struct Model *modelPtr[1]; };

extern struct Model *D_80083A10[3];       // data.driverModelExtras (absolute)
extern struct GameTracker *D_8008D2AC;    // sdata->gGT
extern void *D_8008D870;                  // sdata->PLYROBJECTLIST (absolute)
extern void func_8003147C(struct GameTracker *, int, void *);   // LibraryOfModels_Store

void LOAD_GlobalModelPtrs_MPK(void)
{
    struct GameTracker *gGT = D_8008D2AC;
    int i;

    for (i = 0; i < 3; i++)
    {
        struct Model *m = D_80083A10[i];
        if (m == 0)
            continue;
        if (m->id == -1)
            continue;
        gGT->modelPtr[m->id] = m;
    }

    if (D_8008D870 != 0)
        func_8003147C(D_8008D2AC, -1, D_8008D870);
}
