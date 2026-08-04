#include "../uitypes.h"

#define START_OF_RACE 0x40
#define ACTION_BOT    0x100000
#define ACTION_RACE_FINISHED 0x2000000
#define ACTION_BACKWARDS 0x100
#define HIDE_MODEL    0x80
#define FONT_BIG      1
#define FONT_SMALL    2
#define ITEM_NONE     0xf
#define BTN_TRIANGLE  0x40000

extern struct GameTracker *D_8008D2AC;          /* sdata_gGT */
extern short *D_8008625C[];                      /* g_apHudLayout (short*[4]) */
extern short D_800862B8[];                       /* g_awRankIconsDesired */
extern short D_800862C8[];                       /* g_awRankIconsCurr */
extern short D_800862D8[];                       /* g_awRankIconsTransitionTimer */
extern unsigned char D_8008D69C[];               /* g_abKartSpawnOrder */
extern int D_8008D000;                           /* g_dwHudDisplayFlags */
extern short D_8008D2A8;                          /* g_nTurboPadIndicator */
extern struct Gamepad *D_8008D2B0;               /* sdata_gGamepads (stride 0x50; buttonsTapped@0x14) */
extern char **D_8008D878;                        /* sdata_lngStrings */
extern int *D_80081D70[];                        /* data_ptrColor (int*[] 4-int color rows) */
extern char D_8008D530;                          /* g_szFormatMinusLong */
extern char D_8008D538;                          /* g_szFormatPlusLong */
extern char D_8008D540;                          /* g_szFormatMinusInt */
extern char D_8008D544;                          /* g_szText999 */
extern char D_8008D54C;                          /* g_szFormatIntSpace */
extern char D_8008D550;                          /* g_szFormatString */

/* gp-relative (defined = 0; the 0x8008 image, patch_gprel resolves via GP). */
struct Instance *D_8008D9C4 = 0;                 /* g_apHudLetterInst (gp+0xA58) */
struct Instance *D_8008D9C8 = 0;                 /* g_pHudLetterInstR (gp+0xA5C) */
struct Instance *D_8008D9CC = 0;                 /* g_pHudLetterInstC (gp+0xA60) */
char D_8008D9DC = 0;                             /* g_bBackwardsWarnPrevState (gp+0xA70) */
int D_8008D9E4 = 0;                              /* _g_nBackwardsWarnFrameCount (gp+0xA78) */

extern void func_8004E0E0(void);                                              /* UI_WeaponBG_AnimateShine */
extern void func_800524C4(void);                                              /* UI_DrawRankedDrivers */
extern void func_80051C64(struct Driver *d);                                  /* UI_JumpMeter_Update */
extern void func_80022878(char *str, short x, short y, int font, int color);  /* DecalFont_DrawLine */
extern void func_800511C0(short x, short y, struct Driver *d);                /* UI_DrawSpeedNeedle */
extern void func_80051E24(short x, short y, struct Driver *d);                /* UI_JumpMeter_Draw */
extern void func_80052250(short x, short y, struct Driver *d);                /* UI_DrawSlideMeter */
extern void func_800516AC(int x, int y, struct Driver *d);                    /* UI_DrawSpeedBG */
extern void func_80050C20(int x, int y, int d);                               /* UI_DrawNumWumpa */
extern void func_80050E6C(int x, int y, struct Driver *d);                    /* UI_DrawNumTimebox */
extern void func_8004EC18(short *out, short x0, short y0, short x1, short y1, int t, int dur); /* UI_Lerp2D_HUD */
extern unsigned int func_800348E8(void);                                      /* LOAD_IsOpen_RacingOrBattle */
extern void func_800ABEFC(struct Driver *d, int a);                           /* RB_Player_ModifyWumpa */
extern void func_80028468(int id, int a);                                     /* OtherFX_Play */
extern void func_80022DB0(struct Icon *icon, int x, int y, struct PrimMem *pm,
                          unsigned long *ot, int p6, int p7);                  /* DecalHUD_DrawPolyFT4 */
