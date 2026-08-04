/*
 * UI_RenderFrame_Wumpa3D_2P3P4P @ 0x8005465C (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg126, score 4430 — residual is a register-COLORING wall:
 * iDriverIdx/pHudSlot land in s1/s2 swapped vs retail, the loop's global-gGT + p pointer color to
 * different temps, and a few short-field loads pick lhu where retail picked lh; all non-behavioral).
 * Real types. (Previously ⏸ deferred; brought to Br/St 0 this pass.)
 *
 * Split-screen (2P/3P/4P) 3D-wumpa HUD counter renderer. Copies the wumpa DecalMP sub-render rect
 * (g_rectWumpaDecalMP_2P for 2 players, _3P4P for 3-4) to the stack, sets up its draw env via
 * PushBuffer_SetDrawEnv_DecalMP into the wumpa DecalMP slot (g_aGpuDmaQueue[2].pInlineData[0xf]) at
 * center (x + w/2 - 0x100, y + h/2 - 0x6c), and cycles its animated texture
 * (CTR_CycleTex_2p3p4pWumpaHUD) into tileView[0]'s OT. Then per player (pHudSlot walks
 * g_apHudLayout[numPlyrCurrGame-1], stride 0xA0): unless that driver finished
 * (actionsFlagSet & ACTION_RACE_FINISHED 0x2000000) or it's END_OF_RACE (gameMode1 & 0x200000),
 * emits a POLY_FT4 quad (code 0x2c) at (sQuadX,sQuadY) = HUD slot wumpaX/Y + DecalMP xOff/yOff -
 * half-viewport, UVs/tpage packed from the viewport rect, gray 0x808080 tint (or g_nWumpaShineResult
 * when the driver holds >= 10 wumpas), advancing backBuffer->primMem.curr by 0x28 and linking into
 * tileView_UI's OT via AddPrim.
 *
 * NOTE(claude) — param/global mixing (the key hazard, verified against 926 asm register use):
 *   $s3 = PARAM gGT; D_8008D2AC = sdata_gGT GLOBAL. Reads are NOT interchangeable:
 *     GLOBAL(D_8008D2AC): the 2<numPlyr viewport pick, SetDrawEnv's backBuffer, and inside the loop
 *       drivers[i]->actionsFlagSet, gameMode1, backBuffer->primMem.curr (the p read), and the shine
 *       numWumpas check (pGt = D_8008D2AC, a separate reload).
 *     PARAM(gGT): CTR_CycleTex's tileView[0].ptrOT, the 1<numPlyr guard, g_apHudLayout index, the
 *       numPlyr!=0 guard + loop bound, backBuffer->primMem.curr ADVANCE (+0x28), and AddPrim's
 *       tileView_UI.ptrOT.
 *   The two g_rectWumpaDecalMP globals copy to the stack as unaligned 8-byte struct-assigns (lwl/lwr).
 *   The DecalMP ptr is g_aGpuDmaQueue[2].pInlineData[0xf] @ gp+0x548 (D_8008D4B4); g_nWumpaShineResult
 *   @ gp+0xA28 (D_8008D994). sQuadX/sQuadY are int (retail keeps them full-width, truncating only at
 *   the sh store) so the wumpaX/xOff loads sign-extend as in retail.
 *
 * Phase B (saphi game/UI/UI_RenderFrame.c UI_RenderFrame_Wumpa3D_2P3P4P): CLEAN — no bugs. The rect
 * unpack, viewport pick (>=3), DecalMP draw-env, CTR_CycleTex (ptrOT[0x3ff] = +0xFFC, u32* pointer-sub
 * span /4 +1), the full POLY_FT4 quad (tag 0x09000000, color 0x2c808080, XY/UV/tpage), shine at >=10,
 * prim+1 advance, and AddPrim all match. saphi already carries the retail corrections (NOTE tags):
 * reads ptrPushBufferUI (not the adjacent ptrFruitDisp instance slot), and tints with the scalar
 * wumpaShineResult (0x80..0xFF), not the gradient byte. Only divergence: saphi uses the param gGT
 * where retail reads the global sdata_gGT for some fields — identical since the call passes sdata->gGT.
 */
#include "../uitypes.h"

#define ACTION_RACE_FINISHED 0x2000000
#define END_OF_RACE          0x200000

/* g_apHudLayout entry (per-player sub-layout, stride 0xA0); only the wumpa counter pos named. */
struct HudSlot { char _pad_0000[0x18]; short wumpaX; short wumpaY; char _pad_001C[0xA0 - 0x1C]; };

extern struct GameTracker *D_8008D2AC;   /* sdata_gGT (global, absolute) */
extern struct RECT D_8008D554;           /* g_rectWumpaDecalMP_2P */
extern struct RECT D_8008D55C;           /* g_rectWumpaDecalMP_3P4P */
extern unsigned char *D_8008625C[];      /* g_apHudLayout */

/* gp-relative globals (defined =0 so patch_gprel resolves via GP_VALUE=0x8008CF6C) */
struct DecalMP *D_8008D4B4 = 0;          /* gp+0x548 g_aGpuDmaQueue[2].pInlineData[0xf] */
unsigned char D_8008D994 = 0;            /* gp+0xA28 g_nWumpaShineResult */

