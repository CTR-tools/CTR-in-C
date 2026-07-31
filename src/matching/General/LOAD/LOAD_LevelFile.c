// LOAD_LevelFile @ 0x80034874  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Begins loading a new level: wipe modelMaskHints3D, clear hudFlags bit0, save
// prevLEV=old levelID and set the new levelID, mask renderFlags to just 0x1000
// (checkered flag), transition the race flag off-screen if fully off, then arm
// Loading.stage = LOAD_TEN_STAGES_0 (0). Match lever: keep the top-loaded gGT for
// hudFlags/levelID/prevLEV but read renderFlags via a fresh D_8008D2AC (retail
// reloads gGT once for renderFlags).
#include "CTR.h"

struct GameTracker {
    char pad_1A10[0x1A10];
    int levelID;                       /* 0x1A10 */
    char pad_1D31[0x1D31 - 0x1A14];
    unsigned char hudFlags;            /* 0x1D31 */
    char pad_1EB4[0x1EB4 - 0x1D32];
    int prevLEV;                       /* 0x1EB4 */
    char pad_256C[0x256C - 0x1EB8];
    int renderFlags;                   /* 0x256C */
};

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
int D_8008D0F0 = 0;   /* sdata->modelMaskHints3D (gp+0x184) */
extern int D_8008D0F8;   /* sdata->Loading.stage (absolute); LOAD_TEN_STAGES_0 = 0 */

extern int  func_80043F28(void);   /* RaceFlag_IsFullyOffScreen */
extern void func_80043FB0(int);    /* RaceFlag_BeginTransition */

void LOAD_LevelFile(int levelID)
{
    struct GameTracker *gGT = D_8008D2AC;

    D_8008D0F0 = 0;
    gGT->hudFlags &= 0xFE;
    gGT->prevLEV = gGT->levelID;
    gGT->levelID = levelID;
    D_8008D2AC->renderFlags &= 0x1000;

    if (func_80043F28() == 1)
        func_80043FB0(1);

    D_8008D0F8 = 0;
}
