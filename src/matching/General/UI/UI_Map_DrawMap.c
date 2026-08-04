/*
 * UI_Map_DrawMap @ 0x8004D614 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (large reg residual = GPU field-write ordering + coloring wall
 * on a 168-insn 2-quad primitive builder). Real types (uitypes.h: POLY_FT4/Icon/PrimMem/Level).
 * Draws the minimap as two stacked textured POLY_FT4 quads (top+bottom) at bottom-right (posX,posY).
 * colorID: 0/1=gray 0x808080 vtx (1 also forces texpage blend bits), 2=black, 3=blue 0x402000. Top
 * quad drawn only when the ST1_MAP spawn says so (*(short*)(mapSpawn+0x12)==0) or in MAIN_MENU
 * (gameMode1&0x2000); bottom always. ExtraFunc inlined twice (no widescreen scaling in this build).
 * Phase B (saphi UI_Map.c:4): CLEAN — same color logic / top-quad condition / quad build; saphi's NOTE
 * documents the retail-aligning ptrSpawnType1->count check + a defensive native null-guard (behavior-neutral).
 */
#include "../uitypes.h"

#define MAIN_MENU 0x2000

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
extern void func_80075310(unsigned long *ot, void *p);   /* AddPrim */

void UI_Map_DrawMap(struct Icon *mapTop, struct Icon *mapBottom, short posX, short posY,
                   struct PrimMem *primMem, unsigned long *otMem, unsigned int colorID)
{
    int iMapSpawn;
    POLY_FT4 *p;
    unsigned int color;
    unsigned int colorMode;
    struct GameTracker *gGT;
    struct SpawnType1 *sp;
    short mapBottomHeight;
    short leftX;

    colorMode = colorID & 0xff;
    iMapSpawn = 0;
    color = 0x808080;
    if (colorMode == 2) {
        colorMode = 0;
        color = 0;
    } else if (colorMode == 3) {
        colorMode = 0;
        color = 0x402000;
    }

    gGT = D_8008D2AC;
    sp = gGT->level1->ptrSpawnType1;
    if (sp->count != 0) {
        iMapSpawn = *(int *)((char *)sp + 4);
    }

    mapBottomHeight = mapBottom->texLayout.v2 - mapBottom->texLayout.v0;

    if (((iMapSpawn != 0) && (*(short *)(iMapSpawn + 0x12) == 0)) ||
        ((gGT->gameMode1 & MAIN_MENU) != 0)) {
        p = (POLY_FT4 *)primMem->curr;
        *(int *)&p->r0 = color;
        p->y0 = posY - (((unsigned short)mapTop->texLayout.v2 - (unsigned short)mapTop->texLayout.v0) + mapBottomHeight);
        p->y1 = p->y0;
        p->y2 = posY - mapBottomHeight;
        p->y3 = posY - mapBottomHeight;

        leftX = posX - (mapTop->texLayout.u1 - mapTop->texLayout.u0);
        p->x0 = leftX;
        p->x1 = posX;
        p->x2 = leftX;
        p->x3 = posX;
        ((char *)&p->tag)[3] = 9;
        p->code = 0x2c;
        *(int *)&p->u0 = *(int *)&mapTop->texLayout.u0;
        *(int *)&p->u1 = *(int *)&mapTop->texLayout.u1;
        *(short *)&p->u2 = *(short *)&mapTop->texLayout.u2;
        *(short *)&p->u3 = *(short *)&mapTop->texLayout.u3;
        if (colorMode != 0) {
            p->tpage = (p->tpage & 0xff9f) | (unsigned short)(colorMode << 5);
        }
        p->code |= 2;
        func_80075310(otMem, p);
        primMem->curr = (char *)primMem->curr + 0x28;
    }

    p = (POLY_FT4 *)primMem->curr;
    *(int *)&p->r0 = color;
    p->y0 = posY - mapBottomHeight;
    p->y1 = posY - mapBottomHeight;
    p->y2 = posY;
    p->y3 = posY;

    leftX = posX - (mapBottom->texLayout.u1 - mapBottom->texLayout.u0);
    p->x0 = leftX;
    p->x1 = posX;
    p->x2 = leftX;
    p->x3 = posX;
    ((char *)&p->tag)[3] = 9;
    p->code = 0x2c;
    *(int *)&p->u0 = *(int *)&mapBottom->texLayout.u0;
    *(int *)&p->u1 = *(int *)&mapBottom->texLayout.u1;
    *(short *)&p->u2 = *(short *)&mapBottom->texLayout.u2;
    *(short *)&p->u3 = *(short *)&mapBottom->texLayout.u3;
    if (colorMode != 0) {
        p->tpage = (p->tpage & 0xff9f) | (unsigned short)(colorMode << 5);
    }
    p->code |= 2;
    func_80075310(otMem, p);
    primMem->curr = (char *)primMem->curr + 0x28;
}
