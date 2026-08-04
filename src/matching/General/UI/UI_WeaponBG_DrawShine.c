/*
 * UI_WeaponBG_DrawShine @ 0x8004E37C (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg residual = GPU register-pressure/spill wall; base176~tgt185).
 * Real types (uitypes.h POLY_GT4). Draws the weapon-roulette shine as a 2x2 grid of 4 mirrored gouraud
 * POLY_GT4 quads of `icon`, expanding from (posX,posY) by texLayout-span*scale>>12. Vertex colors from
 * the shine gradient (color2 if layer==3 else color1): r0=color[2], r1=r2=color[1], r3=color[0].
 * setPolyGT4 (code 0x3C); layer sets tpage semi-transparency (layer-1)*0x20 + code bit 2. AddPrim each.
 * Phase B (saphi UI_Icon.c:41): CLEAN — identical geometry/colors/tpage; switch+wumpaShine-ptr vs
 * if-else+color-locals is codegen-neutral.
 */
#include "../uitypes.h"

extern void func_80075310(unsigned long *ot, void *p);   /* AddPrim */

/* wumpa shine gradient colors (gp-rel; produced by UI_WeaponBG_AnimateShine) */
int D_8008D998 = 0;   /* g_anWumpaShineColor1[0] (gp+0xA2C) */
int D_8008D99C = 0;   /* g_anWumpaShineColor1[1] (gp+0xA30) */
int D_8008D9A0 = 0;   /* g_anWumpaShineColor1[2] (gp+0xA34) */
int D_8008D9A4 = 0;   /* g_anWumpaShineColor2[0] (gp+0xA38) */
int D_8008D9A8 = 0;   /* g_anWumpaShineColor2[1] (gp+0xA3C) */
int D_8008D9AC = 0;   /* g_anWumpaShineColor2[2] (gp+0xA40) */

void UI_WeaponBG_DrawShine(struct Icon *icon, short posX, short posY, struct PrimMem *primMem, unsigned long *ot,
                   char layer, short scaleX, short scaleY, int unused)
{
    POLY_GT4 *p;
    int iQuadW;
    short sQuadW, sQuadH;
    short sRightX, sBottomY;
    short sHalfX, sHalfY;
    short sInnerRightX, sInnerBottomY;
    short sMirrorX, sMirrorY;
    int iTile;
    unsigned int color0, color1, color2;   /* r3=color[0], r1=r2=color[1], r0=color[2] */

    color0 = D_8008D9A4;
    color1 = D_8008D9A8;
    color2 = D_8008D9AC;
    if (layer != 3) {
        color0 = D_8008D998;
        color1 = D_8008D99C;
        color2 = D_8008D9A0;
    }
    iTile = 0;
    iQuadW = ((icon->texLayout.u1 - icon->texLayout.u0) * (int)scaleX) >> 0xc;
    sQuadW = (short)iQuadW;
    sRightX = posX + sQuadW;
    sHalfX = scaleX >> 0xc;
    sInnerRightX = sRightX - sHalfX;
    sQuadH = (short)(((icon->texLayout.v2 - icon->texLayout.v0) * (int)scaleY) >> 0xc);
    sBottomY = posY + sQuadH;
    sHalfY = scaleY >> 0xc;
    sInnerBottomY = sBottomY - sHalfY;
    do {
        p = (POLY_GT4 *)primMem->curr;
        *(int *)&p->u0 = *(int *)&icon->texLayout.u0;
        *(int *)&p->u1 = *(int *)&icon->texLayout.u1;
        *(int *)&p->u2 = *(int *)&icon->texLayout.u2;
        *(short *)&p->u3 = *(short *)&icon->texLayout.u3;
        if (iTile == 0) {
            p->x0 = posX;    p->y0 = posY;
            p->x1 = sRightX; p->y1 = posY;
            p->x2 = posX;    p->y2 = sBottomY;
            p->x3 = sRightX; p->y3 = sBottomY;
        } else if (iTile == 1) {
            sMirrorX = (posX + (short)(iQuadW << 1)) - sHalfX;
            p->x0 = sMirrorX;     p->y0 = posY;
            p->x1 = sInnerRightX; p->y1 = posY;
            p->x2 = sMirrorX;     p->y2 = sBottomY;
            p->x3 = sInnerRightX; p->y3 = sBottomY;
        } else {
            if (iTile == 2) {
                sMirrorX = (posY + sQuadH * 2) - sHalfY;
                p->x0 = posX;    p->y0 = sMirrorX;
                p->x1 = sRightX; p->y1 = sMirrorX;
                p->x2 = posX;    p->y2 = sInnerBottomY;
                p->x3 = sRightX;
            } else {
                sMirrorX = (posX + sQuadW * 2) - sHalfX;
                sMirrorY = (posY + sQuadH * 2) - sHalfY;
                p->x0 = sMirrorX;     p->y0 = sMirrorY;
                p->x1 = sInnerRightX; p->y1 = sMirrorY;
                p->x2 = sMirrorX;     p->y2 = sInnerBottomY;
                p->x3 = sInnerRightX;
            }
            p->y3 = sInnerBottomY;
        }
        *(unsigned int *)&p->r0 = color2;
        *(unsigned int *)&p->r1 = color1;
        *(unsigned int *)&p->r2 = color1;
        *(unsigned int *)&p->r3 = color0;
        ((char *)&p->tag)[3] = 0xc;
        p->code = 0x3c;
        if (layer != 0) {
            p->tpage = (p->tpage & 0xff9f) | (unsigned short)((layer - 1) * 0x20);
            p->code |= 2;
        }
        func_80075310(ot, p);
        iTile = iTile + 1;
        primMem->curr = (char *)primMem->curr + 0x34;
    } while (iTile < 4);
}
