// MainLoadVLC_Callback @ 0x8003c508  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// VLC-table load-complete callback: sets sdata->bool_IsLoaded_VlcTable = 1 (gp+0x494).
#include "CTR.h"

int D_8008D400 = 0;   /* sdata->bool_IsLoaded_VlcTable (gp+0x494) */

void MainLoadVLC_Callback(void *lqs)
{
    (void)lqs;
    D_8008D400 = 1;
}
