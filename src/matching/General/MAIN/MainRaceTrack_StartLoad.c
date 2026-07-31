// MainRaceTrack_StartLoad @ 0x8003cf7c  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Begins loading a race track: howl_StopAudio(1,0,0) (clear backup, keep music,
// destroy most fx), ElimBG_Deactivate(gGT), LOAD_LevelFile(levelID).
// gGT gp-relative (gp+0x340); levelID is a sign-extended short arg.
#include "CTR.h"

struct GameTracker { int dummy; };
struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */

extern void func_8002C8A8(int a, int b, int c);   /* howl_StopAudio */
extern void func_80024C08(struct GameTracker *g); /* ElimBG_Deactivate */
extern void func_80034874(int levelID);           /* LOAD_LevelFile */

void MainRaceTrack_StartLoad(short levelID)
{
    func_8002C8A8(1, 0, 0);
    func_80024C08(D_8008D2AC);
    func_80034874(levelID);
}