extern void func_80042974(unsigned int *primEnd, struct PushBuffer *backBuffer, struct RECT *viewport,
                          int centerX, int centerY, int a, int b, int c, int d, int e); /* SetDrawEnv_DecalMP */
extern void func_80021B94(unsigned int *ot, unsigned int *prim, int count); /* CTR_CycleTex_2p3p4pWumpaHUD */
extern void func_80075310(unsigned long *ot, void *prim); /* AddPrim */

void UI_RenderFrame_Wumpa3D_2P3P4P(struct GameTracker *gGT)
{
    struct HudSlot *pHudSlot;
    unsigned int *prim;
    unsigned char *p;
    int sQuadX, sQuadY;
    struct RECT *viewport;
    int iDriverIdx;
    struct RECT rectWumpa2P, rectWumpa3P4P;
    char cTexV0;
    struct GameTracker *pGt;
    short sVpWidth;
    unsigned char uShineColor;

    rectWumpa2P = D_8008D554;
    rectWumpa3P4P = D_8008D55C;
    viewport = &rectWumpa2P;
    if (2 < D_8008D2AC->numPlyrCurrGame) {
        viewport = &rectWumpa3P4P;
    }
    if (D_8008D4B4 != 0) {
        func_80042974(D_8008D4B4->primEnd, D_8008D2AC->backBuffer, viewport,
                      (short)((int)viewport->x + ((int)((unsigned int)viewport->w << 16) >> 17) - 0x100),
                      (short)((int)viewport->y + ((int)((unsigned int)viewport->h << 16) >> 17) - 0x6c),
                      0, 0, 0, 0, 1);
        prim = D_8008D4B4->prim;
        if (prim != 0 && D_8008D4B4->primEnd != 0) {
            func_80021B94((unsigned int *)((int)gGT->pushBuffer[0].ptrOT + 0xffc), prim,
                          (((int)D_8008D4B4->primEnd - (int)prim) >> 2) + 1);
        }
    }
    if (1 < gGT->numPlyrCurrGame) {
        iDriverIdx = 0;
        pHudSlot = (struct HudSlot *)D_8008625C[gGT->numPlyrCurrGame - 1];
        if (gGT->numPlyrCurrGame != 0) {
            do {
                if ((D_8008D2AC->drivers[iDriverIdx]->actionsFlagSet & ACTION_RACE_FINISHED) == 0 &&
                    (D_8008D2AC->gameMode1 & END_OF_RACE) == 0) {
                    sQuadX = (pHudSlot->wumpaX + D_8008D4B4->xOff) -
                             (short)((int)((unsigned int)viewport->w << 16) >> 17);
                    sQuadY = (pHudSlot->wumpaY + D_8008D4B4->yOff) -
                             (short)((int)((unsigned int)viewport->h << 16) >> 17);
                    p = (unsigned char *)D_8008D2AC->backBuffer->primMem.curr;
                    p[3] = 9;
                    p[7] = 0x2c;
                    p[6] = 0x80;
                    p[5] = 0x80;
                    p[4] = 0x80;
                    *(short *)(p + 8) = sQuadX;
                    *(short *)(p + 0xA) = sQuadY;
                    sVpWidth = viewport->w;
                    *(short *)(p + 0x12) = sQuadY;
                    *(short *)(p + 0x18) = sQuadX;
                    *(short *)(p + 0x10) = sVpWidth + sQuadX;
                    *(short *)(p + 0x1A) = viewport->h + sQuadY;
                    *(short *)(p + 0x20) = viewport->w + sQuadX;
                    *(short *)(p + 0x22) = viewport->h + sQuadY;
                    p[0xC] = viewport->x & 0x3f;
                    p[0xD] = (char)viewport->y;
                    cTexV0 = p[0xD];
                    p[0x14] = p[0xC] + (char)viewport->w;
                    p[0x15] = cTexV0;
                    p[0x1C] = p[0xC];
                    p[0x1D] = cTexV0 + (char)viewport->h;
                    p[0x24] = p[0xC] + (char)viewport->w;
                    p[0x25] = cTexV0 + (char)viewport->h;
                    pGt = D_8008D2AC;
                    *(unsigned short *)(p + 0x16) =
                        (short)(viewport->y & 0x100) >> 4 |
                        (unsigned short)(((unsigned short)viewport->x & 0x3ff) >> 6) | 0x100 |
                        (viewport->y & 0x200) << 2;
                    uShineColor = D_8008D994;
                    if (9 < pGt->drivers[iDriverIdx]->numWumpas) {
                        p[6] = D_8008D994;
                        p[5] = uShineColor;
                        p[4] = uShineColor;
                    }
                    gGT->backBuffer->primMem.curr =
                        (unsigned char *)gGT->backBuffer->primMem.curr + 0x28;
                    func_80075310(gGT->pushBuffer_UI.ptrOT, p);
                }
                iDriverIdx = iDriverIdx + 1;
                pHudSlot = pHudSlot + 1;
            } while (iDriverIdx < (int)(unsigned char)gGT->numPlyrCurrGame);
        }
    }
}
