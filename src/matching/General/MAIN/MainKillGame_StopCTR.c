// MainKillGame_StopCTR @ 0x8003c41c  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// CTR shutdown sequence: EnterCriticalSection, DrawSyncCallback(MainDrawCb_DrawSyncPtr
// @gp+0x48C), ExitCriticalSection, StopCallback, MEMCARD_CloseCard, PadStopCom,
// ResetGraph(3), VSyncCallback(0), Timer_Destroy.
#include "CTR.h"

void *D_8008D3F8 = 0;   /* sdata->MainDrawCb_DrawSyncPtr (gp+0x48C) */

extern void func_80076990(void);      /* EnterCriticalSection */
extern void func_80072EF8(void *cb);  /* DrawSyncCallback */
extern void func_800767D0(void);      /* ExitCriticalSection */
extern void func_80077288(void);      /* StopCallback */
extern void func_8003D95C(void);      /* MEMCARD_CloseCard */
extern void func_8007588C(void);      /* PadStopCom */
extern void func_80072D28(int mode);  /* ResetGraph */
extern void func_80077254(int cb);    /* VSyncCallback */
extern void func_8004B370(void);      /* Timer_Destroy */

void MainKillGame_StopCTR(void)
{
    func_80076990();
    func_80072EF8(D_8008D3F8);
    func_800767D0();
    func_80077288();
    func_8003D95C();
    func_8007588C();
    func_80072D28(3);
    func_80077254(0);
    func_8004B370();
}
