/*
 * UI_RaceStart_IntroText1P @ 0x80055840 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg90, score 10350 — deep instruction-scheduling +
 * register-allocation cascade; the retail scheduler batches all four viewport-rect lhu loads
 * upfront per draw and groups the (sVar7 - const) offsets differently, and copies the bar color
 * via lwl/lwr where natural C emits lw/sw. All non-behavioral: control flow and stack layout are
 * exact.) Real types.
 *
 * Single-player race-start title overlay (drawn each frame during the fly-in). Picks a title
 * language-string index from the current game mode via a mode-priority chain:
 *   RELIC_RACE(0x4000000)      -> 0xB8
 *   CRYSTAL_CHALLENGE(0x8000000)-> 0xBE
 *   ADVENTURE_CUP(0x10000000)  -> g_aAdvCupStringIndex[cup.cupID]
 *   gameMode2 & CUP_ANY_KIND(0x10) -> g_aArcadeVsCupStringIndex[cup.cupID]
 *   ARCADE_MODE(0x400000)      -> 0x4E
 *   TIME_TRIAL(0x20000)        -> 0x4D
 *   boss (gameMode1 < 0)       -> g_aLngChallenge[bossID]
 *   trophy (else)              -> 0xB7, or 0x176 if gameMode2 & TOKEN_RACE(0x8)
 * Computes the fly-in slide offset sVar7 = 0x1E - cameraDC[0].nTransitionFrame (clamped: 0 once the
 * transition frame reaches 0x1F). If RaceFlag_IsFullyOnScreen() reports the flag is NOT yet fully
 * on screen, draws (all with DecalFont color -0x8000 = the top/dark shade):
 *   - the mode title (lngStrings[index], FONT_BIG=1), centered X = rect.x + rect.w/2, Y slides in;
 *   - for cup modes only: a "%s %ld/4" line (lngStrings[0x175] = "TRACK", cup.trackIndex+1) at
 *     X=0x100 in FONT_SMALL=2, and the title shifted up by 6;
 *   - the level name (lngStrings[data_metaDataLEV[levelID].name_LNG], FONT_BIG) below the viewport;
 *   - four solid title bars (CTR_Box_DrawSolidBox) framing the viewport top & bottom: two thin
 *     (h=2) accent bars in g_dwBattleSetupColorUI1, then two tall (h=0x1E) bars in that color masked
 *     to its alpha byte only (color & 0xFF000000) — the letterbox fade. All bars span the full
 *     viewport width and are anchored to the fly-in offset.
 * The viewport rect is pushBuffer[0] (tileView[0]) — x@0x184, y@0x186, w@0x188, h@0x18A; the bars
 * draw into gGT->backBuffer's otMem/primMem.
 *
 * NOTE(claude) vs in-repo decomp (REFERENCE only; 926 Ghidra+asm authoritative): the mode chain is
 * transcribed as explicit gotos to match the retail branch directions/block order (lookup ->
 * trophy -> have_index). gGT is reloaded (D_8008D2AC) after RaceFlag_IsFullyOnScreen rather than
 * cached, matching the retail don't-cache-gGT codegen; only the rect base (tv = &pushBuffer[0]) is
 * kept live. The pushBuffer struct uses element-base 0x168 (rect@0x1C, matrix@0x28, ptrOT@0xF4) so
 * gGT+0x168 is the array base as in the asm; all absolute field addresses are preserved and every
 * other pushBuffer user re-verified Br/St 0. DecalFont color arg is the literal -0x8000; the "TRACK
 * n/4" sprintf takes 4 args (buf, fmt, "TRACK", trackIndex+1).
 *
 * Phase B (saphi game/UI/UI_RaceFlow.c UI_RaceStart_IntroText1P): CLEAN — no bugs. Mode chain,
 * textIDs, transition slide, all six DecalFont draws, and the four letterbox bars are faithful.
 * Divergences are intentional saphi refactors, not defects: CTR_Box_DrawSolidBox is a 3-arg native
 * re-impl (RECT*, Color by-value, ot*) that resolves primMem internally via GetPrimMem() rather than
 * taking the 926's explicit 4th &backBuffer->primMem arg; and the DecalFont style word
 * (JUSTIFY_CENTER 0x8000 | ORANGE 0) equals the retail's sign-extended -0x8000 (low 16 bits
 * identical, ORANGE==0 static-asserted).
 */
#include "../uitypes.h"

#define RELIC_RACE        0x4000000
#define CRYSTAL_CHALLENGE 0x8000000
#define ADVENTURE_CUP     0x10000000
#define ARCADE_MODE       0x400000
#define TIME_TRIAL        0x20000
#define CUP_ANY_KIND      0x10
#define TOKEN_RACE        0x8

extern struct GameTracker *D_8008D2AC;      /* gGT (absolute) */
extern char **D_8008D878;                   /* sdata->lngStrings */
extern short D_80086D2C[];                  /* g_aAdvCupStringIndex */
extern short D_80086D38[];                  /* g_aArcadeVsCupStringIndex */
extern short D_8008413C[];                  /* g_aLngChallenge */
extern struct MetaDataLEV D_80083A80[];     /* data_metaDataLEV */
extern unsigned int D_8008D438;             /* g_dwBattleSetupColorUI1 */
extern char D_80011708[];                   /* "%s %ld/4" */
extern unsigned int func_80043F1C(void);                              /* RaceFlag_IsFullyOnScreen */
extern void func_80022878(char *s, short x, short y, int font, int color); /* DecalFont_DrawLine */
extern void func_80021894(struct RECT *r, unsigned int *color, unsigned int *ot,
                          struct PrimMem *pm);                        /* CTR_Box_DrawSolidBox */
