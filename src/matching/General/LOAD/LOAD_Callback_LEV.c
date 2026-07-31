// LOAD_Callback_LEV @ 0x80031a78  (vs SCUS_944.26.exe)
// SCORE 0. If !(flags & LT_GETADDR=0x2): clear load_inProgress. sdata->ptrLevelFile = ptrDestination (gp+0x120).
// Score with GP_VALUE=0x8008CF6C (patch_gprel resolves %gp_rel relocs).
#include "CTR.h"
struct LoadQueueSlot { char pad0[0x4]; u16 flags; char pad1[0x6]; void *ptrDestination; };
int D_8008D0A4 = 0;      // load_inProgress (gp+0x138)
void *D_8008D08C = 0;    // sdata->ptrLevelFile (gp+0x120)
void LOAD_Callback_LEV(struct LoadQueueSlot *lqs)
{
    if ((lqs->flags & 0x2) == 0)
        D_8008D0A4 = 0;
    D_8008D08C = lqs->ptrDestination;
}
