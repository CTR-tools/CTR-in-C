// MainFrame_RequestMaskHint @ 0x80035e20  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// (Ghidra: AkuHint_Request.) Requests an Aku/Uka mask direction-hint: if not paused
// (gGT->gameMode1 & 0xF == 0) and no hint pending (AkuHint_RequestedHint == -1), set
// AkuAkuHintState=1 (gp+0x908), install VehPhysProc_FreezeEndEvent_Init on
// drivers[0]->funcPtrs[0], and record RequestedHint=hintId (gp+0x360) +
// boolInterruptWarppad (gp+0x362, a HALFWORD/s16 — matches saphi s16 field).
// gGT gp-relative (gp+0x340).
#include "CTR.h"

struct Driver { char pad0[0x54]; void *funcPtrs[13]; };
struct GameTracker { int gameMode1; char pad4[0x24EC - 4]; struct Driver *drivers[8]; };

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */
short D_8008D2CC = 0;   /* sdata->AkuHint_RequestedHint (gp+0x360) */
short D_8008D2CE = 0;   /* sdata->AkuHint_boolInterruptWarppad (gp+0x362, stored sh) */
short D_8008D874 = 0;   /* sdata->AkuAkuHintState (gp+0x908) */

extern void func_80062D04(void);   /* VehPhysProc_FreezeEndEvent_Init */

void MainFrame_RequestMaskHint(short hintId, short interruptWarpPad)
{
    struct GameTracker *gGT = D_8008D2AC;

    if ((gGT->gameMode1 & 0xF) == 0 && D_8008D2CC == -1)
    {
        D_8008D874 = 1;
        gGT->drivers[0]->funcPtrs[0] = func_80062D04;
        D_8008D2CC = hintId;
        D_8008D2CE = interruptWarpPad;
    }
}
