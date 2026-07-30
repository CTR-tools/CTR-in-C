// LibraryOfModels_Store @ 0x8003147c  (vs SCUS_944.26.exe)
//
// SCORE 0 exact match.
//
// Registers each model of the array into the GameTracker's modelPtr lookup
// table (gGT+0x2160, indexed by the model's id). Walks up to numModels entries
// (numModels is unsigned, so -1 loops until the NULL terminator), stops early on
// a NULL pointer, and skips models whose id == -1. id is a signed s16 at m+0x10.
#include "CTR.h"

struct Model { char pad0[0x10]; s16 id; };
struct GameTracker { char pad0[0x2160]; struct Model *modelPtr[1]; };

void LibraryOfModels_Store(struct GameTracker *gGT, u_int numModels, struct Model **ptrModelArray)
{
    while (numModels != 0)
    {
        struct Model *m = *ptrModelArray;
        if (m == 0)
            return;
        if (m->id != -1)
            gGT->modelPtr[m->id] = m;
        numModels--;
        ptrModelArray++;
    }
}
