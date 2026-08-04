/*
 * UI_INSTANCE_InitAll @ 0x8004CEC4 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg210, score 17990 — the biggest UI fn; residual is a
 * register-COLORING wall across the 468-insn body: the 32-byte tileView matrix block-move picks
 * different temp registers (retail s2/s3/t8/t9 vs t4/t5/t6/t7), the reloaded gGT source colors to a
 * different reg, and that cascades. All non-behavioral — structure, the 5 relic-time division idioms
 * [magics 0x91A2B3C5/0x1B4E81B5/0x88888889/0x2AAAAAAB/0x66666667 all match], block order, and every
 * global store verified exact). Real types. (Previously ⏸ deferred monster; brought to Br/St 0.)
 *
 * Spawns all 3D HUD-object instances at race/event start, dispatched by game mode, each via
 * UI_INSTANCE_BirthWithThread(modelIndex, tickFunc, hudElementIndex, faceCamera, pushBuffer, name).
 * Clears g_nMenuReadyToPass bit0 (@gp+0x550) and sets gGT->renderFlags 0x8000 (@0x256C), then:
 *   - Crystal Challenge (gameMode1 & 0x8000000): g_pMenuCrystalInst + g_pHudCrystalInst (hidden).
 *   - Adventure Arena (& 0x100000): relic/key/trophy counters (hud 0xE/0xF/0x10) + GAMEPROG_AdvPercent.
 *   - Relic/TimeTrial (& 0x4120000): seed g_awRankIconsCurr[i]=drivers[i]->driverRank (+transition
 *     timer 5 in MP) over 8 via a gtCursor stepping +4; if relic race (& 0x4000000) spawn g_pHudRelicInst
 *     (scaled 0) + g_pHudTimeboxInst and fill the relic-time digits g_nRelicTime_* from
 *     g_aRelicTime[levelID*3 + relicType] (relicType = next unearned medal from g_abAdvProgress reward
 *     bits at levelID+0x16/0x28/0x3A).
 *   - Normal race: for 2+ players copy gGT->pushBuffer_UI into g_tileViewDecalMP and point ptrPushBufferUI
 *     at it; spawn g_pHudFruitDispInst; the big rank number (1-2P non-battle); in adventure mode
 *     (& 0x80000) the 3 CTR letters (all hidden, flags|=0x80).
 *   - Always: g_pHudTokenInst (CTR token, scaled 0, hidden).
 *
 * NOTE(claude) — key transcription facts (verified vs 926 asm): the top dispatch is inverted so the
 * CRYSTAL branch is the fall-through and the (returning) RELIC block is emitted AFTER normal-race, to
 * match the retail block order (CRYSTAL→ADV_ARENA→NORMAL_RACE→RELIC). The tileView matrix (0x28, 32B)
 * copies as a MATRIX struct-assign (grouped 4-lw/4-sw block move); pos/rect/ptrOT/distanceToScreen copy
 * field-by-field. g_tileViewDecalMP (D_8009AD18) and gGT->pushBuffer_UI share the PushBuffer layout
 * (pos@0, distanceToScreen@0x18, rect@0x1C, matrix@0x28, ptrOT@0xF4). The DMA pInlineData[0xf/0x10/0x11]
 * = gp+0x548/0x54C/0x550 (ptrPushBufferUI/ptrFruitDisp/menuReadyToPass); instance-ptr + relic-time
 * globals are gp-rel @0xA44..0xA7C. Phase B (saphi UI_Instance.c): CLEAN per prior audit (matches
 * retail; the CTR_NATIVE null-guards are native-safety, and the decomp's #if 0'd rank loop + relic-ms
 * are present in retail+saphi).
 */
#include "../uitypes.h"

#define CRYSTAL_CHALLENGE 0x8000000
#define ADVENTURE_ARENA   0x100000
#define RELIC_ADV_TT      0x4120000
#define RELIC_RACE        0x4000000
#define BATTLE_MODE       0x20
#define ADVENTURE_MODE    0x80000
#define HIDE_MODEL        0x80

extern struct GameTracker *D_8008D2AC;   /* sdata_gGT (absolute) */

