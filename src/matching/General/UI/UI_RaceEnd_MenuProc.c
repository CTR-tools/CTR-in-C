/*
 * UI_RaceEnd_MenuProc @ 0x80055C90 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg42, score 1350 — residual is a register-COLORING cascade:
 * the retail keeps `menu` in a0 before the RECTMENU_HideMenu call (live-range split) where our gcc
 * uses s0 throughout, plus the EXIT-TO-MAP gameMode1 coloring; all non-behavioral). Real types.
 *
 * RECTMENU funcPtr callback for the post-race results menu. On a sub-box redraw (unk1e != 0) it only
 * clears drawStyle bit 0x100, re-sets it for 3+ players, and returns. On commit (unk1e == 0) it acts
 * on the selected option = menu->rows[menu->rowSelected].stringIndex (rowSelected < 0 aborts):
 *   - Unless the option is 9 (Save Ghost) it first hides the menu (RECTMENU_HideMenu).
 *   - Resets the end-of-race anim (g_nFramesSinceRaceEnded=0 @gp+0x5C0, g_nNumIconsEOR=1 @gp+0x604).
 *   - Dispatches (compiled as a comparison-tree switch; the far value 0xC9 stays in-tree):
 *       3  QUIT           GhostTape_Destroy, mainMenuState=0, load main-menu LEV (0x27);
 *       4  RESTART        hudFlags&=~1; if RaceFlag_IsFullyOffScreen()==1 RaceFlag_BeginTransition(1);
 *                         Loading.stage=-5; howl_StopAudio(1,0,0); if the player beat the ghost
 *                         (gGT->gameModeEnd@0x1D44 bit0) copy GhostRecording->ptrGhost into
 *                         ptrGhostTapePlaying (0x3E00 bytes, inlined runtime-alignment block move),
 *                         boolGhostsDrawing=0, and set g_awCharacterIDs[1] = the ghost's characterID;
 *       5  CHANGE CHAR    GhostTape_Destroy, mainMenuState=1 ─┐
 *       6  CHANGE LEVEL   GhostTape_Destroy, mainMenuState=2 ─┼ shared tail: Loading.AddBitsConfig0
 *       10 CHANGE SETUP   mainMenuState=3 ───────────────────┘ |= 0x2000, load 0x27;
 *       9  SAVE GHOST     g_nFramesSinceRaceEnded=0x3F9, SelectProfile_ToggleMode(0x31),
 *                         ptrDesiredMenuBox = &data_menuBox_GhostSelection;
 *       0xC9 PRESS-X      g_nMenuReadyToPass |= 1 (@gp+0x550);
 *       0xD  EXIT TO MAP  Loading.OnBegin: AddBitsConfig0|=0x100000, RemBitsConfig0|=0xC000000,
 *                         RemBitsConfig8|=8; for ADVENTURE_CUP add ADVENTURE_CUP (→0x1C000000) and
 *                         load Gemstone Valley (0x19); else for a boss race (gameMode1<0) add
 *                         0x80000000 + AddBitsConfig8|=1, then load gGT->prevLEV.
 *
 * NOTE(claude) vs in-repo decomp (REFERENCE only; 926 Ghidra+asm authoritative): the 926 diverges
 * from UI_55_RaceEnd_MenuProc.c — it compiles as a comparison-tree switch (not a jump table), keeps
 * the 0xC9 case INSIDE the tree, splits the decomp's `case 5:case 6: mainMenuState=option-4` into two
 * constant stores, and folds EXIT-TO-MAP's RemBitsConfig0 bits per-path (only |0xC000000 baseline,
 * +0x10000000 for adv-cup, +0x80000000 for boss) rather than a single |0x1C000000. The 0x3E00 ghost
 * copy is written as memcpy() and inlines to the retail runtime-(src|dst)&3 alignment branch
 * (aligned 16-byte loop / unaligned lwl-lwr loop). gGT is reloaded per-use (D_8008D2AC) except within
 * EXIT-TO-MAP, where it is cached (gameMode1 read once). Cases are ordered to match the retail block
 * layout; the three CHANGE cases full-duplicate the tail so cross-jumping reproduces the retail
 * mainMenuState/AddBitsConfig0 merge. The 0x8008D100 Loading block is labelled OnBegin per the decomp
 * + saphi (the Ghidra DB's "OnComplete" is a mislabel; same address either way).
 *
 * Phase B (saphi game/UI/UI_RaceFlow.c UI_RaceEnd_MenuProc): CLEAN — no bugs. Every case is faithful.
 * saphi already carries the retail corrections: case 9 writes ptrDesiredMenu (not ptrActiveMenu, so
 * the RECTMENU core does the swap + clears flag 0x1000), case 10 sets the "in menus" |0x2000 flag,
 * howl_StopAudio(1,0,0) args ASM-verified. Structural divergences are behavior-equivalent: 0xC9
 * handled before the switch (vs in-tree), case 5/6 uses `option-4` (vs split constants), EXIT-TO-MAP
 * re-reads `RemBitsConfig0 |=` (vs our cached remCfg0) yielding identical bits (0xC000000 baseline,
 * +ADVENTURE_CUP, +ADVENTURE_BOSS 0x80000000), and prevLEV passed as int (LEV ids fit a short).
 */
