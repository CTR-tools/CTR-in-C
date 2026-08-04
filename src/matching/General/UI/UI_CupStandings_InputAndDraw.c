#include "../uitypes.h"

#define CUP_ANY_KIND 0x10
#define ACTION_ARCADE 0x400000
#define FONT_BIG   1
#define FONT_SMALL 2

extern struct GameTracker *D_8008D2AC;          /* sdata_gGT */
extern struct MetaDataCharacter D_80086D84[];   /* g_anMetaDataCharacters */
extern short D_80086E84[];                       /* g_awCharacterIDs */
extern int D_80086D44[];                          /* g_anCupPositionPerPlayer[8] */
extern int D_80086D64[];                          /* g_anCupPointsPerPosition[8] */
extern struct MetaDataLEV D_80083A80[];          /* data_metaDataLEV */
extern short D_80084114[];                        /* g_aAdvCupNameIndex (record stride 8) */
extern short D_80084148[];                        /* g_aArcadeCups (record stride 0x12; +2 = trackIDs) */
extern short D_800840A4[];                        /* g_aAdvCupTrackIDs (stride 0x10) */
extern char D_8008D510;                           /* g_szFormatInt */
extern char D_8008D520;                           /* g_szFormatLong */
extern char D_8008D574;                           /* g_szFormatPlusInt */
extern char D_80011708;                           /* "%s %ld/4" */
struct UnlockRow { short v[3]; };
struct UnlockRowU { unsigned short v[3]; };
extern struct UnlockRow D_8008D578;                /* g_anArcadeCupUnlockBitBase */
extern struct UnlockRow D_8008D580;                /* g_anArcadeAllCupsGateBitBase */
extern struct UnlockRowU D_8008D588;               /* g_anArcadeBattleMapUnlockBit */
extern char **D_8008D878;                         /* sdata_lngStrings */
extern unsigned int D_8008D950;                   /* sdata_AnyPlayerTap (& 0x50) */
extern struct GameProgress D_8008E6E8;            /* sdata_gameProgress */
extern unsigned char D_8008FBA4[80];              /* g_abAdvProgress */
extern unsigned int D_8008D104;                   /* sdata_Loading.OnComplete.RemBitsConfig0 */

/* gp-relative state scalars (defined, not extern — patch_gprel resolves via GP). */
int D_8008D4BC = 0;                               /* cup phase flags (&4 running-totals, &8 advance) */
int D_8008D52C = 0;                               /* sdata_framesSinceRaceEnded (anim timer) */
short D_8008D570 = 0;                             /* sdata_numIconsEOR (row count) */

extern void func_80022878(char *str, short x, short y, int font, int color);  /* DecalFont_DrawLine */
extern void func_8003CFC0(int levID);                                          /* MainRaceTrack_RequestLoad */
extern void func_80041C84(struct GameTracker *gt);                             /* Podium_InitModels */
extern unsigned int func_80043F1C(void);                                       /* RaceFlag_IsFullyOnScreen */
extern unsigned int func_80043F28(void);                                       /* RaceFlag_IsFullyOffScreen */
extern void func_80043FB0(int a);                                              /* RaceFlag_BeginTransition */
extern void func_80044088(int a);                                              /* RaceFlag_SetCanDraw */
extern void func_800457B0(struct RECT *r, int inner, void *otMem);             /* RECTMENU_DrawInnerRect */
extern void func_80046404(void);                                               /* RECTMENU_ClearInput */
extern void func_8004E8D8(struct Icon *icon, short x, int y, struct PrimMem *pm,
                          unsigned long *ot, int one, int z, int color);       /* UI_DrawDriverIcon */
extern void func_8004ECD4(short *out, short x0, short y0, short x1, short y1, int t, short dur); /* UI_Lerp2D_Linear */
extern void func_8005607C(void);                                               /* UI_CupStandings_FinalizeCupRanks */
extern void func_80056220(void);                                               /* UI_CupStandings_UpdateCupRanks */
extern int func_80078348(char *buf, const char *fmt, ...);                     /* sprintf */