/* gp-relative globals (defined =0 so patch_gprel resolves via GP_VALUE=0x8008CF6C) */
struct Instance *D_8008D4B4 = 0;   /* gp+0x548 g_aGpuDmaQueue[2].pInlineData[0xf] ptrPushBufferUI */
struct Instance *D_8008D4B8 = 0;   /* gp+0x54C pInlineData[0x10] ptrFruitDisp */
unsigned int     D_8008D4BC = 0;   /* gp+0x550 pInlineData[0x11] g_nMenuReadyToPass */
int              D_8008D9B0 = 0;   /* gp+0xA44 g_nRelicTime_1min */
struct Instance *D_8008D9B4 = 0;   /* gp+0xA48 g_pHudRelicInst */
int              D_8008D9B8 = 0;   /* gp+0xA4C g_nRelicTime_1sec */
struct Instance *D_8008D9BC = 0;   /* gp+0xA50 g_pHudCrystalInst */
struct Instance *D_8008D9C0 = 0;   /* gp+0xA54 g_pMenuCrystalInst */
struct Instance *D_8008D9C4 = 0;   /* gp+0xA58 g_apHudLetterInst */
struct Instance *D_8008D9C8 = 0;   /* gp+0xA5C g_pHudLetterInstR */
struct Instance *D_8008D9CC = 0;   /* gp+0xA60 g_pHudLetterInstC */
struct Instance *D_8008D9D0 = 0;   /* gp+0xA64 g_pHudTokenInst */
int              D_8008D9D4 = 0;   /* gp+0xA68 g_nRelicTime_10sec */
struct Instance *D_8008D9D8 = 0;   /* gp+0xA6C g_pHudTimeboxInst */
int              D_8008D9E0 = 0;   /* gp+0xA74 g_nRelicTime_1ms */
int              D_8008D9E8 = 0;   /* gp+0xA7C g_nRelicTime_10ms */

/* absolute globals */
extern short D_800862C8[];   /* g_awRankIconsCurr */
extern short D_800862D8[];   /* g_awRankIconsTransitionTimer */
extern int   D_80086340[];   /* g_aRelicTime (stride 0xc/level = 3 ints) */
extern unsigned int D_8008FBA4[];  /* g_abAdvProgress (advProgress reward bits) */
extern struct PushBuffer D_8009AD18;  /* g_tileViewDecalMP */
extern char D_800116B0[];    /* "crystal1" */
extern char D_800116BC[];    /* "fruitdisp" */
extern char D_800116C8[];    /* "timebox1" */
extern char D_8008D4C0[];    /* s_token1 */
extern char D_8008D4C8[];    /* s_relic1 */
extern char D_8008D4D0[];    /* s_key1 */
extern char D_8008D4D8[];    /* s_trophy1 */
extern char D_8008D4E0[];    /* s_big1 */
extern char D_8008D4E8[];    /* s_letterC */
extern char D_8008D4F0[];    /* s_letterT */
extern char D_8008D4F8[];    /* s_letterR */

extern struct Instance *func_8004CAE8(int modelIndex, void *tick, int hudElem, int faceCam,
                                      struct Instance *pushBuffer, char *name); /* BirthWithThread */
extern void func_800265C0(void *advProgress);   /* GAMEPROG_AdvPercent */
extern void func_8004C850(void);   /* UI_ThTick_Reward */
extern void func_8004C718(void);   /* UI_ThTick_CountPickup */
extern void func_8004C914(void);   /* UI_ThTick_CtrLetters */
extern void func_8004CA04(void);   /* UI_ThTick_big1 */

