// LOAD_Callback_Overlay_Generic @ 0x800319e8  (vs SCUS_944.26.exe)
//
// SCORE 0 exact match. Score with GP_VALUE=0x8008CF6C (patch_gprel resolves the
// %gp_rel relocation: 0x8008D0A4 - 0x8008CF6C = 0x138).
//
// Async overlay-load completion callback: clears the global load-in-progress
// flag (sdata->load_inProgress @ gp+0x138). The queue-slot arg is unused.
#include "CTR.h"

int D_8008D0A4 = 0;   // sdata->load_inProgress (gp+0x138)

void LOAD_Callback_Overlay_Generic(void *lqs)
{
    D_8008D0A4 = 0;
}