/*
 * UI_CupStandings_InputAndDraw @ 0x800562FC (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg441, score 28725). The end-of-cup "STANDINGS" screen +
 * the cup-progression state machine, driven by three gp-relative state scalars (the Ghidra
 * "g_aGpuDmaQueue[N].pInlineData[k]" struct is a MISLABEL — the 926 uses discrete gp-rel scalars):
 *   D_8008D4BC(gp+0x550) = cup phase flags (&4 running-totals vs this-race points, &8 advance phase),
 *   D_8008D52C(gp+0x5C0) = sdata_framesSinceRaceEnded (anim timer, ramps 0..0x10f),
 *   D_8008D570(gp+0x604) = sdata_numIconsEOR (rows drawn, grows over time).
 *
 * DRAW: MP gates on the race-flag transition (RaceFlag_IsFullyOff/OnScreen + BeginTransition +
 * SetCanDraw). Slides in a title (level name data_metaDataLEV[levelID].name_LNG, or cup name from
 * g_aAdvCupNameIndex adv / g_aArcadeCups arcade) + STANDINGS (lng 0xCA) + "TRACK n/4" via
 * UI_Lerp2D_Linear. One row per racer (rank digit, character icon via UI_DrawDriverIcon, points:
 * the "+N" awarded this race g_anCupPointsPerPosition[i]/numPlyr-pos for AI, else the running cup
 * total cup.points[g_anCupPositionPerPlayer[i]]). Row layout branches by cup: Purple Gem (cupID 4,
 * 5-wide) / 6-racer 2P arcade / VS / adventure. Shows "PRESS X" (lng 0xC9); Cross/Circle advances.
 *
 * PROGRESSION (timer >= 0x10f): toggles phase flag &4. First pass awards cup.points[driverID] and
 * re-ranks (UI_CupStandings_UpdateCupRanks). Next pass advances cup.trackIndex; if <4 loads the
 * next track (MainRaceTrack_RequestLoad, adv g_aAdvCupTrackIDs / arcade g_aArcadeCupTrackIDs), else
 * finalizes: assigns final driverRanks, Podium_InitModels, then — arcade (gameMode1 & 0x400000, P1
 * podium): FinalizeCupRanks + unlock bits (diff 0x50->0/0xA0->1/else->2; cup-completion bit +
 * all-4-cups gate -> battle-map reward bit + gameMode2 |0x1000/0x2000); adventure: gem + character
 * unlocks via g_abAdvProgress / gameProgress on a P1 win — and loads Gemstone Valley (0x19).
 *
 * NOTE(claude): residual is the register-COLORING wall (the tileView_UI-base vs gGT-base hoist, the
 * title-Lerp arg sign-extension scheduling) — all non-behavioral. KEY MATCHING LEVERS to reach
 * 28725 (from 53245 first-compile): every `animTimer < 0xf1`/`< 0x10f` split lays out the HIGHER
 * range as fall-through (invert the C condition); the arcade branch is the fall-through in every
 * gameMode2 & CUP_ANY_KIND dispatch (cup-name, next-track, finalize) so ARCADE is written first;
 * the three arcade unlock-bit tables are copied as small structs (retail block-copies them ->
 * lwl/lwr, not element-wise). track-length is unused here. Verified vs decomp UI_58.
 * Phase-B (saphi game/UI/UI_Rank.c or UI_CupStandings.c): see UI_PROGRESS.md.
 * (926/NTSC-U)
 */
