/*
 * UI_TrackerSelf @ 0x8004FD34 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg286, score 18310 — inline-GTE tracker renderer; residual is
 * the register-COLORING wall: `driver` colors to s4 vs retail s1, renumbering the whole s-file across
 * the state machine + the two POLY_G3 chevron builders. Non-behavioral — GTE projection, all globals,
 * the beep/warpball logic, and both prim builds are verified.) Real types + retail-exact GTE __asm__.
 * (Previously ⏸ monster.)
 *
 * Draws the incoming-missile/warpball homing-warning tracker for the local player (pulsing lock-on
 * reticle + L/R warning chevrons around your kart, with proximity beeping). Per-driver state tables
 * (indexed by driverID@0x4A): g_awTrackerTimer (pulse countdown), g_awTrackerDist (last distance),
 * g_awTrackerType (0=missile, 1=warpball). driver->thTrackingMe@0x4A4 = the thread homing on you.
 * Clears type + returns if nothing is tracking and the timer expired; on a NEW lock sets actionsFlagSet
 * bit 0x4000000 + resets the timer (8/12). Projects the player position (instSelf->matrix.t) to screen
 * via the player's tileView viewproj (gte_SetRot/Trans/ldv0/rtps/stsxy). Distance =
 * VehCalc_FastSqrt(TrackerWeapon.distanceToTarget)/0x32; beeps OtherFX_Play(0x56) at a rate that
 * quickens as it closes (every 30/10/5 frames by >200/>100/near), flashing white. For a warpball
 * (tracked inst model id 0x36) gates the warning by along-track lap distance (< 16000) vs the player's
 * checkpoint node. Draws the chevrons as paired gouraud POLY_G3 (colors by type) into
 * tileView[driverID].ptrOT, then UI_TrackerBG (icon ptrIcons[0x2d]) for the reticle.
 *
 * NOTE(claude) vs decomp UI_26 (926 authoritative): the FastSqrt-distance beep comparisons are SIGNED
 * (slti). Tracker tables g_awTrackerTimer@D_800862E8 / Dist@D_800862F0 / Type@D_800862F8 / Anim1@
 * D_8008626C / Anim2@D_8008628C. TrackerWeapon(thread->object): flags@0x16, distanceToTarget@0x28,
 * ptrNodeCurr@0x3c; CheckpointNode.distToFinish@6 (stride 0xc); Level.ptr_restart_points@0x14C.
 * RB_GetThread_ClosestTracker=func_800B28C0, VehCalc_FastSqrt=func_80028468, OtherFX_Play=func_80059070,
 * UI_TrackerBG=func_8004E660.
 *
 * Phase B (saphi game/UI/UI_RaceHud.c UI_TrackerSelf): CLEAN — no bugs. The state machine (flags@0x16
 * &0x10, ACTION_TRACKER_TARGETED=0x4000000, the re-read trackerTimer decrement guard), the beep-rate
 * boundaries (saphi `>100→10, >200→30` == my `<201&&>100→10` — identical at 200/201), the flash +
 * PAUSE gate, and the warpball lap-dist ((ptrNodeCurr->distToFinish - firstNode[currentIndex].
 * distToFinish)*8 with the <0 wrap; currentIndex = unknown_lap_related[1]) all match. saphi already
 * carries retail-verified notes (NOTE(claude) trackerTimer re-read, NOTE(aalhendi) beep SFX args).
 */
#include "../uitypes.h"

#define DYNAMIC_WARPBALL 0x36
#define PAUSE_MASK       0xf0000000

#define gte_SetRotMatrix(m) __asm__ volatile( \
    "lw $12,0(%0);lw $13,4(%0);ctc2 $12,$0;ctc2 $13,$1;" \
    "lw $12,8(%0);lw $13,12(%0);lw $14,16(%0);ctc2 $12,$2;ctc2 $13,$3;ctc2 $14,$4" \
    :: "r"(m) : "$12","$13","$14")
