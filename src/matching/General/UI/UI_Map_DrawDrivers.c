/*
 * UI_Map_DrawDrivers @ 0x8004DD5C (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg-coloring: mapCtx/0x100000-mask s1<->s2 swap). Real types.
 * Draws every racer's minimap blip (shared-screen only: numPlyrCurrGame 1 or 3 per-bucket; else skip).
 * kartColor = g_awCharacterIDs[driver->driverID]+5. AI (actionsFlagSet&0x100000): icon 0x31. Human:
 * icon 0x32, blinks WHITE(4) on even frames (timer&2); in Adventure Arena (gameMode1&0x100000) drawn
 * via UI_Map_DrawAdvPlayer (arrow rotated by rotCurr.y). Pos = inst->matrix.t. *pCounter++ per thread.
 * NOTE: 926 checks the player-count per-bucket (decomp hoists it to an early return).
 * Phase B (saphi UI_Map.c:253): CLEAN — saphi's NOTE documents it FIXED the decomp early-return to
 * retail's per-bucket check + counter advance. ACTION_BOT==0x100000 and ADVENTURE_ARENA==0x100000 verified.
 */
#include "../uitypes.h"

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
extern unsigned short D_80086E84[];      /* g_awCharacterIDs[driverID] (lhu) */
/* UI_Map_DrawAdvPlayer(mapCtx, &pos.t[0], icon, color, rot, scale) */
extern void func_8004DBAC(unsigned long *mapCtx, int *pos, int icon, int color, int rot, int scale);
/* UI_Map_DrawRawIcon(mapCtx, &pos.t[0], icon, color, a4, a5) */
extern void func_8004DC44(unsigned long *mapCtx, int *pos, int icon, int color, int a4, int a5);

void UI_Map_DrawDrivers(unsigned long *mapCtx, struct Thread *threadList, short *pCounter)
{
    int kartColor;
    int iconIdx;
    struct GameTracker *gGT;
    struct Instance *inst;
    struct Driver *pDriver;

    do {
        if (threadList == 0) {
            return;
        }
        gGT = D_8008D2AC;
        inst = threadList->inst;
        if (gGT->numPlyrCurrGame == 1 || gGT->numPlyrCurrGame == 3) {
            pDriver = (struct Driver *)threadList->object;
            kartColor = D_80086E84[pDriver->driverID] + 5;
            if ((pDriver->actionsFlagSet & 0x100000) == 0) {
                /* human */
                if ((gGT->gameMode1 & 0x100000) != 0) {
                    /* Adventure Arena — draw rotated arrow, skip the normal icon */
                    if ((gGT->timer & 2) == 0) {
                        kartColor = 4;
                    } else {
                        kartColor = (short)kartColor;
                    }
                    func_8004DBAC(mapCtx, (int *)inst->matrix.t, 0x32, kartColor,
                                  (pDriver->rotCurr.y + 0x800) | 0x1000, 0x800);
                    goto next;
                }
                if ((gGT->timer & 2) == 0) {
                    kartColor = 4;
                } else {
                    kartColor = (short)kartColor;
                }
                iconIdx = 0x32;
            } else {
                /* AI */
                iconIdx = 0x31;
                kartColor = (short)kartColor;
            }
            func_8004DC44(mapCtx, (int *)inst->matrix.t, iconIdx, kartColor, 0, 0x1000);
        }
    next:
        *pCounter += 1;
        threadList = threadList->siblingThread;
    } while (1);
}
