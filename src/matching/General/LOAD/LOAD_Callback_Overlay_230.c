// LOAD_Callback_Overlay_230 @ 0x800319f4  (vs SCUS_944.26.exe)
// SCORE 0. Overlay-230 load-done cb: clear load_inProgress; gGT->overlayIndex_Threads=0.
// Score with GP_VALUE=0x8008CF6C (patch_gprel resolves %gp_rel relocs).
#include "CTR.h"

extern char *D_8008D2AC;   // sdata->gGT
int D_8008D0A4 = 0;        // load_inProgress (gp+0x138)

void LOAD_Callback_Overlay_230(void)
{
    D_8008D0A4 = 0;
    D_8008D2AC[0x2541] = 0;
}
