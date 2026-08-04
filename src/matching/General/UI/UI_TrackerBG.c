/*
 * UI_TrackerBG @ 0x8004E660 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg residual = GPU register-pressure/spill wall; base159~tgt158).
 * Real types (uitypes.h POLY_FT4). Draws the lock-on tracker background as a 2x2 grid of 4 mirrored
 * flat-textured POLY_FT4 quads of targetIcon, expanding from (centerX,centerY) by texLayout-span*angle>>12.
 * Single flat `color`. Also advances g_nWumpaShineTheta by 0x100. setPolyFT4 (code 0x2C); transparency>0
 * sets tpage semi-trans (transparency-1)*0x20 + code bit 2. AddPrim each; curr += 0x28.
 * Phase B (saphi UI_Icon.c:172): CLEAN — identical geometry/theta/colors; compressed vertex form
 * (x2=x0,y1=y0,x3=x1,y3=y2) is value-equivalent to retail's explicit per-tile writes.
 */
#include "../uitypes.h"

extern void func_80075310(unsigned long *ot, void *p);   /* AddPrim */
short D_8008D990 = 0;   /* g_nWumpaShineTheta (gp+0xA24) */

void UI_TrackerBG(struct Icon *targetIcon, short centerX, short centerY, struct PrimMem *primMem,
                   unsigned long *ot, char transparency, short angleX, short angleY, int color)
{
    POLY_FT4 *p;
    int iQuadW;
    short sQuadW, sQuadH;
    short sRightX, sBottomY;
    short sHalfX, sHalfY;
    short sInnerRightX, sInnerBottomY;
    short sMirror, sMirrorX;
    int iTile;

    iTile = 0;
    iQuadW = ((targetIcon->texLayout.u1 - targetIcon->texLayout.u0) * (int)angleX) >> 0xc;
    sQuadW = (short)iQuadW;
    sRightX = centerX + sQuadW;
    sHalfX = angleX >> 0xc;
    sInnerRightX = sRightX - sHalfX;
    D_8008D990 = D_8008D990 + 0x100;
    sQuadH = (short)(((targetIcon->texLayout.v2 - targetIcon->texLayout.v0) * (int)angleY) >> 0xc);
    sBottomY = centerY + sQuadH;
    sHalfY = angleY >> 0xc;
    sInnerBottomY = sBottomY - sHalfY;
    do {
        p = (POLY_FT4 *)primMem->curr;
        *(int *)&p->r0 = color;
        *(int *)&p->u0 = *(int *)&targetIcon->texLayout.u0;
        *(int *)&p->u1 = *(int *)&targetIcon->texLayout.u1;
        *(int *)&p->u2 = *(int *)&targetIcon->texLayout.u2;
        *(short *)&p->u3 = *(short *)&targetIcon->texLayout.u3;
        if (iTile == 0) {
            p->x0 = centerX; p->y0 = centerY;
            p->x1 = sRightX; p->y1 = centerY;
            p->x2 = centerX; p->y2 = sBottomY;
            p->x3 = sRightX; p->y3 = sBottomY;
        } else if (iTile == 1) {
            sMirror = (centerX + (short)(iQuadW << 1)) - sHalfX;
            p->x0 = sMirror;      p->y0 = centerY;
            p->x1 = sInnerRightX; p->y1 = centerY;
            p->x2 = sMirror;      p->y2 = sBottomY;
            p->x3 = sInnerRightX; p->y3 = sBottomY;
        } else {
            if (iTile == 2) {
                sMirror = (centerY + sQuadH * 2) - sHalfY;
                p->x0 = centerX; p->y0 = sMirror;
                p->x1 = sRightX; p->y1 = sMirror;
                p->x2 = centerX; p->y2 = sInnerBottomY;
                p->x3 = sRightX;
            } else {
                sMirrorX = (centerX + sQuadW * 2) - sHalfX;
                sMirror = (centerY + sQuadH * 2) - sHalfY;
                p->x0 = sMirrorX;     p->y0 = sMirror;
                p->x1 = sInnerRightX; p->y1 = sMirror;
                p->x2 = sMirrorX;     p->y2 = sInnerBottomY;
                p->x3 = sInnerRightX;
            }
            p->y3 = sInnerBottomY;
        }
        ((char *)&p->tag)[3] = 9;
        p->code = 0x2c;
        if (transparency != 0) {
            p->tpage = (p->tpage & 0xff9f) | (unsigned short)((transparency - 1) * 0x20);
            p->code |= 2;
        }
        func_80075310(ot, p);
        iTile = iTile + 1;
        primMem->curr = (char *)primMem->curr + 0x28;
    } while (iTile < 4);
}