void UI_CupStandings_InputAndDraw(void)
{
    unsigned char bVar1;
    short sVar2;
    int bVar3;
    struct GameTracker *psVar4;
    short sVar5;
    unsigned int uVar6;
    unsigned int uVar7;
    void *pvVar8;
    struct Driver *psVar9;
    short sVar10;
    short *puVar11;
    struct GameTracker *psVar12;
    char *pcVar13;
    unsigned int uVar14;
    short sVar15;
    int iVar16;
    int *piVar17;
    short sVar18;
    int iVar19;
    int iVar20;
    int iVar21;
    struct RECT panelRect;
    char acStack_78[8];
    char acStack_70[24];
    short lerpX;
    short lerpY;
    struct UnlockRow asStack_50;
    struct UnlockRow asStack_48;
    struct UnlockRowU auStack_40;
    short slideY0;
    short slideY1;
    struct PushBuffer *pTileView;

    psVar4 = D_8008D2AC;
    pTileView = &psVar4->pushBuffer_UI;
    iVar21 = (unsigned int)(unsigned char)D_8008D2AC->numPlyrCurrGame +
             (unsigned int)(unsigned char)D_8008D2AC->numBotsNextGame;
    if (((D_8008D2AC->gameMode2 & CUP_ANY_KIND) != 0) && ((unsigned char)D_8008D2AC->numPlyrCurrGame != 1) &&
        (uVar6 = func_80043F28(), uVar6 == 1)) {
        func_80043FB0(1);
    }
    uVar6 = func_80043F1C();
    if ((uVar6 != 0) && (D_8008D2AC->numPlyrCurrGame != 1)) {
        func_80044088(1);
    }
    uVar6 = func_80043F1C();
    psVar12 = D_8008D2AC;
    if ((uVar6 == 0) && (D_8008D2AC->numPlyrCurrGame != 1)) {
        return;
    }
    uVar6 = func_80043F1C();
    if ((uVar6 != 0) || (D_8008D2AC->numPlyrCurrGame == 1)) {
        if (D_8008D2AC->numPlyrCurrGame != 1) {
            uVar7 = D_8008D2AC->renderFlags;
            D_8008D2AC->confetti_numParticles_max = 0;
            D_8008D2AC->confetti_nParticleRampStep = 0;
            D_8008D2AC->renderFlags = uVar7 & 0x1000;
        }
        if ((D_8008D52C < 0xf0) || (((D_8008D4BC & 8) != 0) && (D_8008D52C < 400))) {
            D_8008D52C = D_8008D52C + 1;
        }
    }
    if ((D_8008D52C < 0x3c) && ((D_8008D950 & 0x50) != 0)) {
        D_8008D52C = 0x3c;
        D_8008D570 = (unsigned short)(unsigned char)D_8008D2AC->numPlyrCurrGame +
                     (unsigned short)(unsigned char)D_8008D2AC->numBotsNextGame;
        func_80046404();
    }
    if ((D_8008D4BC & 4) == 0) {
        slideY0 = -0x32;
        slideY1 = 0x1e;
    } else {
        slideY0 = 0x1e;
        slideY1 = -0x32;
    }
    if (0xf0 < D_8008D52C) {
        sVar10 = 0x100;
        sVar18 = 0x296;
        sVar15 = 0x1e;
        pvVar8 = (void *)(D_8008D52C - 0xf0);
        sVar5 = slideY1;
    } else {
        sVar10 = -0x96;
        sVar18 = 0x100;
        sVar5 = 0x1e;
        sVar15 = slideY0;
        pvVar8 = (void *)D_8008D52C;
    }
    func_8004ECD4(&lerpX, sVar10, sVar15, sVar18, sVar5, (int)pvVar8, 0x14);
    if ((D_8008D4BC & 4) == 0) {
        sVar15 = D_80083A80[D_8008D2AC->levelID].name_LNG;
    } else {
        if (D_8008D2AC->cup_trackIndex == 3) {
            pcVar13 = D_8008D878[0x22e];
            goto LAB_80056678;
        }
        if ((D_8008D2AC->gameMode2 & CUP_ANY_KIND) != 0) {
            puVar11 = D_80084148;
            iVar19 = D_8008D2AC->cup_cupID * 0x12;
        } else {
            puVar11 = D_80084114;
            iVar19 = D_8008D2AC->cup_cupID << 3;
        }
        sVar15 = *(short *)(iVar19 + (int)puVar11);
    }
    pcVar13 = D_8008D878[sVar15];
LAB_80056678:
    func_80022878(pcVar13, lerpX, lerpY + -0x11, FONT_BIG, 0x8000);
    func_80022878(D_8008D878[0xca], lerpX, lerpY, FONT_BIG, 0x8000);
    func_80078348(acStack_70, &D_80011708, D_8008D878[0x175], D_8008D2AC->cup_trackIndex + 1);
    func_80022878(acStack_70, lerpX, lerpY + 0x11, FONT_SMALL, 0x8000);
    if ((D_8008D52C == (D_8008D52C / 10) * 10) && (D_8008D570 < iVar21)) {
        D_8008D570 = D_8008D570 + 1;
    }
    iVar19 = 0;
    if (0 < D_8008D570) {
        iVar20 = 0;
        sVar15 = 0;
        do {
            sVar5 = (short)iVar19;
            if (D_8008D2AC->cup_cupID == 4) {
                if (iVar19 < 5) {
                    sVar18 = 0x60;
                    sVar10 = pTileView->rectX;
                    sVar5 = (short)((pTileView->rectW + -0x20) / 5) * sVar5 + 0x10;
                    goto LAB_800568d4;
                }
                sVar10 = 0;
                sVar18 = 0;
            } else {
                if ((unsigned int)(unsigned char)D_8008D2AC->numPlyrCurrGame +
                    (unsigned int)(unsigned char)D_8008D2AC->numBotsNextGame == 6) {
                    sVar18 = 0x42;
                    if (iVar19 < 3) {
                        sVar10 = pTileView->rectX;
                        sVar5 = (short)((pTileView->rectW + -0x80) >> 2) * sVar5 + 0x20;
                    } else {
                        sVar2 = pTileView->rectW;
                        sVar5 = sVar5 + -2;
LAB_800568b8:
                        sVar18 = 0x79;
                        sVar10 = pTileView->rectX;
                        sVar5 = (short)((sVar2 + -0x80) >> 2) * sVar5 + 0x60;
                    }
                } else {
                    if (D_8008D2AC->numBotsNextGame == 0) {
                        sVar18 = 0x6c;
                        sVar10 = pTileView->rectX +
                                 (short)(((int)pTileView->rectW + iVar21 * -0x5a + 0xc) / 2) +
                                 sVar15;
                        goto LAB_800568d8;
                    }
                    sVar18 = 0x42;
                    if (3 < iVar19) {
                        sVar2 = pTileView->rectW;
                        sVar5 = sVar5 + -4;
                        goto LAB_800568b8;
                    }
                    sVar10 = pTileView->rectX;
                    sVar5 = (short)((pTileView->rectW + -0x80) >> 2) * sVar5 + 0x20;
                }
LAB_800568d4:
                sVar10 = sVar10 + sVar5;
            }
LAB_800568d8:
            if (0xf0 < D_8008D52C) {
                pvVar8 = (void *)(D_8008D52C - 0xf0);
                sVar5 = sVar10;
                sVar10 = -0x96;
            } else {
                sVar5 = 0x296;
                pvVar8 = (void *)(D_8008D52C + (iVar20 + iVar19) * -2);
            }
            func_8004ECD4(&lerpX, sVar5, sVar18, sVar10, sVar18, (int)pvVar8, 0x14);
            func_80078348(acStack_78, &D_8008D510, iVar19 + 1);
            func_80022878(acStack_78, lerpX + 0x20, lerpY + -1, FONT_SMALL, 3);
            if ((D_8008D4BC & 4) == 0) {
                psVar9 = *(struct Driver **)((int)D_8008D2AC->driversInRaceOrder + iVar20);
            } else {
                psVar9 = D_8008D2AC->drivers[*(int *)((int)D_80086D44 + iVar20)];
            }
            func_8004E8D8(*(struct Icon **)
                          ((int)D_8008D2AC->ptrIcons +
                           ((int)((unsigned int)(unsigned short)
                                  D_80086D84[(short)D_80086E84[psVar9->driverID]].iconID << 0x10) >> 0xe)),
                          lerpX, (int)lerpY, &D_8008D2AC->backBuffer->primMem,
                          D_8008D2AC->pushBuffer_UI.ptrOT, 1, 0x1000, 0x808080);
            if ((D_8008D4BC & 4) == 0) {
                if (D_8008D2AC->numBotsNextGame == 0) {
                    iVar16 = (unsigned int)(unsigned char)D_8008D2AC->numPlyrCurrGame - (iVar19 + 1);
                } else {
                    iVar16 = *(int *)((int)D_80086D64 + iVar20);
                }
                pcVar13 = &D_8008D574;
            } else {
                iVar16 = D_8008D2AC->cup_points[*(int *)((int)D_80086D44 + iVar20)];
                pcVar13 = &D_8008D520;
            }
            iVar20 = iVar20 + 4;
            func_80078348(acStack_78, pcVar13, iVar16);
            sVar15 = sVar15 + 0x5a;
            func_80022878(acStack_78, lerpX + 0x2d, lerpY + 6, FONT_BIG, 1);
            iVar19 = iVar19 + 1;
        } while (iVar19 < D_8008D570);
    }
    if ((D_8008D4BC & 4) == 0) {
        func_8004ECD4(&lerpX, -10, slideY0, -10, 9, D_8008D52C, 0x14);
    } else if (0xf0 < D_8008D52C) {
        func_8004ECD4(&lerpX, -10, 9, -10, slideY1, D_8008D52C + -0xf0, 0x14);
    } else {
        lerpX = -10;
        lerpY = 9;
    }
    panelRect.w = 0x214;
    panelRect.h = 0x32;
    panelRect.x = lerpX;
    panelRect.y = lerpY;
    func_800457B0(&panelRect, 4, D_8008D2AC->backBuffer->otMem_startPlusFour);
    psVar4 = D_8008D2AC;
    if (0x10e < D_8008D52C) {
        D_8008D570 = 1;
        D_8008D52C = 0;
        uVar7 = D_8008D4BC & 0xfffffff6;
        uVar14 = D_8008D4BC & 4;
        D_8008D4BC = uVar7 | 4;
        if (uVar14 == 0) {
            if (D_8008D2AC->numBotsNextGame == 0) {
                iVar21 = 0;
                if (D_8008D2AC->numPlyrCurrGame != 0) {
                    iVar19 = 0;
                    do {
                        iVar21 = iVar21 + 1;
                        bVar1 = *(unsigned char *)(*(int *)((int)psVar4->driversInRaceOrder + iVar19) + 0x4a);
                        psVar4->cup_points[bVar1] =
                            psVar4->cup_points[bVar1] + ((unsigned int)(unsigned char)psVar4->numPlyrCurrGame - iVar21);
                        iVar19 = iVar21 * 4;
                    } while (iVar21 < (int)(unsigned int)(unsigned char)psVar4->numPlyrCurrGame);
                }
            } else {
                iVar21 = 0;
                piVar17 = D_80086D64;
                psVar12 = D_8008D2AC;
                do {
                    iVar19 = *piVar17;
                    piVar17 = piVar17 + 1;
                    bVar1 = psVar12->driversInRaceOrder[0]->driverID;
                    iVar21 = iVar21 + 1;
                    psVar4->cup_points[bVar1] = psVar4->cup_points[bVar1] + iVar19;
                    psVar12 = (struct GameTracker *)((int)psVar12 + 4);
                } while (iVar21 < 8);
            }
            func_80056220();
        } else {
            D_8008D4BC = uVar7;
            D_8008D2AC->hudFlags = D_8008D2AC->hudFlags | 1;
            D_8008D2AC->hudFlags = D_8008D2AC->hudFlags & 0xfb;
            psVar4 = D_8008D2AC;
            D_8008D4BC = D_8008D4BC & 0xfffffffb;
            iVar21 = D_8008D2AC->cup_trackIndex + 1;
            D_8008D2AC->cup_trackIndex = iVar21;
            if (iVar21 < 4) {
                if ((psVar4->gameMode2 & CUP_ANY_KIND) != 0) {
                    func_8003CFC0(*(short *)((int)D_80084148 + 2 + iVar21 * 4 + psVar4->cup_cupID * 0x12));
                } else {
                    func_8003CFC0(*(short *)((int)D_800840A4 + iVar21 * 4 + psVar4->cup_cupID * 0x10));
                }
            } else {
                iVar21 = 0;
                if ((psVar4->gameMode2 & CUP_ANY_KIND) != 0) {
                    D_8008D2AC->cup_trackIndex = 0;
                    func_8005607C();
                    psVar4 = D_8008D2AC;
                    iVar21 = 0;
                    piVar17 = D_80086D44;
                    psVar12 = D_8008D2AC;
                    do {
                        if (psVar4->drivers[*piVar17] != 0) {
                            psVar4->drivers[*piVar17]->driverRank = (short)iVar21;
                        }
                        psVar12->cup_points[0] = 0;
                        psVar12 = (struct GameTracker *)((int)psVar12 + 4);
                        iVar21 = iVar21 + 1;
                        piVar17 = piVar17 + 1;
                    } while (iVar21 < 8);
                    func_80041C84(D_8008D2AC);
                    psVar4 = D_8008D2AC;
                    if (((D_8008D2AC->drivers[0]->driverRank == 0) || (D_8008D2AC->drivers[1]->driverRank == 0)) &&
                        ((D_8008D2AC->gameMode1 & 0x400000) != 0)) {
                        bVar3 = 1;
                        if (D_8008D2AC->arcadeDifficulty == 0x50) {
                            iVar21 = 0;
                        } else {
                            iVar21 = 2;
                            if (D_8008D2AC->arcadeDifficulty == 0xa0) {
                                iVar21 = 1;
                            }
                        }
                        asStack_50 = D_8008D578;
                        asStack_48 = D_8008D580;
                        auStack_40 = D_8008D588;
                        uVar7 = (int)asStack_50.v[iVar21] + psVar4->cup_cupID;
                        iVar19 = (int)uVar7 >> 5;
                        D_8008E6E8.unlocks[iVar19] = D_8008E6E8.unlocks[iVar19] | 1 << (uVar7 & 0x1f);
                        uVar7 = (int)asStack_48.v[iVar21] + psVar4->cup_cupID;
                        if ((D_8008E6E8.unlocks[(int)uVar7 >> 5] >> (uVar7 & 0x1f) & 1) == 0) {
                            iVar19 = 0;
                            psVar4->gameMode2 = psVar4->gameMode2 | 0x1000;
                            psVar4 = D_8008D2AC;
                            uVar14 = (unsigned int)asStack_50.v[iVar21];
                            uVar7 = uVar14;
                            do {
                                iVar19 = iVar19 + 1;
                                if ((D_8008E6E8.unlocks[(int)uVar7 >> 5] >> (uVar7 & 0x1f) & 1) == 0) {
                                    bVar3 = 0;
                                    break;
                                }
                                uVar7 = uVar14 + iVar19;
                            } while (iVar19 < 4);
                            if (bVar3) {
                                iVar19 = (int)((unsigned int)auStack_40.v[iVar21] << 0x10) >> 0x15;
                                D_8008E6E8.unlocks[iVar19] =
                                    D_8008E6E8.unlocks[iVar19] | 1 << (auStack_40.v[iVar21] & 0x1f);
                                psVar4->gameMode2 = psVar4->gameMode2 | 0x2000;
                            }
                        }
                    }
                    D_8008D2AC->podiumRewardID = 0x38;
                    func_8003CFC0(0x19);
                } else {
                    piVar17 = D_80086D44;
                    psVar4->levelID = psVar4->cup_cupID + 100;
                    D_8008D2AC->cup_trackIndex = 0;
                    D_8008D104 = D_8008D104 | 0x10000000;
                    psVar12 = psVar4;
                    do {
                        if (psVar4->drivers[*piVar17] != 0) {
                            psVar4->drivers[*piVar17]->driverRank = (short)iVar21;
                        }
                        psVar12->cup_points[0] = 0;
                        psVar12 = (struct GameTracker *)((int)psVar12 + 4);
                        iVar21 = iVar21 + 1;
                        piVar17 = piVar17 + 1;
                    } while (iVar21 < 8);
                    func_80041C84(D_8008D2AC);
                    psVar4 = D_8008D2AC;
                    if (D_80086D44[0] == (unsigned int)D_8008D2AC->drivers[0]->driverID) {
                        uVar7 = D_8008D2AC->cup_cupID + 0x6a;
                        if ((*(unsigned int *)(D_8008FBA4 + ((int)uVar7 >> 5) * 4) >> (uVar7 & 0x1f) & 1) == 0) {
                            D_8008D2AC->podiumRewardID = 0x5f;
                            uVar7 = psVar4->cup_cupID + 7;
                            iVar21 = (int)uVar7 >> 5;
                            D_8008E6E8.unlocks[iVar21] = D_8008E6E8.unlocks[iVar21] | 1 << (uVar7 & 0x1f);
                            uVar7 = psVar4->cup_cupID + 0x6a;
                            *(unsigned int *)(D_8008FBA4 + ((int)uVar7 >> 5) * 4) =
                                *(unsigned int *)(D_8008FBA4 + ((int)uVar7 >> 5) * 4) | 1 << (uVar7 & 0x1f);
                        }
                        D_8008FBA4[D_8008D2AC->cup_trackIndex + 0x42] = 0;
                    } else {
                        iVar21 = D_8008D2AC->cup_trackIndex;
                        if ((char)D_8008FBA4[iVar21 + 0x42] < 10) {
                            D_8008FBA4[iVar21 + 0x42] = D_8008FBA4[iVar21 + 0x42] + 1;
                        }
                    }
                    func_8003CFC0(0x19);
                }
            }
        }
    } else {
        if (((0x3b < D_8008D52C) && ((D_8008D4BC & 8) == 0)) &&
            (func_80022878(D_8008D878[0xc9], 0x100, 0xbe, FONT_BIG, 0x8000),
             (D_8008D950 & 0x50) != 0)) {
            D_8008D52C = 0xf0;
            D_8008D4BC = D_8008D4BC | 8;
            func_80046404();
        }
    }
    return;
}
