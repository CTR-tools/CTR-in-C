/*
 * UI_DrawDriverIcon @ 0x8004E8D8 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg27, 2555). Real types (uitypes.h POLY_FT4).
 * Builds+emits one textured POLY_FT4 quad for a driver/HUD icon at (posX,posY): color=colorCode,
 * copies icon texLayout UVs/clut/tpage; width=(u1-u0)*scale>>12, height=(v2-v0)*scale>>12. Y coords
 * clamped to 0xA5 (165, NTSC on-screen); when the bottom is clamped, bottom V is cropped
 * (v = v0 + clampedBottomY - posY). transparency>0 sets tpage semi-trans (transparency-1) + code bit.
 * setPolyFT4 (code 0x2C); AddPrim; curr += 0x28.
 * Phase B (saphi UI_Icon.c:265): CLEAN — identical Y-clamp-to-165 + bottom-V crop + UV/color/transparency
 * (the EurRetail 176/175 clamp path is compiled out for NTSC 926).
 */
#include "../uitypes.h"

extern void func_80075310(unsigned long *ot, void *p);   /* AddPrim */

void UI_DrawDriverIcon(struct Icon *icon, short posX, int posY, struct PrimMem *primMem, unsigned long *ot,
                   char transparency, short scale, unsigned int colorCode)
{
    POLY_FT4 *p;
    int widthSpan;
    int height;
    int bottomY;

    p = (POLY_FT4 *)primMem->curr;
    *(int *)&p->r0 = colorCode;
    *(int *)&p->u0 = *(int *)&icon->texLayout.u0;
    *(int *)&p->u1 = *(int *)&icon->texLayout.u1;
    widthSpan = icon->texLayout.u1 - icon->texLayout.u0;
    *(int *)&p->u2 = *(int *)&icon->texLayout.u2;
    *(short *)&p->u3 = *(short *)&icon->texLayout.u3;
    p->x0 = posX;
    height = icon->texLayout.v2 - icon->texLayout.v0;
    if (posY < 0xa6) {
        p->y0 = (short)posY;
    } else {
        p->y0 = 0xa5;
    }
    p->x1 = posX + (short)((widthSpan * scale) >> 0xc);
    if (posY < 0xa6) {
        p->y1 = (short)posY;
    } else {
        p->y1 = 0xa5;
    }
    bottomY = posY + ((height * scale) >> 0xc);
    p->x2 = posX;
    if (bottomY < 0xa6) {
        p->y2 = (short)bottomY;
    } else {
        p->y2 = 0xa5;
    }
    p->x3 = posX + (short)((widthSpan * scale) >> 0xc);
    bottomY = posY + ((height * scale) >> 0xc);
    if (bottomY < 0xa6) {
        p->y3 = (short)bottomY;
    } else {
        p->y3 = 0xa5;
    }
    ((char *)&p->tag)[3] = 9;
    p->code = 0x2c;
    p->v2 = (p->v0 + (char)p->y2) - (char)posY;
    p->v3 = (p->v0 + (char)p->y3) - (char)posY;
    if (transparency != 0) {
        p->tpage = (p->tpage & 0xff9f) | (unsigned short)((transparency - 1) * 0x20);
        p->code |= 2;
    }
    func_80075310(ot, p);
    primMem->curr = (char *)primMem->curr + 0x28;
}
