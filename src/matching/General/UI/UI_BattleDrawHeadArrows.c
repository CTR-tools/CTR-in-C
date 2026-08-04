/*
 * UI_BattleDrawHeadArrows @ 0x8004F9D8 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg112, 4120 — register-coloring wall typical of the
 * GTE + GPU-primitive builders). Real types (uitypes.h / gametracker.h / driver.h).
 *
 * Draws a floating gouraud-triangle arrow (POLY_G3, semi-trans, tpage 0xE1000A20) above every
 * OTHER visible, unfinished driver's head, as seen from `player`'s view. Loads player's view
 * matrix (pushBuffer[playerID].matrix_ViewProj) into the GTE, then per driver i (skipping self,
 * invisibleTimer!=0, or actionsFlagSet bit 0x2000000): rejects if within ~768 units
 * (playerDistance <= 0x90000), else RTPS-projects the driver's world position, skips if the GTE
 * FLAG has bit 0x40000 (off-screen), allocates a prim from backBuffer->primMem, sizes the arrow
 * from distance (iVar6 = 0x1000 - playerDistance/6>>13), colors it from the team gradient, and
 * links it into pushBuffer[playerID].ptrOT.
 *
 * NOTE(claude) vs in-repo decomp (which is a REFERENCE; 926 asm is authoritative):
 *   1) The arrow is a 3-COLOR gradient: r0=ptrColor[i][0], r1=ptrColor[i][1], r2=ptrColor[i][2]
 *      (the decomp collapses all three to one color — verified 3 distinct reads in the 926 asm).
 *   2) Distance cull boundary is `playerDistance <= 0x90000` (draw when strictly greater); the
 *      decomp's `0x90000 > playerDistance` differs by one at the exact boundary (retail uses
 *      `slt 0x90000, pd; beqz`).
 * PLAYER_BLUE (0x18) is the base index into data.ptrColor (D_80081D70) for battle teams.
 * gGT (D_8008D2AC) is reloaded per-region (not cached in a callee-saved reg), matching retail.
 * Phase B (saphi): see note below.
 */
#include "../uitypes.h"

#define PLAYER_BLUE 0x18

extern struct GameTracker *D_8008D2AC;      /* gGT (absolute) */
extern unsigned int *D_80081D70[];          /* data.ptrColor[] — per-team 3-color gradients */

/* GTE inline intrinsics (retail-exact instruction sequences). */
#define gte_SetRotMatrix(m) __asm__ volatile( \
    "lw $12,0(%0);lw $13,4(%0);ctc2 $12,$0;ctc2 $13,$1;" \
    "lw $12,8(%0);lw $13,12(%0);lw $14,16(%0);ctc2 $12,$2;ctc2 $13,$3;ctc2 $14,$4" \
    :: "r"(m) : "$12","$13","$14")
#define gte_SetTransMatrix(m) __asm__ volatile( \
    "lw $12,20(%0);lw $13,24(%0);ctc2 $12,$5;lw $14,28(%0);ctc2 $13,$6;ctc2 $14,$7" \
    :: "r"(m) : "$12","$13","$14")
#define gte_ldv0(v)  __asm__ volatile("lwc2 $0,0(%0)\nlwc2 $1,4(%0)" :: "r"(v))
#define gte_rtps()   __asm__ volatile("nop\nnop\ncop2 0x0180001")
#define gte_stsxy(p) __asm__ volatile("swc2 $14,0(%0)" :: "r"(p))
#define gte_stflg(p) __asm__ volatile("cfc2 $12,$31\nnop\nsw $12,0(%0)" :: "r"(p) : "$12")

typedef struct { unsigned int tag; unsigned int tpage; POLY_G3 g3; } G3_SEMITRANS;

void UI_BattleDrawHeadArrows(struct Driver *player)
{
    int playerDistance;
    short colorIdx;
    short sVar1, sVar3;
    int iVar6;
    short outXY[2];
    unsigned int flag;
    MATRIX *m;
    G3_SEMITRANS *p;
    SVECTOR pos;
    int playerID;
    int i;
    struct Driver *currDriver;
    struct Instance *currInst, *playerInst;
    struct PushBuffer *backBuffer;
    unsigned int *ot;
    unsigned int **grad;
    int xDistance, zDistance;

    playerID = player->driverID;
    m = &D_8008D2AC->pushBuffer[playerID].matrix_ViewProj;
    gte_SetRotMatrix(m);
    gte_SetTransMatrix(m);

    for (i = 0; i < D_8008D2AC->numPlyrCurrGame; i++) {
        currDriver = D_8008D2AC->drivers[i];
        if (i == playerID) continue;
        if (currDriver->invisibleTimer != 0) continue;
        if ((currDriver->actionsFlagSet & 0x2000000) != 0) continue;

        sVar1 = 5;
        if (D_8008D2AC->numPlyrCurrGame < 3) sVar1 = 3;

        playerInst = player->instSelf;
        currInst = currDriver->instSelf;
        xDistance = playerInst->matrix.t[0] - currInst->matrix.t[0];
        zDistance = playerInst->matrix.t[2] - currInst->matrix.t[2];
        playerDistance = (xDistance * xDistance) + (zDistance * zDistance);
        if (playerDistance <= 0x90000) continue;

        pos.vx = currDriver->instSelf->matrix.t[0];
        colorIdx = PLAYER_BLUE + (unsigned short)currDriver->BattleHUD.teamID;
        pos.vy = currDriver->instSelf->matrix.t[1];
        pos.vz = currDriver->instSelf->matrix.t[2];

        gte_ldv0(&pos);
        gte_rtps();
        gte_stsxy(&outXY[0]);
        gte_stflg(&flag);
        if ((flag & 0x40000) != 0) continue;

        backBuffer = D_8008D2AC->backBuffer;
        p = (G3_SEMITRANS *)backBuffer->primMem.curr;
        if ((unsigned int)backBuffer->primMem.endMin100 < (unsigned int)p) return;
        backBuffer->primMem.curr = (char *)(p + 1);

        outXY[1] = outXY[1] + sVar1;
        iVar6 = 0x1000 - ((playerDistance / 6 + (playerDistance >> 0x1f) >> 0xd) - (playerDistance >> 0x1f));
        sVar1 = (short)(iVar6 * 3 >> 10);
        sVar3 = (short)(iVar6 * 7 >> 12) + 12;

        p->g3.x2 = outXY[0] + sVar1;
        p->g3.y2 = outXY[1] - sVar3;
        p->g3.x1 = outXY[0];
        p->g3.y1 = outXY[1] - 12;
        p->g3.x0 = outXY[0] - sVar1;
        ((char *)&p->tag)[3] = 8;
        p->g3.code = 0x32;
        p->tpage = 0xe1000a20;
        p->g3.tag = 0;
        p->g3.y0 = outXY[1] - sVar3;

        grad = &D_80081D70[colorIdx];
        *(int *)&p->g3.r1 = grad[0][1] | 0x30000000;
        *(int *)&p->g3.r0 = (grad[0][0] & 0xffffff) | 0x30000000;
        *(int *)&p->g3.r2 = grad[0][2] | 0x30000000;

        ot = D_8008D2AC->pushBuffer[playerID].ptrOT;
        *(int *)p = *ot | 0x8000000;
        *ot = (unsigned int)p & 0xffffff;
    }
}