void UI_INSTANCE_InitAll(void)
{
    struct Instance *pTokenInst;
    unsigned int gameMode;
    unsigned int tokenFlags;
    struct GameTracker *gtCursor;
    struct Instance *pHideInst;
    short *pRankCur;
    int levelId;
    short *pRankTimer;
    unsigned int uRelicType;
    int iIdx;
    struct Instance *pushBuffer;
    struct GameTracker *pGt;
    struct Instance *pInst;

    pGt = D_8008D2AC;
    D_8008D4BC = D_8008D4BC & 0xfffffffe;
    D_8008D2AC->renderFlags = D_8008D2AC->renderFlags | 0x8000;
    gameMode = pGt->gameMode1;
    if ((gameMode & CRYSTAL_CHALLENGE) != 0) {
        D_8008D9C0 = func_8004CAE8(0x60, func_8004C850, 0x11, 0, 0, D_800116B0);
        D_8008D9BC = func_8004CAE8(0x60, func_8004C850, 0x11, 0, 0, D_800116B0);
        pTokenInst = func_8004CAE8(0x7d, func_8004C850, 0x12, 0, 0, D_8008D4C0);
        pHideInst = D_8008D9BC;
    } else {
        if ((gameMode & ADVENTURE_ARENA) != 0) {
            func_8004CAE8(0x61, func_8004C850, 0xe, 1, 0, D_8008D4C8);
            func_8004CAE8(99, func_8004C850, 0xf, 1, 0, D_8008D4D0);
            func_8004CAE8(0x62, func_8004C850, 0x10, 0, 0, D_8008D4D8);
            func_800265C0(D_8008FBA4);
            return;
        }
        iIdx = 0;
        if ((gameMode & RELIC_ADV_TT) == 0) {
            pushBuffer = 0;
            if (1 < (unsigned char)pGt->numPlyrCurrGame) {
                pushBuffer = (struct Instance *)&D_8009AD18;
            }
            D_8009AD18.matrix_ViewProj = D_8008D2AC->pushBuffer_UI.matrix_ViewProj;
            D_8009AD18.pos[0] = D_8008D2AC->pushBuffer_UI.pos[0];
            D_8009AD18.pos[1] = D_8008D2AC->pushBuffer_UI.pos[1];
            D_8009AD18.pos[2] = D_8008D2AC->pushBuffer_UI.pos[2];
            D_8009AD18.rectX = D_8008D2AC->pushBuffer_UI.rectX;
            D_8009AD18.rectY = D_8008D2AC->pushBuffer_UI.rectY;
            D_8009AD18.rectW = D_8008D2AC->pushBuffer_UI.rectW;
            D_8009AD18.rectH = D_8008D2AC->pushBuffer_UI.rectH;
            D_8009AD18.ptrOT = D_8008D2AC->pushBuffer_UI.ptrOT;
            D_8009AD18.distanceToScreen_PREV = D_8008D2AC->pushBuffer_UI.distanceToScreen_PREV;
            D_8008D4B4 = pushBuffer;
            D_8008D4B8 = func_8004CAE8(0x37, func_8004C718, 3, 1, pushBuffer, D_800116BC);
            if ((unsigned char)D_8008D2AC->numPlyrCurrGame < 3 && (D_8008D2AC->gameMode1 & BATTLE_MODE) == 0) {
                func_8004CAE8(0x38, func_8004CA04, 2, 0, 0, D_8008D4E0);
            }
            if ((D_8008D2AC->gameMode1 & ADVENTURE_MODE) == 0) {
                return;
            }
            D_8008D9CC = func_8004CAE8(0x93, func_8004C914, 0x12, 0, 0, D_8008D4E8);
            D_8008D9C4 = func_8004CAE8(0x94, func_8004C914, 0x12, 0, 0, D_8008D4F0);
            D_8008D9C8 = func_8004CAE8(0x95, func_8004C914, 0x12, 0, 0, D_8008D4F8);
            pTokenInst = func_8004CAE8(0x7d, func_8004C850, 0x12, 0, 0, D_8008D4C0);
            pInst = D_8008D9C4;
            D_8008D9CC->flags = D_8008D9CC->flags | HIDE_MODEL;
            pHideInst = D_8008D9C8;
            pInst->flags = pInst->flags | HIDE_MODEL;
        } else {
            pRankTimer = D_800862D8;
            pRankCur = D_800862C8;
            gtCursor = pGt;
            do {
                *pRankCur = gtCursor->drivers[0]->driverRank;
                if (1 < (unsigned char)pGt->numPlyrCurrGame) {
                    *pRankTimer = 5;
                }
                pRankTimer = pRankTimer + 1;
                pRankCur = pRankCur + 1;
                iIdx = iIdx + 1;
                gtCursor = (struct GameTracker *)((char *)gtCursor + 4);
            } while (iIdx < 8);
            if ((D_8008D2AC->gameMode1 & RELIC_RACE) == 0) {
                return;
            }
            D_8008D9B4 = func_8004CAE8(0x61, func_8004C850, 0xe, 1, 0, D_8008D4C8);
            D_8008D9D8 = func_8004CAE8(0x5c, func_8004C718, 0x13, 1, 0, D_800116C8);
            pInst = D_8008D9B4;
            if (D_8008D9B4 != 0) {
                D_8008D9B4->scale[2] = 0;
                pInst->scale[1] = 0;
                pInst->scale[0] = 0;
            }
            levelId = D_8008D2AC->levelID;
            if (((D_8008FBA4[(levelId + 0x3a) >> 5] >> (levelId + 0x3a & 0x1f) & 1) == 0) &&
                ((D_8008FBA4[(levelId + 0x28) >> 5] >> (levelId + 0x28 & 0x1f) & 1) == 0)) {
                uRelicType = D_8008FBA4[(levelId + 0x16) >> 5] >> (levelId + 0x16 & 0x1f) & 1;
            } else {
                uRelicType = 2;
            }
            levelId = D_8008D2AC->levelID;
            iIdx = uRelicType * 4;
            D_8008D9B0 = *(int *)((char *)D_80086340 + iIdx + levelId * 0xc) / 0xe100;
            D_8008D9E0 = ((*(int *)((char *)D_80086340 + iIdx + D_8008D2AC->levelID * 0xc) * 100) / 0x3c0) % 10;
            D_8008D9E8 = (*(int *)((char *)D_80086340 + iIdx + D_8008D2AC->levelID * 0xc) / 0x60) % 10;
            D_8008D9D4 = (*(int *)((char *)D_80086340 + iIdx + levelId * 0xc) / 0x2580) % 6;
            D_8008D9B8 = (*(int *)((char *)D_80086340 + iIdx + levelId * 0xc) / 0x3c0) % 10;
            return;
        }
    }
    pHideInst->flags = pHideInst->flags | HIDE_MODEL;
    tokenFlags = pTokenInst->flags;
    D_8008D9D0 = pTokenInst;
    pTokenInst->scale[0] = 0;
    pTokenInst->scale[1] = 0;
    pTokenInst->scale[2] = 0;
    pTokenInst->flags = tokenFlags | HIDE_MODEL;
}