#include "../uitypes.h"

extern struct GameTracker *D_8008D2AC;      /* gGT (absolute) */

/* gp-relative globals (defined =0 so patch_gprel resolves via GP_VALUE=0x8008CF6C) */
int   D_8008D52C = 0;   /* gp+0x5C0 g_nFramesSinceRaceEnded */
short D_8008D570 = 0;   /* gp+0x604 g_nNumIconsEOR */
int   D_8008D4BC = 0;   /* gp+0x550 g_nMenuReadyToPass */

/* absolute globals */
extern int   D_8008D0F8;   /* Loading.stage */
extern unsigned int D_8008D100;   /* Loading.OnBegin.AddBitsConfig0 */
extern unsigned int D_8008D104;   /* Loading.OnBegin.RemBitsConfig0 */
extern unsigned int D_8008D108;   /* Loading.OnBegin.AddBitsConfig8 */
extern unsigned int D_8008D10C;   /* Loading.OnBegin.RemBitsConfig8 */
extern short D_8008D740;   /* g_bGhostsDrawing */
extern char *D_8008D754;   /* ptrGhostTapePlaying */
extern short D_8008D958;   /* g_bPlayGhost */
extern short D_8008D97C;   /* sdata_mainMenuState */
extern char *D_8008FBF4;   /* GhostRecording.ptrGhost */
extern short D_80086E86;   /* g_awCharacterIDs[1] */
extern struct RectMenu D_80085BB4;   /* data_menuBox_GhostSelection */
extern struct RectMenu *D_8008D924;  /* sdata_ptrDesiredMenuBox */

extern void *memcpy(void *dst, const void *src, unsigned int n);
extern void func_800469C8(struct RectMenu *m);   /* RECTMENU_HideMenu */
extern int  func_80043F28(void);                 /* RaceFlag_IsFullyOffScreen */
extern void func_80043FB0(int on);               /* RaceFlag_BeginTransition */
extern void func_8002C8A8(int a, int b, int c);  /* howl_StopAudio */
extern void func_80028410(void);                 /* GhostTape_Destroy */
extern void func_8003CFC0(int levID);            /* MainRaceTrack_RequestLoad */
extern void func_80048E2C(int mode);             /* SelectProfile_ToggleMode */

void UI_RaceEnd_MenuProc(struct RectMenu *menu)
{
    short option;
    unsigned int remCfg0;
    unsigned short ds;

    if (menu->unk1e != 0) {
        ds = menu->drawStyle & 0xFEFF;
        menu->drawStyle = ds;
        if (2 < D_8008D2AC->numPlyrCurrGame) {
            menu->drawStyle = ds | 0x100;
        }
        return;
    }
    if (menu->rowSelected < 0) return;
    if (menu->rows[menu->rowSelected].stringIndex != 9) {
        func_800469C8(menu);
    }
    D_8008D52C = 0;
    D_8008D570 = 1;
    option = menu->rows[menu->rowSelected].stringIndex;
    switch (option) {
    case 4:
        D_8008D2AC->hudFlags &= 0xFE;
        if (func_80043F28() == 1) {
            func_80043FB0(1);
        }
        D_8008D0F8 = -5;
        func_8002C8A8(1, 0, 0);
        if ((D_8008D2AC->gameModeEnd & 1) == 0) return;
        D_8008D958 = 1;
        memcpy(D_8008D754, D_8008FBF4, 0x3e00);
        D_8008D740 = 0;
        D_80086E86 = *(short *)(D_8008D754 + 6);
        return;
    case 0xd: {
        struct GameTracker *gGT = D_8008D2AC;
        unsigned int gm1;
        D_8008D100 |= 0x100000;
        remCfg0 = D_8008D104 | 0xC000000;
        gm1 = gGT->gameMode1;
        D_8008D104 = remCfg0;
        D_8008D10C |= 8;
        if ((gm1 & 0x10000000) != 0) {
            D_8008D104 = remCfg0 | 0x10000000;
            func_8003CFC0(0x19);
            return;
        }
        if ((int)gm1 < 0) {
            D_8008D104 = remCfg0 | 0x80000000;
            D_8008D108 |= 1;
        }
        func_8003CFC0((short)gGT->prevLEV);
        return;
    }
    case 6:
        func_80028410();
        D_8008D97C = 2;
        D_8008D100 |= 0x2000;
        func_8003CFC0(0x27);
        return;
    case 5:
        func_80028410();
        D_8008D97C = 1;
        D_8008D100 |= 0x2000;
        func_8003CFC0(0x27);
        return;
    case 10:
        D_8008D97C = 3;
        D_8008D100 |= 0x2000;
        func_8003CFC0(0x27);
        return;
    case 3:
        func_80028410();
        D_8008D97C = 0;
        func_8003CFC0(0x27);
        return;
    case 0xc9:
        D_8008D4BC |= 1;
        return;
    case 9:
        D_8008D52C = 0x3F9;
        func_80048E2C(0x31);
        D_8008D924 = &D_80085BB4;
        return;
    default:
        return;
    }
}