#define gte_SetTransMatrix(m) __asm__ volatile( \
    "lw $12,20(%0);lw $13,24(%0);ctc2 $12,$5;lw $14,28(%0);ctc2 $13,$6;ctc2 $14,$7" \
    :: "r"(m) : "$12","$13","$14")
#define gte_ldv0(v)  __asm__ volatile("lwc2 $0,0(%0)\nlwc2 $1,4(%0)" :: "r"(v))
#define gte_rtps()   __asm__ volatile("nop\nnop\n.word 0x4a180001")
#define gte_stsxy(p) __asm__ volatile("swc2 $14,0(%0)" :: "r"(p))

extern struct GameTracker *D_8008D2AC;   /* sdata_gGT */
extern short D_800862E8[];               /* g_awTrackerTimer */
extern short D_800862F0[];               /* g_awTrackerDist */
extern short D_800862F8[];               /* g_awTrackerType */
extern char D_8008626C[];                /* g_awTrackerAnim1 */
extern char D_8008628C[];                /* g_awTrackerAnim2 */

extern struct Thread *func_800B28C0(struct Driver *d);           /* RB_GetThread_ClosestTracker */
extern unsigned int func_80028468(unsigned int x, int y);        /* VehCalc_FastSqrt */
extern void func_80059070(int id, int a);                        /* OtherFX_Play */
extern void func_8004E660(struct Icon *icon, short x, short y, struct PrimMem *pm, unsigned long *ot,
                          int a, short animX, short animY, int color); /* UI_TrackerBG */

