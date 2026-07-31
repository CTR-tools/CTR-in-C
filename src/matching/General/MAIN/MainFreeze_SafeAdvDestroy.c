// MainFreeze_SafeAdvDestroy @ 0x800399fc  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// If in Adventure Arena (gGT->gameMode1 & ADVENTURE_ARENA=0x100000) AND the AdvHub
// overlay is loaded (LOAD_IsOpen_AdvHub), tear down the pause menu (AH_Pause_Destroy).
// gGT gp-relative (gp+0x340).
#include "CTR.h"

#define ADVENTURE_ARENA 0x100000

struct GameTracker { int gameMode1; };
struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */

extern int  func_80034920(void);   /* LOAD_IsOpen_AdvHub */
extern void func_800B1EF8(void);   /* AH_Pause_Destroy */

void MainFreeze_SafeAdvDestroy(void)
{
    if ((D_8008D2AC->gameMode1 & ADVENTURE_ARENA) != 0 && func_80034920() != 0)
        func_800B1EF8();
}
