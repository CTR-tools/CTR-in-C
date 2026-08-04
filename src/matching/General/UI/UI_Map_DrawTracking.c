/*
 * UI_Map_DrawTracking @ 0x8004DFFC (SCUS_944.26 / NTSC-U 926)
 * Byte-exact match (score 0). Real types (uitypes.h).
 * Walks the sibling-thread list; for each warpball (model->id == DYNAMIC_WARPBALL 0x36) draws its
 * map marker (icon 0x20) at inst->matrix.t, and if it has a homing target driver
 * (inst->thread->object->driverTarget) draws a target marker (icon 0x21) at the target's
 * instSelf->matrix.t, flickering color 4<->3 by gGT->timer&1.
 * Phase B (saphi UI_Map.c:360): CLEAN — identical loop + inst->thread->object->driverTarget
 * access path (matches 926 asm) + timer&1 flicker.
 */
#include "../uitypes.h"

#define DYNAMIC_WARPBALL 0x36

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
/* UI_Map_DrawRawIcon(mapCtx, &pos.t[0], icon, color, a4, a5) */
extern void func_8004DC44(unsigned long *mapCtx, int *pos, int icon, int color, int a4, int a5);

void UI_Map_DrawTracking(unsigned long *mapCtx, struct Thread *threadList)
{
    struct Instance *inst;
    struct Driver *d;
    int colorIdx;

    for (; threadList != 0; threadList = threadList->siblingThread) {
        inst = threadList->inst;
        if (inst->model->id == DYNAMIC_WARPBALL) {
            func_8004DC44(mapCtx, (int *)inst->matrix.t, 0x20, 0, 0, 0x1000);
            d = ((struct TrackerWeapon *)inst->thread->object)->driverTarget;
            colorIdx = 4;
            if (d != 0) {
                int *pos = (int *)d->instSelf->matrix.t;
                if ((D_8008D2AC->timer & 1) != 0) {
                    colorIdx = 3;
                }
                func_8004DC44(mapCtx, pos, 0x21, colorIdx, 0, 0x1000);
            }
        }
    }
}