void UI_TrackerSelf(struct Driver *driver)
{
    short sTimer;
    struct Thread *pTrackerThread;
    MATRIX *r0;
    unsigned int uDist;
    short *pTimer;
    unsigned int uBeepRate;
    struct PushBuffer *pBackBuf;
    char *pAnimTable;
    struct CheckpointNode *pCheckNode;
    unsigned int *pPrimCurr;
    unsigned int uColor1;
    unsigned int *pPrim;
    void *pOt;
    short sSide;
    int uFlashColor;
    int iChevronIdx;
    unsigned int driverId;
    int iLapDist;
    unsigned short uAnimX;
    unsigned short uAnimY;
    char *pProjScratch;
    char auStack_58[40];
    short posX;
    short posY;
    short posZ;
    struct GameTracker *pGt;
    short sChevronX;
    short sOuterX;
    short sReticleSize;

    pProjScratch = auStack_58;
    driverId = (unsigned int)driver->driverID;
    if ((driver->thTrackingMe == 0) && (D_800862E8[driverId] == 0)) {
        D_800862F8[driverId] = 0;
        return;
    }
    iLapDist = 0;
    if (driver->thTrackingMe == 0) {
        sTimer = D_800862E8[driverId];
        pAnimTable = &D_8008628C[0];
    } else {
        sTimer = D_800862E8[driverId];
        pAnimTable = &D_8008626C[0];
    }
    uAnimX = *(unsigned short *)(pAnimTable + sTimer * 4) >> 8;
    uAnimY = *(unsigned short *)((int)(pAnimTable + sTimer * 4) + 2);
    pTrackerThread = func_800B28C0(driver);
    if ((pTrackerThread == 0) && (pTimer = D_800862E8 + driverId, *pTimer == 0)) {
        if ((((struct TrackerWeapon *)driver->thTrackingMe->object)->flags & 0x10) != 0) goto LAB_8004fe8c;
        sTimer = 0xc;
    } else {
        if ((((driver->actionsFlagSet & 0x4000000) != 0) && (pTrackerThread == driver->thTrackingMe)) ||
            (pTimer = D_800862E8 + driverId, *pTimer != 0)) goto LAB_8004fe8c;
        driver->actionsFlagSet = driver->actionsFlagSet | 0x4000000;
        sTimer = 8;
    }
    *pTimer = sTimer;
LAB_8004fe8c:
    iChevronIdx = driverId * 2;
    driver->thTrackingMe = pTrackerThread;
    pTimer = D_800862E8 + driverId;
    if (*pTimer != 0) {
        *pTimer = *pTimer + -1;
    }
    posX = (short)driver->instSelf->matrix.t[0];
    posY = (short)driver->instSelf->matrix.t[1];
    posZ = (short)driver->instSelf->matrix.t[2];
    *(short *)(pProjScratch + 0x28) = posX;
    *(short *)(pProjScratch + 0x2a) = posY;
    *(short *)(pProjScratch + 0x2c) = posZ;
    r0 = &D_8008D2AC->pushBuffer[driverId].matrix_ViewProj;
    gte_SetRotMatrix(r0);
    gte_SetTransMatrix(r0);
    gte_ldv0((short *)(pProjScratch + 0x28));
    gte_rtps();
    gte_stsxy((long *)(pProjScratch + 0x30));
    uFlashColor = 0xff;
    if (driver->thTrackingMe == 0) {
        uDist = (unsigned int)*(short *)((int)D_800862F0 + iChevronIdx);
    } else {
        uDist = func_80028468(((struct TrackerWeapon *)driver->thTrackingMe->object)->distanceToTarget, 0);
        uDist = uDist / 0x32;
        *(short *)((int)D_800862F0 + iChevronIdx) = (short)uDist;
        uBeepRate = 0x1e;
        if (((int)uDist < 0xc9) && (uBeepRate = 5, 100 < (int)uDist)) {
            uBeepRate = 10;
        }
        uFlashColor = 0xff;
        if ((unsigned int)D_8008D2AC->timer % uBeepRate == 0) {
            if ((D_8008D2AC->gameMode1 & PAUSE_MASK) == 0) {
                func_80059070(0x56, 1);
            }
            uFlashColor = 0xffffff;
        }
        if (driver->thTrackingMe->inst->model->id == DYNAMIC_WARPBALL) {
            pCheckNode = D_8008D2AC->level1->ptr_restart_points;
            iLapDist = ((unsigned int)((struct TrackerWeapon *)driver->thTrackingMe->object)->ptrNodeCurr->distToFinish -
                        (unsigned int)pCheckNode[driver->unknown_lap_related[1]].distToFinish) * 8;
            if (iLapDist < 0) {
                iLapDist = iLapDist + (unsigned int)pCheckNode->distToFinish * 8;
            }
            D_800862F8[driverId] = 1;
        } else {
            D_800862F8[driverId] = 0;
        }
    }
    iChevronIdx = 0;
    if (iLapDist < 16000) {
        sReticleSize = (short)uDist;
        sOuterX = sReticleSize + (char)(uAnimX >> 8);
        sTimer = (short)((short)uAnimY * 7 >> 0xc);
        do {
            sSide = 1;
            if (iChevronIdx == 0) {
                sSide = -1;
            }
            pBackBuf = D_8008D2AC->backBuffer;
            pPrimCurr = (unsigned int *)pBackBuf->primMem.curr;
            pPrim = 0;
            if (pPrimCurr <= (unsigned int *)pBackBuf->primMem.endMin100) {
                pBackBuf->primMem.curr = pPrimCurr + 7;
                pPrim = pPrimCurr;
            }
            if (pPrim == 0) {
                return;
            }
            if (D_800862F8[driverId] == 1) {
                uBeepRate = 0x309c6900;
                uDist = 0x30ffff00;
            } else {
                uBeepRate = 0x3000699c;
                uDist = 0x3000ffff;
            }
            pPrim[3] = 0x30ffffff;
            pPrim[1] = uBeepRate;
            pPrim[5] = uDist;
            sChevronX = sSide * sOuterX;
            *(short *)(pPrim + 6) = *(short *)(pProjScratch + 0x30) + sChevronX;
            *(short *)((int)pPrim + 0x1a) = *(short *)(pProjScratch + 0x32) + -0xc;
            *(short *)(pPrim + 4) = *(short *)(pProjScratch + 0x30) + sSide * sReticleSize;
            *(short *)((int)pPrim + 0x12) = *(short *)(pProjScratch + 0x32) + -0xc;
            *(short *)(pPrim + 2) = *(short *)(pProjScratch + 0x30) + sChevronX;
            pGt = D_8008D2AC;
            *(short *)((int)pPrim + 10) = *(short *)(pProjScratch + 0x32) - (sTimer + 0xc);
            pPrimCurr = pGt->pushBuffer[driverId].ptrOT;
            *pPrim = *pPrimCurr | 0x6000000;
            *pPrimCurr = (unsigned int)pPrim & 0xffffff;
            pBackBuf = D_8008D2AC->backBuffer;
            pPrimCurr = (unsigned int *)pBackBuf->primMem.curr;
            pPrim = 0;
            if (pPrimCurr <= (unsigned int *)pBackBuf->primMem.endMin100) {
                pBackBuf->primMem.curr = pPrimCurr + 7;
                pPrim = pPrimCurr;
            }
            if (pPrim == 0) {
                return;
            }
            if (D_800862F8[driverId] == 1) {
                uColor1 = 0x305b5b00;
                uBeepRate = 0x30322b01;
                uDist = 0x30ffbb00;
            } else {
                uColor1 = 0x30005b5b;
                uBeepRate = 0x30012b32;
                uDist = 0x3000bbff;
            }
            pPrim[3] = uColor1;
            pPrim[1] = uBeepRate;
            pPrim[5] = uDist;
            sChevronX = sSide * sOuterX;
            *(short *)(pPrim + 6) = *(short *)(pProjScratch + 0x30) + sChevronX;
            *(short *)((int)pPrim + 0x1a) = *(short *)(pProjScratch + 0x32) + sTimer + -0xc;
            *(short *)(pPrim + 4) = *(short *)(pProjScratch + 0x30) + sSide * sReticleSize;
            *(short *)((int)pPrim + 0x12) = *(short *)(pProjScratch + 0x32) + -0xc;
            iChevronIdx = iChevronIdx + 1;
            *(short *)(pPrim + 2) = *(short *)(pProjScratch + 0x30) + sChevronX;
            pGt = D_8008D2AC;
            *(short *)((int)pPrim + 10) = *(short *)(pProjScratch + 0x32) + -0xc;
            pPrimCurr = pGt->pushBuffer[driverId].ptrOT;
            *pPrim = *pPrimCurr | 0x6000000;
            *pPrimCurr = (unsigned int)pPrim & 0xffffff;
        } while (iChevronIdx < 2);
    }
    pGt = D_8008D2AC;
    pBackBuf = D_8008D2AC->backBuffer;
    pOt = D_8008D2AC->pushBuffer[driverId].ptrOT;
    *(int *)(pProjScratch + 0x14) = 1;
    *(int *)(pProjScratch + 0x18) = (int)(short)uAnimX;
    *(int *)(pProjScratch + 0x1c) = (int)(short)uAnimY;
    *(int *)(pProjScratch + 0x20) = uFlashColor;
    *(void **)(pProjScratch + 0x10) = pOt;
    func_8004E660(pGt->ptrIcons[0x2d],
                  *(short *)(pProjScratch + 0x30) - (short)((int)(short)uAnimX >> 7),
                  *(short *)(pProjScratch + 0x32) - (short)((short)uAnimY * 0xf >> 0xb),
                  &pBackBuf->primMem, *(unsigned long **)(pProjScratch + 0x10), *(int *)(pProjScratch + 0x14),
                  *(short *)(pProjScratch + 0x18), *(short *)(pProjScratch + 0x1c),
                  *(int *)(pProjScratch + 0x20));
}
