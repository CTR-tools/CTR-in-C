// MainFreeze_MenuPtrDefault @ 0x80039a44  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 6690, Reg 54; grind floor ~5865 non-natural).
// Residual: whole-function gGT-reload register allocation (retail reloads gp+0x340
// per gGT access: +6 lw / +5 sw / +3 lui vs my consolidated loads) + menuBox-in-temp
// caching + switch scheduling. Behavior identical (target 226 insns vs 201).
// Pause-menu DEFAULT proc: cooldown gate; phase!=0 draw (drawStyle bit 0x100 by
// >2P, AdvArena+!NEEDS_TO_CLOSE+LOAD_IsOpen_AdvHub -> AH_Pause_Update); phase 0
// selection: stringID 14->OptionsMenu(row 8), 11/12->HintMenu, 3->QuitMenu(row 1);
// else Hide+SafeAdvDestroy + switch(stringID): 1/4 restart(stage=-5, ghost P2
// char-copy), 2 resume, 5/6/10 quit-to-menu (mainMenuState 1/2/3, AddBits|=0x2000),
// 13 exit-to-map (Loading.OnBegin bit config incl boss/adv-cup branch), then
// MainRaceTrack_RequestLoad. gGT/Loading gp-relative; the 13-entry jtbl indexes
// (stringID-1). Verified: Loading.OnBegin@gp+0x194, ptrGhostTapePlaying char@+6.
#include "CTR.h"

#define PAUSE_1         0x1
#define ADVENTURE_ARENA 0x100000
#define ADVENTURE_CUP   0x10000000
#define NEEDS_TO_CLOSE  0x1000

struct RectRow { u16 stringIndex; char pad[6 - 2]; };   /* stride 6 */

struct RectMenu {
    char pad0[0x8];
    int state;                /* 0x08 */
    struct RectRow *rows;     /* 0x0C */
    char pad_14[0x14 - 0x10];
    u16 drawStyle;            /* 0x14 */
    char pad_16[0x1A - 0x16];
    short rowSelected;        /* 0x1A */
    char pad_1C[0x1E - 0x1C];
    short nProcCallPhase;     /* 0x1E */
};

struct GhostTape { char pad0[6]; u16 characterID; };   /* characterID @0x6 */

struct GameTracker {
    int gameMode1;                              /* 0x0    */
    char pad_1A10[0x1A10 - 0x4];
    int levelID;                                /* 0x1A10 */
    char pad_1CA8[0x1CA8 - 0x1A14];
    unsigned char numPlyrCurrGame;              /* 0x1CA8 */
    char pad_1D38[0x1D38 - 0x1CA9];
    unsigned char cooldownfromPauseUntilUnpause;/* 0x1D38 */
    unsigned char cooldownFromUnpauseUntilPause;/* 0x1D39 */
    char pad_1E58[0x1E58 - 0x1D3A];
    int cupID;                                  /* 0x1E58 (cup.cupID) */
    char pad_1EB4[0x1EB4 - 0x1E5C];
    short prevLEV;                              /* 0x1EB4 */
};

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */
int D_8008D0F8 = 0;   /* Loading.stage (gp+0x18C); LOAD_RESTART = -5 */
int D_8008D100 = 0;   /* Loading.OnBegin.AddBitsConfig0 (gp+0x194) */
int D_8008D104 = 0;   /* Loading.OnBegin.RemBitsConfig0 (gp+0x198) */
int D_8008D108 = 0;   /* Loading.OnBegin.AddBitsConfig8 (gp+0x19C) */
int D_8008D10C = 0;   /* Loading.OnBegin.RemBitsConfig8 (gp+0x1A0) */

extern short D_8008D958;              /* sdata->boolReplayHumanGhost (lh) */
extern struct GhostTape *D_8008D754; /* sdata->ptrGhostTapePlaying */
extern short D_80086E86;             /* data.characterIDs[1] */
extern short D_8008D97C;             /* sdata->mainMenuState */
extern struct RectMenu *D_8008D924;  /* sdata->ptrDesiredMenu */
extern struct RectMenu D_80084190;   /* data.menuRacingWheelConfig */
extern char D_800B518C[];            /* D232.menuHintMenu */
extern struct RectMenu D_800841D0;   /* data.menuQuit */

