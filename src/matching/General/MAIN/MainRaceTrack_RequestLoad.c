// MainRaceTrack_RequestLoad @ 0x8003cfc0  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Requests a race-track load: clear hudFlags bit0, transition the race flag off-screen
// if fully off (IsFullyOffScreen==1 -> BeginTransition(1)), ResetTextAnim, then arm
// Loading.stage = LOAD_REQUESTED (-4, gp+0x18C) and Loading.Lev_ID_To_Load = levelID
// (gp+0x190, sign-extended short -> word). gGT gp-relative (gp+0x340).
#include "CTR.h"

struct GameTracker { char pad0[0x1D31]; unsigned char hudFlags; };   /* @0x1D31 */
struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */
int D_8008D0F8 = 0;   /* sdata->Loading.stage (gp+0x18C); LOAD_REQUESTED = -4 */
int D_8008D0FC = 0;   /* sdata->Loading.Lev_ID_To_Load (gp+0x190) */

extern int  func_80043F28(void);   /* RaceFlag_IsFullyOffScreen */
extern void func_80043FB0(int);    /* RaceFlag_BeginTransition */
extern void func_80044290(void);   /* RaceFlag_ResetTextAnim */

void MainRaceTrack_RequestLoad(short levelID)
{
    D_8008D2AC->hudFlags &= 0xFE;
    if (func_80043F28() == 1)
        func_80043FB0(1);
    func_80044290();
    D_8008D0F8 = -4;
    D_8008D0FC = levelID;
}