extern int func_8004CAA8(int v, int range);                                   /* UI_ConvertX_2 */
extern int func_8004CAC8(int v, int range);                                   /* UI_ConvertY_2 */
extern void func_800507E0(short x, short y, short scale, struct Driver *d);   /* UI_Weapon_DrawSelf */
extern int func_80078348(char *buf, const char *fmt, ...);                    /* sprintf */
extern void func_80050528(short x, short y, int unused, struct Driver *d);    /* UI_DrawLapCount */
extern void func_80050654(int x, int y, int d);                               /* UI_DrawBattleScores */
extern void func_800A06F8(int id, int a);                                     /* AA_EndEvent_DisplayTime */
extern void func_8005045C(short x, short y, struct Driver *d, unsigned short flags); /* UI_DrawPosSuffix */
extern void func_80023054(struct Icon *icon, short x, short y, struct PrimMem *pm,
                          unsigned long *ot, unsigned int c0, unsigned int c1, unsigned int c2,
                          unsigned int c3, char one, int val);                /* DecalHUD_DrawPolyGT4 */
extern void func_8004FD34(struct Driver *d);                                  /* UI_TrackerSelf */
extern void func_8004F9D8(struct Driver *d);                                  /* UI_BattleDrawHeadArrows */
extern void func_80050AF8(short x, short y, short scale, struct Driver *d);   /* UI_Weapon_DrawBG */
extern void func_8004EDAC(unsigned short x, unsigned short y, unsigned int f, struct Driver *d); /* UI_DrawRaceClock */
extern int func_800224D0(char *str, int font);                                /* DecalFont_GetLineWidth */
extern void func_8004ECD4(short *out, short x0, short y0, short x1, short y1, int t, short dur); /* UI_Lerp2D_Linear */
extern void func_8004F894(int x, int y, int fontType);                        /* UI_DrawLimitClock */
extern void func_8004DD5C(unsigned long *mapCtx, struct Thread *tl, short *ctr); /* UI_Map_DrawDrivers */
extern void func_8004DEE8(unsigned long *ot, struct Thread *t);               /* UI_Map_DrawGhosts */
extern void func_8004DFFC(unsigned long *ot, struct Thread *t);               /* UI_Map_DrawTracking */
extern void func_8004D614(struct Icon *top, struct Icon *bottom, short x, short y,
                          struct PrimMem *pm, unsigned long *ot, int color);  /* UI_Map_DrawMap */
extern void func_80028844(int id);                                            /* OtherFX_Stop2 */

/*
 * UI_RenderFrame_Racing @ 0x80052F98 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg385, score 46105 — proportionate to the 1216-insn size,
 * the largest UI function). Master per-frame HUD renderer; drives nearly the whole UI_* HUD family.
 * Layout from g_apHudLayout[numPlyrCurrGame-1] (short*[4]); each driver's slot is a short[0x50]
 * stepped +0x50 per split-screen viewport (pHudSlot). Per-driver loop over threadBuckets[0]: backwards
 * warning, JumpMeter_Update, 1P instrument cluster, wumpa/letter/timebox/battle-score pickup fly-ins,
 * Weapon_DrawSelf, LapCount/BattleScores, PosSuffix + finish-rank icon, TrackerSelf, BattleHeadArrows,
 * Weapon_DrawBG. Then clock (1P DrawRaceClock + turbo-pad POLY_G4 tab / battle DrawLimitClock), minimap
 * (1P map-on / 3P), and the end-of-race quit-prompt loop.
 *
 * NOTE(claude): the state global the Ghidra shows as struct fields is 5 discrete gp-relative scalars
 * (g_apHudLetterInst/R/C @gp+0xA58/A5C/A60, g_bBackwardsWarnPrevState @gp+0xA70, backwards-warn frame
 * count @gp+0xA78) — DEFINED=0. tileView[driverId] IS gGT->pushBuffer[driverId]. numTurbos @0x4E in
 * the US 926 (driver.h 0x54 = EUR/JPN region diff) + jumpTimer @0x3a via raw-offset. data_ptrColor
 * rows are 4-int block-copied (lwl/lwr). Wumpa-cooldown fly-in lays the Lerp path as fall-through.
 * CORRECTION(claude): three gameMode1 bit-masks were mis-transcribed in the first pass and later
 * caught by an adversarial re-audit (they hid behind Br/St 0 because a wrong mask IMMEDIATE lands in
 * register/insertion cost, not branch/stack): the battle-score-popup gate is
 * (LIFE_LIMIT|BATTLE_MODE|PAUSE_1..4)==BATTLE_MODE = 0x802F==0x20 (was 0x4020023), its inner
 * life-limit format-select is LIFE_LIMIT 0x8000 (was 0x2000), and the end-of-race quit-arm is
 * END_OF_RACE 0x200000 (was 0x20000) — all now cross-verified vs the SCUS asm AND the Ghidra
 * GameMode1 enum. RESIDUAL is the register-coloring wall + the turbo-pad tab vertex-write scheduling +
 * the turbo-indicator goto layout — all non-behavioral. Verified vs decomp UI_44. Phase-B (saphi
 * game/UI/UI_RenderFrame.c): CLEAN. (926/NTSC-U)
 */