extern int func_80078348(char *buf, char *fmt, char *sarg, int iarg);/* sprintf */

void UI_RaceStart_IntroText1P(void)
{
    struct GameTracker *gGT;
    unsigned int gameMode1;
    short uVar6;
    short sVar7;
    char *str;
    short posX, posY;
    int fontType;
    char acStack_48[24];
    struct RECT barRect;
    unsigned int barColor[2];
    struct PushBuffer *tv;

    short *puVar5;
    int iVar2;

    gGT = D_8008D2AC;
    tv = &gGT->pushBuffer[0];
    sVar7 = 0;
    gameMode1 = gGT->gameMode1;
    if ((gameMode1 & RELIC_RACE) == 0) goto notRelic;
    uVar6 = 0xb8;
    goto have_index;
notRelic:
    uVar6 = 0xbe;
    if ((gameMode1 & CRYSTAL_CHALLENGE) != 0) goto have_index;
    if ((gameMode1 & ADVENTURE_CUP) == 0) goto notAdvCup;
    iVar2 = gGT->cup_cupID;
    puVar5 = D_80086D2C;
    goto lookup;
notAdvCup:
    if ((gGT->gameMode2 & CUP_ANY_KIND) == 0) goto notCup;
    iVar2 = gGT->cup_cupID;
    puVar5 = D_80086D38;
    goto lookup;
notCup:
    uVar6 = 0x4e;
    if ((gameMode1 & ARCADE_MODE) != 0) goto have_index;
    uVar6 = 0x4d;
    if ((gameMode1 & TIME_TRIAL) != 0) goto have_index;
    if ((int)gameMode1 >= 0) goto trophy;
    iVar2 = gGT->bossID;
    puVar5 = D_8008413C;
lookup:
    uVar6 = puVar5[iVar2];
    goto have_index;
trophy:
    uVar6 = 0xb7;
    if ((gGT->gameMode2 & TOKEN_RACE) != 0) uVar6 = 0x176;
have_index:
    if (D_8008D2AC->cameraDC0_nTransitionFrame < 0x1f) {
        sVar7 = 0x1e - D_8008D2AC->cameraDC0_nTransitionFrame;
    }
    if (func_80043F1C() == 0) {
        fontType = 1;
        if ((D_8008D2AC->gameMode1 & ADVENTURE_CUP) == 0 && (D_8008D2AC->gameMode2 & CUP_ANY_KIND) == 0) {
            posX = (short)((unsigned short)tv->rectX +
                           ((int)((unsigned int)tv->rectW << 0x10) >> 0x11));
            str = D_8008D878[uVar6];
            posY = tv->rectY - (sVar7 + -7);
        } else {
            func_80022878(D_8008D878[uVar6],
                          (short)((unsigned short)tv->rectX +
                                  ((int)((unsigned int)tv->rectW << 0x10) >> 0x11)),
                          (tv->rectY - (sVar7 + -7)) + -6, 1, -0x8000);
            func_80078348(acStack_48, D_80011708, D_8008D878[0x175], D_8008D2AC->cup_trackIndex + 1);
            str = acStack_48;
            posX = 0x100;
            fontType = 2;
            posY = (tv->rectY - (sVar7 + -7)) + 0xb;
        }
        func_80022878(str, posX, posY, fontType, -0x8000);
        func_80022878(D_8008D878[(short)D_80083A80[D_8008D2AC->levelID].name_LNG],
                      (short)((unsigned short)tv->rectX +
                              ((int)((unsigned int)tv->rectW << 0x10) >> 0x11)),
                      tv->rectY + tv->rectH + sVar7 + -0x17, 1, -0x8000);

        barColor[0] = D_8008D438;
        barRect.x = tv->rectX;
        barRect.y = tv->rectY - (sVar7 + -0x1c);
        barRect.w = tv->rectW;
        barRect.h = 2;
        func_80021894(&barRect, barColor, D_8008D2AC->backBuffer->otMem_startPlusFour, &D_8008D2AC->backBuffer->primMem);
        barRect.x = tv->rectX;
        barRect.y = tv->rectY + tv->rectH + sVar7 + -0x1e;
        barRect.w = tv->rectW;
        barRect.h = 2;
        func_80021894(&barRect, barColor, D_8008D2AC->backBuffer->otMem_startPlusFour, &D_8008D2AC->backBuffer->primMem);
        barColor[0] = barColor[0] & 0xff000000;
        barRect.x = tv->rectX;
        barRect.y = tv->rectY - sVar7;
        barRect.w = tv->rectW;
        barRect.h = 0x1e;
        func_80021894(&barRect, barColor, D_8008D2AC->backBuffer->otMem_startPlusFour, &D_8008D2AC->backBuffer->primMem);
        barRect.x = tv->rectX;
        barRect.y = tv->rectY + tv->rectH + sVar7 + -0x1e;
        barRect.w = tv->rectW;
        barRect.h = 0x1e;
        func_80021894(&barRect, barColor, D_8008D2AC->backBuffer->otMem_startPlusFour, &D_8008D2AC->backBuffer->primMem);
    }
}
