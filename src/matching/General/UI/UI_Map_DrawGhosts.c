/*
 * UI_Map_DrawGhosts @ 0x8004DEE8 (SCUS_944.26 / NTSC-U 926)
 * Byte-exact match (score 0). Real types (uitypes.h).
 * Walks the ghost thread list; for each started ghost (d=thread->object; d->ghostBoolInit@0x632 != 0)
 * picks a map-icon color by d->ghostID@0x630: human (0) blinks 6/5 by gGT->timer&1; N.Tropy/Oxide (!=0)
 * uses 0x11, or blinks 3/4 when Oxide is unlocked (highScoreTracks[levelID].timeTrialFlags & 2). Draws
 * icon 0x31 at thread->inst->matrix.t. NOTE: the highScoreTracks table (base D_8008E6E8, stride 0x124,
 * flags @ +0x12C) is accessed by offset math — odd stride/field make a clean struct impractical.
 * Lever: invert to `if(ghostID!=0){tropy}else{human}` so tropy is the fall-through (matches block layout).
 * Phase B (saphi UI_Map.c:311): CLEAN — identical logic; color enum (namespace_Decal.h) verified vs
 * retail immediates: CORTEX_RED=6, CRASH_BLUE=5, TROPY_LIGHT_BLUE=0x11, RED=3, WHITE=4.
 */
#include "../uitypes.h"

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
extern char D_8008E6E8[];                /* sdata->gameProgress (highScoreTracks region, stride 0x124) */
/* UI_Map_DrawRawIcon(otContext, &pos.t[0], icon, color, a4, a5) */
extern void func_8004DC44(unsigned long *otContext, int *pos, int icon, int color, int a4, int a5);

void UI_Map_DrawGhosts(unsigned long *otContext, struct Thread *ghostThread)
{
    int colorIdx;

    for (; ghostThread != 0; ghostThread = ghostThread->siblingThread) {
        struct Driver *d = (struct Driver *)ghostThread->object;
        struct Instance *inst = ghostThread->inst;
        if (d->ghostBoolInit != 0) {
            if (d->ghostID != 0) {
                /* N.Tropy / Oxide ghost */
                char *track = D_8008E6E8 + D_8008D2AC->levelID * 0x124;
                colorIdx = 0x11;
                if ((*(int *)(track + 0x12C) & 2) != 0) {
                    colorIdx = 3;
                    if ((D_8008D2AC->timer & 1) != 0) {
                        colorIdx = 4;
                    }
                }
            } else {
                /* human ghost — blink 6/5 */
                colorIdx = 6;
                if ((D_8008D2AC->timer & 1) != 0) {
                    colorIdx = 5;
                }
            }
            func_8004DC44(otContext, (int *)inst->matrix.t, 0x31, colorIdx, 0, 0x1000);
        }
    }
}