extern int  func_80034920(void);                 /* LOAD_IsOpen_AdvHub */
extern void func_800B3144(void);                 /* AH_Pause_Update */
extern void func_800469C8(struct RectMenu *);    /* RECTMENU_Hide */
extern void func_800399FC(void);                 /* MainFreeze_SafeAdvDestroy */
extern int  func_80043F28(void);                 /* RaceFlag_IsFullyOffScreen */
extern void func_80043FB0(int);                  /* RaceFlag_BeginTransition */
extern void func_80024C08(struct GameTracker *); /* ElimBG_Deactivate */
extern void func_80034B48(int);                  /* MainFrame_TogglePauseAudio */
extern void func_80028468(int, int);             /* OtherFX_Play */
extern void func_80028410(void);                 /* GhostTape_Destroy */
extern void func_8003CFC0(int);                  /* MainRaceTrack_RequestLoad */

void MainFreeze_MenuPtrDefault(struct RectMenu *menu)
{
    struct GameTracker *gGT = D_8008D2AC;
    short levID = 0;
    u16 stringID;

    if (gGT->cooldownfromPauseUntilUnpause != 0)
        return;

    if (menu->nProcCallPhase != 0)
    {
        u16 style = menu->drawStyle & 0xFEFF;
        menu->drawStyle = style;
        if (gGT->numPlyrCurrGame > 2)
            menu->drawStyle = style | 0x100;
        if ((D_8008D2AC->gameMode1 & ADVENTURE_ARENA) == 0 || (menu->state & NEEDS_TO_CLOSE))
            return;
        if (func_80034920() == 0)
            return;
        func_800B3144();
        return;
    }

    if (menu->rowSelected < 0)
        return;

    stringID = menu->rows[menu->rowSelected].stringIndex;

    if (stringID == 14)
    {
        D_8008D924 = &D_80084190;
        D_80084190.rowSelected = 8;
        return;
    }
    if (stringID == 11 || stringID == 12)
    {
        D_8008D924 = (struct RectMenu *)D_800B518C;
        return;
    }
    if (stringID == 3)
    {
        D_8008D924 = &D_800841D0;
        D_800841D0.rowSelected = 1;
        return;
    }

    D_8008D2AC->cooldownFromUnpauseUntilPause = 5;
    func_800469C8(menu);
    func_800399FC();

    switch (stringID)
    {
    case 1:
    case 4:
        D_8008D2AC->gameMode1 &= ~PAUSE_1;
        if (func_80043F28() == 1)
            func_80043FB0(1);
        D_8008D0F8 = -5;
        if (D_8008D958 == 0)
            return;
        if (D_8008D754 == 0)
            return;
        D_80086E86 = D_8008D754->characterID;
        return;
    case 2:
        func_80024C08(D_8008D2AC);
        D_8008D2AC->gameMode1 &= ~PAUSE_1;
        func_80034B48(0);
        func_80028468(1, 1);
        return;
    case 5:
        func_80028410();
        levID = 0x27;
        D_8008D97C = 1;
        D_8008D100 |= 0x2000;
        D_8008D2AC->gameMode1 &= ~PAUSE_1;
        break;
    case 6:
        func_80028410();
        levID = 0x27;
        D_8008D97C = 2;
        D_8008D100 |= 0x2000;
        D_8008D2AC->gameMode1 &= ~PAUSE_1;
        break;
    case 10:
        levID = 0x27;
        D_8008D97C = 3;
        D_8008D100 |= 0x2000;
        D_8008D2AC->gameMode1 &= ~PAUSE_1;
        break;
    case 13:
    {
        int rem;
        u32 gm = D_8008D2AC->gameMode1;
        D_8008D100 |= 0x100000;
        rem = D_8008D104 | 0xc000000;
        D_8008D10C |= 8;
        D_8008D104 = rem;
        D_8008D2AC->gameMode1 = gm & ~PAUSE_1;
        if ((gm & ADVENTURE_CUP) == 0)
        {
            if ((int)(gm & ~PAUSE_1) < 0)
            {
                D_8008D104 = rem | 0x80000000;
                D_8008D108 |= 1;
            }
            levID = D_8008D2AC->prevLEV;
        }
        else
        {
            levID = 0x19;
            D_8008D104 = rem | 0x10000000;
            D_8008D2AC->levelID = D_8008D2AC->cupID + 0x64;
        }
        break;
    }
    default:
        return;
    }

    func_8003CFC0(levID);
}
