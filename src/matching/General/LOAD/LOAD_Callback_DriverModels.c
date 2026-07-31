// LOAD_Callback_DriverModels @ 0x80031b00  (vs SCUS_944.26.exe)
// SCORE 0. Clear load_inProgress; sdata->ptrMPK = (int)lqs->ptrDestination (gp+0x11C).
// Score with GP_VALUE=0x8008CF6C (patch_gprel resolves %gp_rel relocs).
#include "CTR.h"
struct LoadQueueSlot { char pad0[0xC]; void *ptrDestination; };
int D_8008D0A4 = 0;      // load_inProgress (gp+0x138)
int D_8008D088 = 0;      // sdata->ptrMPK (gp+0x11C)
void LOAD_Callback_DriverModels(struct LoadQueueSlot *lqs)
{
    D_8008D0A4 = 0;
    D_8008D088 = (int)lqs->ptrDestination;
}
