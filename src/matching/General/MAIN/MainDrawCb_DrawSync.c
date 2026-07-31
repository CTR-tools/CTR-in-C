// MainDrawCb_DrawSync @ 0x80034a80  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// DrawSync callback: clears gGT->bool_DrawOTag_InProgress (@0x1D30) if it was 1.
// Key match lever: the gGT pointer itself is accessed GP-RELATIVE here (lw a0,
// 0x340(gp) == D_8008D2AC), so declare it as a DEFINED `= 0` global (small-data),
// not `extern` (which would force absolute lui/lw addressing).
#include "CTR.h"

struct GameTracker { char pad0[0x1D30]; unsigned char bool_DrawOTag_InProgress; };  /* @0x1D30 */

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT — gp-relative (gp+0x340) */

void MainDrawCb_DrawSync(void)
{
    struct GameTracker *gGT = D_8008D2AC;

    if (gGT->bool_DrawOTag_InProgress == 1)
        gGT->bool_DrawOTag_InProgress = 0;
}