void UI_RenderFrame_Racing(void)
{
    unsigned long *ot;
    int **ppColorEntry;
    unsigned char *pSpawnOrderByte;
    struct SpawnType1 *pMapSpawn;
    short sPosX;
    int iFrameScratch;
    struct Instance *pLetterInstR;
    unsigned int gameMode;
    struct Icon *pRankIcon;
    unsigned int *pPrimCurr;
    short sPosY;
    short *pRankTimerCur;
    char *pFormat;
    struct Icon *mapBottom;
    short *pRankDesiredCur;
    unsigned int *pTurboTabPrim;
    int iScratch;
    unsigned short flags;
    short *pRankCurrCur;
    struct PushBuffer *pBackBuf;
    unsigned int uColorScratch;
    unsigned int color1;
    unsigned int color2;
    unsigned int color3;
    short sColorIdx;
    short *pHudSlot;
    struct Instance *pLetterTarget;
    int iDriverPtr;
    struct Driver *pDriver;
    int iEorByteOfs;
    struct Thread *pKartThread;
    int iGamepadOfs;
    unsigned long *pMapOt;
    char cBackwardsWarn;
    short popupX;
    short popupY;
    short letterX;
    short letterY;
    char acStack_50[24];
    unsigned short turboPadX;
    short turboPadY;
    short mapCounter[4];
    int bAnyKartJumping;
    unsigned char driverId;
    struct GameTracker *pGt;
    struct Instance *pLetterInstC;

    pHudSlot = D_8008625C[(unsigned char)D_8008D2AC->numPlyrCurrGame - 1];
    pMapOt = 0;
    func_8004E0E0();
    if (D_8008D2AC->elapsedEventTime == 0) {
        iFrameScratch = 0;
        pRankCurrCur = D_800862C8;
        pRankDesiredCur = D_800862B8;
        pRankTimerCur = D_800862D8;
        do {
            *pRankTimerCur = 0;
            pRankTimerCur = pRankTimerCur + 1;
            pSpawnOrderByte = D_8008D69C + iFrameScratch;
            iFrameScratch = iFrameScratch + 1;
            *pRankDesiredCur = (unsigned short)*pSpawnOrderByte;
            pRankDesiredCur = pRankDesiredCur + 1;
            *pRankCurrCur = (unsigned short)*pSpawnOrderByte;
            pRankCurrCur = pRankCurrCur + 1;
        } while (iFrameScratch < 8);
    }
    if ((D_8008D2AC->gameMode1 & START_OF_RACE) == 0) {
        if ((D_8008D2AC->hudFlags & 0x20) == 0) {
            if ((D_8008D2B0->buttonsTapped & BTN_TRIANGLE) != 0) {
                D_8008D000 = D_8008D000 ^ 8;
            }
        } else {
            D_8008D2AC->hudFlags = D_8008D2AC->hudFlags & 0xdf;
        }
    }
    if ((D_8008D2AC->numPlyrCurrGame == 0) &&
        ((D_8008D2AC->drivers[0]->actionsFlagSet & ACTION_BOT) != 0)) {
        D_8008D000 = 8;
    }
    pMapSpawn = D_8008D2AC->level1->ptrSpawnType1;
    if (pMapSpawn->count != 0) {
        pMapOt = (unsigned long *)pMapSpawn[1].count;
    }
    if ((D_8008D2AC->gameMode1 & 0x4020020) == 0) {
        func_800524C4();
    }
    pKartThread = D_8008D2AC->threadBucket0Thread;
    cBackwardsWarn = 0;
    if (pKartThread != 0) {
        do {
            pDriver = pKartThread->object;
            if (((int)pDriver->distanceDrivenBackwards < 0x1f5) ||
                ((pDriver->actionsFlagSet & ACTION_BACKWARDS) == 0)) {
LAB_80053260:
                if ((D_8008D2AC->gameMode1 & 0xf) == 0) {
                    func_80051C64(pDriver);
                }
            } else if ((D_8008D2AC->gameMode1 & 0xf) == 0) {
                driverId = pDriver->driverID;
                if ((D_8008D2AC->elapsedEventTime & 0x80U) != 0) {
                    func_80022878(D_8008D878[0x1d],
                                  (short)(((unsigned int)D_8008D2AC->pushBuffer[driverId].rectX +
                                           ((int)((unsigned int)D_8008D2AC->pushBuffer[driverId].rectW <<
                                                  0x10) >> 0x11)) * 0x10000 >> 0x10),
                                  (short)(((unsigned int)D_8008D2AC->pushBuffer[driverId].rectY +
                                           ((int)((unsigned int)D_8008D2AC->pushBuffer[driverId].rectH <<
                                                  0x10) >> 0x11) + -0x1e) * 0x10000 >> 0x10),
                                  FONT_BIG, 0x8000);
                }
                cBackwardsWarn = 1;
                goto LAB_80053260;
            }
            if (((unsigned char)D_8008D2AC->numPlyrCurrGame < 2) && ((D_8008D000 & 8) != 0)) {
                func_800511C0(pHudSlot[0x24], pHudSlot[0x25], pDriver);
                func_80051E24(pHudSlot[0x18], pHudSlot[0x19], pDriver);
                func_80052250(pHudSlot[0x20], pHudSlot[0x21], pDriver);
                func_800516AC((int)pHudSlot[0x24], (int)pHudSlot[0x25], pDriver);
            }
            if ((pDriver->actionsFlagSet & ACTION_RACE_FINISHED) == 0) {
                if ((D_8008D2AC->gameMode1 & 0x20) == 0) {
                    func_80052250(pHudSlot[0x20], pHudSlot[0x21], pDriver);
                }
                if ((D_8008D2AC->gameMode1 & 0x4020000) == 0) {
                    func_80050C20(pHudSlot[0x10], pHudSlot[0x11], (int)pDriver);
                }
            }
            if ((D_8008D2AC->gameMode1 & 0x4000000) != 0) {
                func_80050E6C(pHudSlot[0x4c], pHudSlot[0x4d], pDriver);
            }
            if ((D_8008D2AC->gameMode1 & 0xf) == 0) {
                if (pDriver->PickupWumpaHUD.numCollected != 0) {
                    popupX = pHudSlot[0xc];
                    popupY = pHudSlot[0xd];
                    if (pDriver->PickupWumpaHUD.cooldown != 0) {
                        func_8004EC18(&popupX, pDriver->PickupWumpaHUD.startX, pDriver->PickupWumpaHUD.startY,
                                      pHudSlot[0xc], pHudSlot[0xd], pDriver->PickupWumpaHUD.cooldown, 5);
                        iFrameScratch = pDriver->PickupWumpaHUD.cooldown + -1;
LAB_80053498:
                        pDriver->PickupWumpaHUD.cooldown = iFrameScratch;
                    } else {
                        pDriver->PickupWumpaHUD.numCollected = pDriver->PickupWumpaHUD.numCollected + -1;
                        uColorScratch = func_800348E8();
                        if ((uColorScratch != 0) && ((D_8008D2AC->gameMode1 & 0x100000) == 0)) {
                            func_800ABEFC(pDriver, 1);
                        }
                        func_80028468(0x42, 1);
                        iFrameScratch = 5;
                        if (pDriver->PickupWumpaHUD.numCollected != 0) goto LAB_80053498;
                    }
                    func_80022DB0(D_8008D2AC->iconGroup[0xb]->icons[0], popupX, popupY,
                                  &D_8008D2AC->backBuffer->primMem, D_8008D2AC->pushBuffer_UI.ptrOT,
                                  0, pHudSlot[3]);
                }
                pLetterInstC = D_8008D9CC;
                pLetterInstR = D_8008D9C8;
                pLetterTarget = D_8008D9C4;
                if (pDriver->PickupLetterHUD.cooldown != 0) {
                    sPosX = pDriver->PickupLetterHUD.modelID;
                    if (sPosX == 0x93) {
                        D_8008D9CC->flags = D_8008D9CC->flags & ~HIDE_MODEL;
                        letterX = pHudSlot[0x48];
                        pLetterInstR = pLetterInstC;
LAB_80053584:
                        letterY = pHudSlot[0x49];
                        pLetterTarget = pLetterInstR;
                    } else {
                        if (sPosX != 0x94) {
                            D_8008D9C8->flags = D_8008D9C8->flags & ~HIDE_MODEL;
                            letterX = pHudSlot[0x48] + 0x3a;
                            goto LAB_80053584;
                        }
                        D_8008D9C4->flags = D_8008D9C4->flags & ~HIDE_MODEL;
                        letterX = pHudSlot[0x48] + 0x1d;
                        letterY = pHudSlot[0x49] + -1;
                    }
                    pDriver->PickupLetterHUD.cooldown = pDriver->PickupLetterHUD.cooldown + -1;
                    func_8004EC18(&letterX, pDriver->PickupLetterHUD.startX, pDriver->PickupLetterHUD.startY,
                                  letterX, letterY, (int)pDriver->PickupLetterHUD.cooldown, 10);
                    iFrameScratch = func_8004CAA8((int)letterX, 0x200);
                    pLetterTarget->matrix.t[0] = iFrameScratch;
                    iFrameScratch = func_8004CAC8((int)letterY, 0x200);
                    pLetterTarget->matrix.t[1] = iFrameScratch;
                    pLetterTarget->matrix.t[2] = 0x200;
                }
            }
            if ((D_8008D2AC->gameMode1 & 0x4000000) == 0) {
                if ((pDriver->actionsFlagSet & ACTION_RACE_FINISHED) == 0) {
                    func_800507E0(pHudSlot[0], pHudSlot[1], pHudSlot[3], pDriver);
                }
            } else if (pDriver->PickupTimeboxHUD.cooldown != 0) {
                func_80078348(acStack_50, &D_8008D530, D_8008D2AC->timeCrateTypeSmashed);
                func_8004EC18(&popupX, pDriver->PickupTimeboxHUD.startX, pDriver->PickupTimeboxHUD.startY,
                              0x14, 8, pDriver->PickupTimeboxHUD.cooldown, 10);
                pDriver->PickupTimeboxHUD.cooldown = pDriver->PickupTimeboxHUD.cooldown + -1;
                func_80022878(acStack_50, popupX, popupY, FONT_BIG, 1);
            }
            if ((D_8008D2AC->gameMode1 & 0x802f) == 0x20) {
                if (pDriver->BattleHUD.cooldown == 0) {
                    pDriver->BattleHUD.scoreDelta = 0;
                } else {
                    popupX = pHudSlot[0x34] + 0x20;
                    popupY = pHudSlot[0x35];
                    if ((D_8008D2AC->gameMode1 & 0x8000) == 0) {
                        iFrameScratch = pDriver->BattleHUD.scoreDelta;
                        if (iFrameScratch < 1) {
                            pFormat = &D_8008D540;
                            if (iFrameScratch < 0) {
                                iFrameScratch = -iFrameScratch;
                            }
                        } else {
                            pFormat = &D_8008D538;
                        }
                    } else {
                        iFrameScratch = pDriver->BattleHUD.scoreDelta;
                        pFormat = &D_8008D530;
                    }
                    func_80078348((char *)&letterX, pFormat, iFrameScratch);
                    func_8004EC18(&popupX, pDriver->BattleHUD.startX, pDriver->BattleHUD.startY,
                                  pHudSlot[0x34] + 0x20, pHudSlot[0x35] + 8, pDriver->BattleHUD.cooldown, 5);
                    pDriver->BattleHUD.cooldown = pDriver->BattleHUD.cooldown + -1;
                    func_80022878((char *)&letterX, popupX, popupY, FONT_SMALL, 3);
                }
            }
            if ((D_8008D2AC->gameMode1 & 0x20) == 0) {
                if ((pDriver->actionsFlagSet & ACTION_RACE_FINISHED) == 0) {
                    func_80050528(pHudSlot[4], pHudSlot[5], (int)pHudSlot[7], pDriver);
                }
            } else {
                func_80050654(pHudSlot[0x34], pHudSlot[0x35], (int)pDriver);
            }
            if (((D_8008D2AC->gameMode1 & 0x480000) != 0) &&
                ((pDriver->actionsFlagSet & ACTION_RACE_FINISHED) != 0)) {
                func_800A06F8(pDriver->driverID, 0);
            }
            gameMode = D_8008D2AC->gameMode1;
            if ((gameMode & 0x4020020) == 0) {
                if (((pDriver->actionsFlagSet & ACTION_RACE_FINISHED) == 0) ||
                    ((D_8008D2AC->numPlyrCurrGame == 2) && ((gameMode & 0x400000) == 0))) {
                    sColorIdx = 0;
                    sPosX = pHudSlot[0x14];
                    sPosY = pHudSlot[0x15];
                    flags = 0;
                } else {
                    if ((unsigned char)D_8008D2AC->numPlyrCurrGame < 3) goto LAB_80053af4;
                    sPosX = pHudSlot[0x14];
                    sPosY = pHudSlot[0x15];
                    bAnyKartJumping = (D_8008D2AC->timer & 1U) == 0;
                    sColorIdx = (unsigned short)bAnyKartJumping << 2;
                    flags = (unsigned short)bAnyKartJumping << 2;
                }
                func_8005045C(sPosX, sPosY, pDriver, flags);
                if (2 < (unsigned char)D_8008D2AC->numPlyrCurrGame) {
                    ot = D_8008D2AC->pushBuffer_UI.ptrOT;
                    sPosX = pHudSlot[8];
                    sPosY = pHudSlot[9];
                    pBackBuf = D_8008D2AC->backBuffer;
                    ppColorEntry = D_80081D70 + sColorIdx;
                    uColorScratch = **ppColorEntry;
                    color1 = (*ppColorEntry)[1];
                    color2 = (*ppColorEntry)[2];
                    color3 = (*ppColorEntry)[3];
                    pRankIcon = D_8008D2AC->ptrIcons[pDriver->driverRank + 0x19];
LAB_80053aec:
                    func_80023054(pRankIcon, sPosX, sPosY, &pBackBuf->primMem, ot,
                                  uColorScratch, color1, color2, color3, 0, 0x1000);
                }
            } else if ((gameMode & 0x200020) == 0x200020) {
                bAnyKartJumping = (D_8008D2AC->timer & 1U) == 0;
                func_8005045C(pHudSlot[0x14], pHudSlot[0x15], pDriver, (unsigned short)bAnyKartJumping << 2);
                sPosX = pHudSlot[8];
                sPosY = pHudSlot[9];
                ot = D_8008D2AC->pushBuffer_UI.ptrOT;
                pBackBuf = D_8008D2AC->backBuffer;
                ppColorEntry = D_80081D70 + (unsigned int)bAnyKartJumping * 4;
                uColorScratch = **ppColorEntry;
                color1 = (*ppColorEntry)[1];
                color2 = (*ppColorEntry)[2];
                color3 = (*ppColorEntry)[3];
                pRankIcon = D_8008D2AC->ptrIcons
                            [D_8008D2AC->finishedRankOfEachTeam[pDriver->BattleHUD.teamID] + 0x19];
                goto LAB_80053aec;
            }
LAB_80053af4:
            func_8004FD34(pDriver);
            if ((D_8008D2AC->gameMode1 & 0x20) != 0) {
                func_8004F9D8(pDriver);
            }
            if ((9 < pDriver->numWumpas) && ((pDriver->actionsFlagSet & ACTION_RACE_FINISHED) == 0) &&
                (func_80050AF8(pHudSlot[0x30], pHudSlot[0x31], pHudSlot[0x33], pDriver),
                 pDriver->heldItemID != ITEM_NONE)) {
                func_80050AF8(pHudSlot[0x2c], pHudSlot[0x2d], pHudSlot[0x2f], pDriver);
            }
            pKartThread = pKartThread->siblingThread;
            pHudSlot = pHudSlot + 0x50;
        } while (pKartThread != 0);
    }
    if (D_8008D9DC != cBackwardsWarn) {
        D_8008D9E4 = 0;
        D_8008D9DC = cBackwardsWarn;
    }
    D_8008D9E4 = D_8008D9E4 + 1;
    if (D_8008D2AC->numPlyrCurrGame == 1) {
        pDriver = D_8008D2AC->drivers[0];
        func_8004EDAC(0x14, 8, 0, pDriver);
        pKartThread = 0;
        if ((D_8008D2AC->gameMode2 & 0x8000000) != 0) {
            sPosX = 0;
            if (*(short *)((int)pDriver + 0x4e) != 0) {
                pKartThread = D_8008D2AC->threadBucket9Thread;
                while ((pKartThread != 0) &&
                       (*(struct Driver **)((int)pKartThread->object + 4) != pDriver)) {
                    pKartThread = pKartThread->siblingThread;
                }
                sPosX = *(short *)((int)pDriver + 0x4e);
            }
            if ((sPosX < 3) || (9 < D_8008D2A8)) {
                if (pKartThread == 0) goto LAB_80053c98;
LAB_80053cac:
                sPosX = D_8008D2A8;
                if ((*(short *)((int)pDriver + 0x4e) < 3) && (0 < D_8008D2A8)) {
LAB_80053cd4:
                    sPosX = D_8008D2A8 + -1;
                }
            } else {
                sPosX = D_8008D2A8 + 1;
                if (pKartThread == 0) {
LAB_80053c98:
                    if (D_8008D2A8 < 1) goto LAB_80053cac;
                    goto LAB_80053cd4;
                }
            }
            D_8008D2A8 = sPosX;
            if (D_8008D2A8 != 0) {
                func_8004ECD4((short *)&turboPadX, 0x2c8, 0x20, 500, 0x20, (int)D_8008D2A8, 10);
                if (*(short *)((int)pDriver + 0x4e) < 1000) {
                    func_80078348((char *)&popupX, &D_8008D54C);
                } else {
                    func_80078348((char *)&popupX, &D_8008D544);
                }
                iFrameScratch = func_800224D0(D_8008D878[0x24b], 1);
                func_80022878((char *)&popupX,
                              (short)(((unsigned int)turboPadX - iFrameScratch) * 0x10000 >> 0x10),
                              turboPadY, FONT_BIG, 0x4022);
                func_80078348((char *)&popupX, &D_8008D550, D_8008D878[0x24b]);
                func_80022878((char *)&popupX, turboPadX, turboPadY, FONT_BIG, 0x4000);
                pBackBuf = D_8008D2AC->backBuffer;
                pPrimCurr = pBackBuf->primMem.curr;
                pTurboTabPrim = 0;
                if (pPrimCurr <= (unsigned int *)pBackBuf->primMem.endMin100) {
                    pBackBuf->primMem.curr = pPrimCurr + 9;
                    pTurboTabPrim = pPrimCurr;
                }
                if (pTurboTabPrim == 0) {
                    return;
                }
                pTurboTabPrim[1] = 0x3800c8ff;
                pTurboTabPrim[3] = 0x3800c8ff;
                pTurboTabPrim[5] = 0x380000ff;
                pTurboTabPrim[7] = 0x380000ff;
                *(unsigned short *)(pTurboTabPrim + 2) = turboPadX - 0xaa;
                *(short *)((int)pTurboTabPrim + 10) = turboPadY + 9;
                *(unsigned short *)(pTurboTabPrim + 4) = turboPadX + 0x32;
                *(short *)((int)pTurboTabPrim + 0x12) = turboPadY + 9;
                *(unsigned short *)(pTurboTabPrim + 6) = turboPadX - 0x96;
                *(short *)((int)pTurboTabPrim + 0x1a) = turboPadY + 0x12;
                *(unsigned short *)(pTurboTabPrim + 8) = turboPadX + 0x32;
                pGt = D_8008D2AC;
                *(short *)((int)pTurboTabPrim + 0x22) = turboPadY + 0x12;
                pPrimCurr = pGt->pushBuffer_UI.ptrOT;
                *pTurboTabPrim = *pPrimCurr | 0x8000000;
                *pPrimCurr = (unsigned int)pTurboTabPrim & 0xffffff;
            }
        }
    } else if ((D_8008D2AC->gameMode1 & 0x10000) != 0) {
        func_8004F894(0xd7, 0x68, 2);
    }
    if ((((D_8008D2AC->numPlyrCurrGame == 1) && (pMapOt != 0)) && ((D_8008D000 & 8) == 0)) ||
        ((D_8008D2AC->numPlyrCurrGame == 3) && (pMapOt != 0))) {
        mapCounter[0] = 0;
        func_8004DD5C(pMapOt, D_8008D2AC->threadBucket0Thread, mapCounter);
        func_8004DD5C(pMapOt, D_8008D2AC->threadBucket1Thread, mapCounter);
        func_8004DEE8(pMapOt, D_8008D2AC->threadBucket2Thread);
        func_8004DFFC(pMapOt, D_8008D2AC->threadBucket6Thread);
        if (pMapOt != 0) {
            if (D_8008D2AC->numPlyrCurrGame == 1) {
                pBackBuf = D_8008D2AC->backBuffer;
                pMapOt = D_8008D2AC->pushBuffer_UI.ptrOT;
                sPosX = 500;
                pRankIcon = D_8008D2AC->ptrIcons[3];
                mapBottom = D_8008D2AC->ptrIcons[4];
                sPosY = 0xc3;
            } else {
                sPosX = 0x1b8;
                if (D_8008D2AC->numPlyrCurrGame != 3) goto LAB_80054040;
                pBackBuf = D_8008D2AC->backBuffer;
                pMapOt = D_8008D2AC->pushBuffer_UI.ptrOT;
                pRankIcon = D_8008D2AC->ptrIcons[3];
                mapBottom = D_8008D2AC->ptrIcons[4];
                sPosY = 0xcd;
            }
            func_8004D614(pRankIcon, mapBottom, sPosX, sPosY, &pBackBuf->primMem, pMapOt, 1);
        }
    }
LAB_80054040:
    bAnyKartJumping = 0;
    iFrameScratch = 0;
    if (D_8008D2AC->numPlyrCurrGame != 0) {
        iGamepadOfs = 0;
        iEorByteOfs = 0;
        do {
            iDriverPtr = *(int *)((int)D_8008D2AC->drivers + iEorByteOfs);
            driverId = *(unsigned char *)(iDriverPtr + 0x4a);
            if ((((*(unsigned int *)(iDriverPtr + 0x2c8) & 0x2000000) != 0) &&
                 ((D_8008D2AC->gameMode1 & 0x420000) == 0)) &&
                ((D_8008D2AC->timerEndOfRaceVS == 0 || (0x96 < D_8008D2AC->timerEndOfRaceVS)))) {
                if (((int)*(short *)(iDriverPtr + 0x482) < (int)((unsigned char)D_8008D2AC->numPlyrCurrGame - 1)) &&
                    ((D_8008D2AC->gameMode1 & 0x20) == 0)) {
                    sPosX = (short)(((unsigned int)D_8008D2AC->pushBuffer[driverId].rectX +
                                     ((int)((unsigned int)D_8008D2AC->pushBuffer[driverId].rectW << 0x10) >> 0x11)
                                    ) * 0x10000 >> 0x10);
                    iScratch = (unsigned int)D_8008D2AC->pushBuffer[driverId].rectY +
                               ((int)((unsigned int)D_8008D2AC->pushBuffer[driverId].rectH << 0x10) >> 0x11);
                    pFormat = D_8008D878[0x1e];
                } else {
                    sPosX = (short)(((unsigned int)D_8008D2AC->pushBuffer[driverId].rectX +
                                     ((int)((unsigned int)D_8008D2AC->pushBuffer[driverId].rectW << 0x10) >> 0x11)
                                    ) * 0x10000 >> 0x10);
                    iScratch = (unsigned int)D_8008D2AC->pushBuffer[driverId].rectY +
                               ((int)((unsigned int)D_8008D2AC->pushBuffer[driverId].rectH << 0x10) >> 0x11);
                    pFormat = D_8008D878[0x143];
                }
                func_80022878(pFormat, sPosX, (short)((unsigned int)((iScratch + -0x1e) * 0x10000) >> 0x10),
                              FONT_BIG, 0x8000);
                if (((*(unsigned int *)((int)D_8008D2B0 + iGamepadOfs + 0x14) & 0x1010) != 0) &&
                    ((D_8008D2AC->gameMode1 & 0x200000) != 0)) {
                    D_8008D2AC->timerEndOfRaceVS = 0x96;
                }
            }
            if (*(short *)(iDriverPtr + 0x3a) != 0) {
                bAnyKartJumping = 1;
            }
            iGamepadOfs = iGamepadOfs + 0x50;
            iFrameScratch = iFrameScratch + 1;
            iEorByteOfs = iEorByteOfs + 4;
        } while (iFrameScratch < (int)(unsigned int)(unsigned char)D_8008D2AC->numPlyrCurrGame);
    }
    if ((((D_8008D2AC->gameMode1 & 0xf) == 0) && (bAnyKartJumping == 0)) &&
        ((D_8008D2AC->gameMode1 & 0x800000) != 0)) {
        func_80028844(0x5d);
        D_8008D2AC->gameMode1 = D_8008D2AC->gameMode1 & 0xff7fffff;
    }
    return;
}
