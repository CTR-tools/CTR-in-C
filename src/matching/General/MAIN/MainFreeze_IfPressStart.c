// MainFreeze_IfPressStart @ 0x80039e98  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Pause-on-START handler: if none of the 11 guards fire (RaceFlag_IsFullyOnScreen,
// renderFlags&0x1000, AkuAkuHintState, ptrActiveMenu, gameMode1&(END_OF_RACE|PAUSE_ALL)
// =0x20000F, levelID==MAIN_MENU_LEVEL, GAME_CUTSCENE, boolDemoMode, levelID-OXIDE_ENDING<2,
// load_inProgress, gameMode2&VEH_FREEZE_PODIUM), pause: gameMode1|=PAUSE_1, GetMenuPtr,
// rowSelected=0, RECTMENU_Show, TogglePauseAudio(1), OtherFX_Play(1,1), ElimBG_Activate.
// Match lever: load gGT AFTER the leading RaceFlag_IsFullyOnScreen() call so it lands
// in a temp reg (v1), not a saved reg. gGT gp-relative (gp+0x340); AkuAkuHintState
// gp-relative (gp+0x908); ptrActiveMenu/load_inProgress absolute.
#include "CTR.h"

#define END_OF_RACE     0x200000
#define PAUSE_ALL       0xF
#define GAME_CUTSCENE   0x20000000
#define MAIN_MENU_LEVEL 0x27
#define OXIDE_ENDING    0x2A
#define VEH_FREEZE_PODIUM 4

struct RectMenu { char pad0[0x1A]; short rowSelected; };
struct GameTracker {
    int gameMode1;                 /* 0x0    */
    char pad8[0x8 - 0x4];
    int gameMode2;                 /* 0x8    */
    char pad_1A10[0x1A10 - 0xC];
    int levelID;                   /* 0x1A10 */
    char pad_1D32[0x1D32 - 0x1A14];
    unsigned char boolDemoMode;    /* 0x1D32 */
    char pad_256C[0x256C - 0x1D33];
    int renderFlags;               /* 0x256C */
};

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */
short D_8008D874 = 0;                  /* sdata->AkuAkuHintState (gp+0x908) */
extern void *D_8008D908;              /* sdata->ptrActiveMenu (absolute) */
extern int   D_8008D0A4;              /* sdata->load_inProgress (absolute) */

extern int  func_80043F1C(void);           /* RaceFlag_IsFullyOnScreen */
extern struct RectMenu *func_80039DCC(void); /* MainFreeze_GetMenuPtr */
extern void func_80046990(struct RectMenu *); /* RECTMENU_Show */
extern void func_80034B48(int);            /* MainFrame_TogglePauseAudio */
extern void func_80028468(int, int);       /* OtherFX_Play */
extern void func_8002481C(struct GameTracker *); /* ElimBG_Activate */

void MainFreeze_IfPressStart(void)
{
    struct GameTracker *gGT;
    int gameMode1;
    struct RectMenu *menu;

    if (func_80043F1C() != 0) return;
    gGT = D_8008D2AC;
    if ((gGT->renderFlags & 0x1000) != 0) return;
    if (D_8008D874 != 0) return;
    if (D_8008D908 != 0) return;
    gameMode1 = gGT->gameMode1;
    if ((gameMode1 & (END_OF_RACE | PAUSE_ALL)) != 0) return;
    if (gGT->levelID == MAIN_MENU_LEVEL) return;
    if ((gameMode1 & GAME_CUTSCENE) != 0) return;
    if (gGT->boolDemoMode != 0) return;
    if ((u32)(gGT->levelID - OXIDE_ENDING) < 2) return;
    if (D_8008D0A4 != 0) return;
    if ((gGT->gameMode2 & VEH_FREEZE_PODIUM) != 0) return;

    gGT->gameMode1 = gameMode1 | 1;
    menu = func_80039DCC();
    menu->rowSelected = 0;
    func_80046990(menu);
    func_80034B48(1);
    func_80028468(1, 1);
    func_8002481C(D_8008D2AC);
}
