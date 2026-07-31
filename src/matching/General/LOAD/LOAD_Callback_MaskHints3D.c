// LOAD_Callback_MaskHints3D @ 0x80031a50  (vs SCUS_944.26.exe)
// SCORE 0. Clear load_inProgress; sdata->modelMaskHints3D = lqs->ptrDestination (gp+0x184).
// Score with GP_VALUE=0x8008CF6C (patch_gprel resolves %gp_rel relocs).
#include "CTR.h"
struct LoadQueueSlot { char pad0[0xC]; void *ptrDestination; };
int D_8008D0A4 = 0;      // load_inProgress (gp+0x138)
void *D_8008D0F0 = 0;    // sdata->modelMaskHints3D (gp+0x184)
void LOAD_Callback_MaskHints3D(struct LoadQueueSlot *lqs)
{
    D_8008D0A4 = 0;
    D_8008D0F0 = lqs->ptrDestination;
}
