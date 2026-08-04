#include "../uitypes.h"

#define ACTION_RACE_FINISHED 0x2000000
#define ACTION_BEHIND_START  0x1000000
#define DYNAMIC_WARPBALL     0x36
#define FONT_SMALL           2

/* GTE path-projection intrinsics (warpball along-track position). */
#define gte_ldR11R12(v) __asm__ volatile("ctc2 %0,$0" :: "r"(v))
#define gte_ldR13R21(v) __asm__ volatile("ctc2 %0,$1" :: "r"(v))
#define gte_ldVXY0(v)   __asm__ volatile("mtc2 %0,$0" :: "r"(v))
#define gte_ldVZ0(v)    __asm__ volatile("mtc2 %0,$1" :: "r"(v))
#define gte_mvmva()     __asm__ volatile("nop\nnop\n.word 0x4a406012")
#define gte_stMAC1(x)   __asm__ volatile("mfc2 %0,$25" : "=r"(x))

extern struct GameTracker *D_8008D2AC;         /* sdata_gGT */
extern short D_800862B8[];                     /* g_awRankIconsDesired[8] */
extern short D_800862C8[];                     /* g_awRankIconsCurr[8] */
extern short D_800862D8[];                     /* g_awRankIconsTransitionTimer[8] (MP: eased bar X) */
extern struct MetaDataCharacter D_80086D84[];  /* g_anMetaDataCharacters[] */
extern short D_80086E84[];                     /* g_awCharacterIDs[] */
extern char D_8008D524;                        /* g_szWeaponQty (1-char rank string) */

extern void func_80022878(char *str, int x, short y, int font, int color);   /* DecalFont_DrawLine */
extern void func_80022EC4(struct Icon *icon, short x, short y, struct PrimMem *pm,
                          unsigned long *ot, int a, int b, int c);            /* DecalHUD warpball prim */
extern void func_80023054(struct Icon *icon, short x, short y, struct PrimMem *pm,
                          unsigned long *ot, unsigned int c0, unsigned int c1, unsigned int c2,
                          unsigned int c3, char one, int val);               /* DecalHUD_DrawPolyGT4 */
extern void func_8003D378(SVECTOR *v);                                        /* MATH_VectorNormalize */
extern void func_8004E8D8(struct Icon *icon, short x, int y, struct PrimMem *pm,
                          unsigned long *ot, int one, int z, int color);      /* UI_DrawDriverIcon */
extern void func_8004EAA8(short *out, short a, short b, short t);             /* UI_Lerp2D_Angular */

/*
 * UI_DrawRankedDrivers @ 0x800524C4 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg375, score 19115). Draw the race-standings driver icons,
 * two modes selected by numPlyrCurrGame:
 *   [1P - Arcade/Boss] Left-side vertical standings. Refresh g_awRankIconsDesired[i] = each driver's
 *     driverRank@0x482 (unless mid-transition), count finished racers (actionsFlagSet & 0x2000000),
 *     draw rank numbers '1'..'4' (or '1','2' in Boss, gameMode1<0) white(4)/red(3) by finished-state,
 *     then per driver draw its character icon (UI_DrawDriverIcon = func_8004E8D8; icon =
 *     ptrIcons[g_anMetaDataCharacters[characterID].iconID]) sliding between g_awRankIconsCurr and
 *     g_awRankIconsDesired via UI_Lerp2D_Angular (func_8004EAA8) over g_awRankIconsTransitionTimer[i]
 *     (5 fps), tinted by the per-driver damage-flash color inlined from driver->damageColorTimer@0x4AC
 *     (red/white pulse decaying toward 0; 0 -> 0x808080).
 *   [Multiplayer] Horizontal position bar at y=0x66. Per driver, screen X = progress along the lap
 *     (trackLen = level1->ptr_restart_points[0].distToFinish*8 - driver->distanceToFinish_curr@0x488,
 *     scaled /0x1d1), eased toward target (max step /0xe, snap if >400 or behind by >400); drivers
 *     still behind the start line (lapIndex@0x44 0 & actionsFlagSet & 0x1000000) pin to X=0. Icon via
 *     DecalHUD_DrawPolyGT4 (func_80023054), tinted by damage color. Then walks the tracking thread
 *     bucket (threadBuckets[6]) for warpballs (model id 0x36): reads each TrackerWeapon->ptrNodeCurr,
 *     projects its position onto the path (GTE mvmva = .word 0x4a406012) for along-track progress, and
 *     draws the warpball icon (ptrIcons[0xe]) via func_80022EC4.
 *
 * NOTE(claude): residual is the register-COLORING wall plus two ACCEPTED display cosmetics shared with
 * the wider UI render family (no behavioral divergence):
 *   (1) the MP damage-color RGB is a 3-byte CONCAT/uint3 value in retail, so the target copies it into
 *       the DecalHUD_DrawPolyGT4 color args with unaligned lwl/lwr/swl/swr; natural `int` C here copies
 *       aligned (lw/sw). Forcing the 3-byte type would be a non-natural hack ([[no-hacky-match-forcing]]).
 *   (2) the 1P finished-racers loop slides the raw gGT base with a 0x24EC load offset in retail; gcc
 *       folds 0x24EC into an `addiu` base here (same instruction count, different form).
 * distToFinish track-length divisions are SIGNED in retail (unsigned short * 8 promotes to signed int;
 * the Ghidra `(uint)` render is misleading) -> plain int math, not (unsigned int), to get div not divu.
 * Phase-B (saphi game/UI): CLEAN — see UI_PROGRESS.md.
 * (926/NTSC-U)
 */
void UI_DrawRankedDrivers(void)
{
    int iMpScratch;
    unsigned short flags;
    struct GameTracker *gtCursor;
    int uTrackLen;
    short *pRankAnimCursor;
    unsigned int uTrackScratch;
    short *pRankTableCursor;
    short nFinishedRacers;
    struct Instance *pWarpballInst;
    struct CheckpointNode *pRestartNode;
    unsigned short easedBarX;
    int iRankScratch;
    short *pSlotTimer;
    struct CheckpointNode *pNextNode;
    int iDriverIdx;
    int iTableOfs;
    struct Thread *pWarpballThread;
    unsigned short targetBarX;
    int iconPos;
    int relXY;
    short relZ;
    short warpX;
    short warpY;
    short warpZ;
    SVECTOR pathDir;
    int damageColor;
    int damageColorMp;
    int iEaseScratch;
    int bStandingCmp;
    unsigned short prevBarX;

    if (D_8008D2AC->numPlyrCurrGame == 1) {
        nFinishedRacers = 0;
        iDriverIdx = 0;
        pRankTableCursor = D_800862B8;
        pRankAnimCursor = D_800862D8;
        gtCursor = D_8008D2AC;
        do {
            if (*pRankAnimCursor == 0) {
                *pRankTableCursor = gtCursor->drivers[0]->driverRank;
            }
            if ((gtCursor->drivers[0] != 0) &&
                ((gtCursor->drivers[0]->actionsFlagSet & ACTION_RACE_FINISHED) != 0)) {
                nFinishedRacers = nFinishedRacers + 1;
            }
            gtCursor = (struct GameTracker *)((int)gtCursor + 4);
            pRankTableCursor = pRankTableCursor + 1;
            iDriverIdx = iDriverIdx + 1;
            pRankAnimCursor = pRankAnimCursor + 1;
        } while (iDriverIdx < 8);
        iDriverIdx = 4;
        if ((int)D_8008D2AC->gameMode1 < 0) {
            iDriverIdx = 2;
        }
        iTableOfs = 0;
        if (0 < iDriverIdx) {
            iRankScratch = 0x380000;
            bStandingCmp = 0 < nFinishedRacers;
            do {
                flags = 4;
                if (bStandingCmp) {
                    flags = 3;
                }
                (&D_8008D524)[0] = (char)iTableOfs + '1';
                func_80022878(&D_8008D524, 0x34, (short)((unsigned int)iRankScratch >> 0x10),
                              FONT_SMALL, flags);
                iRankScratch = iRankScratch + 0x1b0000;
                iTableOfs = iTableOfs + 1;
                bStandingCmp = iTableOfs < nFinishedRacers;
            } while (iTableOfs < iDriverIdx);
        }
        iDriverIdx = 0;
        pRankTableCursor = D_800862C8;
        pRankAnimCursor = D_800862B8;
        iTableOfs = 0;
        do {
            if ((D_8008D2AC->drivers[iDriverIdx] != 0) && (*pRankAnimCursor + 1 < 9)) {
                iRankScratch = D_8008D2AC->drivers[iDriverIdx]->damageColorTimer;
                damageColor = 0;
                if (0 < iRankScratch) {
                    *(char *)&damageColor =
                        ~((0x1e - iRankScratch) * 4);
                    *((char *)&damageColor + 1) =
                        (0x1e - D_8008D2AC->drivers[iDriverIdx]->damageColorTimer) * 4;
                    *((char *)&damageColor + 2) =
                        (0x1e - D_8008D2AC->drivers[iDriverIdx]->damageColorTimer) * 4;
                    D_8008D2AC->drivers[iDriverIdx]->damageColorTimer =
                        D_8008D2AC->drivers[iDriverIdx]->damageColorTimer + -1;
                } else if (iRankScratch < 0) {
                    *(char *)&damageColor =
                        (iRankScratch + 0x1e) * 4;
                    *((char *)&damageColor + 1) =
                        ~((D_8008D2AC->drivers[iDriverIdx]->damageColorTimer + 0x1e) * 4);
                    *((char *)&damageColor + 2) =
                        (D_8008D2AC->drivers[iDriverIdx]->damageColorTimer + 0x1e) * 4;
                    D_8008D2AC->drivers[iDriverIdx]->damageColorTimer =
                        D_8008D2AC->drivers[iDriverIdx]->damageColorTimer + 1;
                } else {
                    damageColor = 0x808080;
                }
                pSlotTimer = (short *)((int)D_800862D8 + iTableOfs);
                if (*pSlotTimer == 0) {
                    iRankScratch = (int)*pRankAnimCursor;
                    if (iRankScratch == *pRankTableCursor) goto fastpath;
                }
                func_8004EAA8((short *)&iconPos, *(short *)((int)D_800862C8 + iTableOfs),
                              *(short *)((int)D_800862B8 + iTableOfs), *pSlotTimer);
                func_8004E8D8(D_8008D2AC->ptrIcons
                              [(short)D_80086D84[*(short *)((int)D_80086E84 + iTableOfs)].iconID],
                              (short)iconPos, (int)(short)((unsigned int)iconPos >> 0x10),
                              &D_8008D2AC->backBuffer->primMem,
                              D_8008D2AC->pushBuffer_UI.ptrOT, 1, 0x1000, damageColor);
                nFinishedRacers = *pSlotTimer;
                *pSlotTimer = nFinishedRacers + 1;
                if (4 < (short)(nFinishedRacers + 1)) {
                    *pSlotTimer = 0;
                    *(short *)((int)D_800862C8 + iTableOfs) =
                        *(short *)((int)D_800862B8 + iTableOfs);
                }
                goto LAB_800528cc;
fastpath:
                if (iRankScratch + 1 < 5) {
                    func_8004E8D8(D_8008D2AC->ptrIcons
                                  [(short)D_80086D84
                                   [*(short *)((int)D_80086E84 + iTableOfs)].iconID],
                                  0x14, iRankScratch * 0x1b + 0x39,
                                  &D_8008D2AC->backBuffer->primMem,
                                  D_8008D2AC->pushBuffer_UI.ptrOT, 1, 0x1000, damageColor);
                    *pRankTableCursor = *pRankAnimCursor;
                }
            }
LAB_800528cc:
            pRankTableCursor = pRankTableCursor + 1;
            pRankAnimCursor = pRankAnimCursor + 1;
            iDriverIdx = iDriverIdx + 1;
            iTableOfs = iTableOfs + 2;
        } while (iDriverIdx < 8);
    } else {
        iDriverIdx = 0;
        if ((unsigned int)(unsigned char)D_8008D2AC->numPlyrCurrGame +
            (unsigned int)(unsigned char)D_8008D2AC->numBotsNextGame != 0) {
            pRankAnimCursor = D_800862D8;
            iRankScratch = 0;
            iTableOfs = 0;
            do {
                iMpScratch = *(int *)(*(int *)((int)D_8008D2AC->drivers + iTableOfs) + 0x4ac);
                damageColorMp = 0;
                if (0 < iMpScratch) {
                    *(char *)&damageColorMp =
                        ~((0x1e - iMpScratch) * 4);
                    *((char *)&damageColorMp + 1) =
                        (0x1e - *(int *)(*(int *)((int)D_8008D2AC->drivers + iTableOfs) + 0x4ac)) * 4;
                    *((char *)&damageColorMp + 2) =
                        (0x1e - *(int *)(*(int *)((int)D_8008D2AC->drivers + iTableOfs) + 0x4ac)) * 4;
                    iMpScratch = *(int *)((int)D_8008D2AC->drivers + iTableOfs);
                    *(int *)(iMpScratch + 0x4ac) = *(int *)(iMpScratch + 0x4ac) + -1;
                } else if (iMpScratch < 0) {
                    *(char *)&damageColorMp =
                        (iMpScratch + 0x1e) * 4;
                    *((char *)&damageColorMp + 1) =
                        ~((*(int *)(*(int *)((int)D_8008D2AC->drivers + iTableOfs) + 0x4ac) + 0x1e) * 4);
                    *((char *)&damageColorMp + 2) =
                        (*(int *)(*(int *)((int)D_8008D2AC->drivers + iTableOfs) + 0x4ac) + 0x1e) * 4;
                    iMpScratch = *(int *)((int)D_8008D2AC->drivers + iTableOfs);
                    *(int *)(iMpScratch + 0x4ac) = *(int *)(iMpScratch + 0x4ac) + 1;
                } else {
                    damageColorMp = 0x808080;
                }
                iconPos = damageColorMp;
                iMpScratch = *(int *)((int)D_8008D2AC->drivers + iTableOfs);
                prevBarX = *pRankAnimCursor;
                if ((*(char *)(iMpScratch + 0x44) == 0) &&
                    ((*(unsigned int *)(iMpScratch + 0x2c8) & ACTION_BEHIND_START) != 0)) {
                    targetBarX = 0;
                } else {
                    uTrackScratch =
                        (unsigned int)(unsigned short)
                            D_8008D2AC->level1->ptr_restart_points->distToFinish * 8;
                    iMpScratch = uTrackScratch -
                                 *(int *)(*(int *)((int)D_8008D2AC->drivers + iTableOfs) + 0x488);
                    uTrackScratch = (int)uTrackScratch / 0x1d1;
                    iEaseScratch = iMpScratch / (int)uTrackScratch;
                    targetBarX = (unsigned short)iEaseScratch;
                    if (iEaseScratch << 0x10 < 0) {
                        targetBarX = 0;
                    }
                }
                iEaseScratch = (int)(short)targetBarX;
                iMpScratch = iEaseScratch - (short)prevBarX;
                if (iMpScratch < 0) {
                    iMpScratch = -iMpScratch;
                }
                uTrackScratch = iMpScratch / 0xe;
                if ((uTrackScratch << 0x10) == 0) {
                    uTrackScratch = 1;
                }
                iMpScratch = prevBarX + uTrackScratch;
                easedBarX = (unsigned short)iMpScratch;
                if ((short)prevBarX < iEaseScratch) {
                    bStandingCmp = iEaseScratch < (iMpScratch << 0x10 >> 0x10);
                } else {
                    iMpScratch = prevBarX - uTrackScratch;
                    easedBarX = (unsigned short)iMpScratch;
                    bStandingCmp = (iMpScratch << 0x10 >> 0x10) < iEaseScratch;
                }
                if (bStandingCmp) {
                    easedBarX = targetBarX;
                }
                iMpScratch = (int)(short)targetBarX - (int)(short)easedBarX;
                if (iMpScratch < 0) {
                    iMpScratch = -iMpScratch;
                }
                if (400 < iMpScratch) {
                    easedBarX = targetBarX;
                }
                pRankTableCursor = (short *)((int)D_80086E84 + iRankScratch);
                iRankScratch = iRankScratch + 2;
                iTableOfs = iTableOfs + 4;
                func_80023054(D_8008D2AC->ptrIcons[(short)D_80086D84[*pRankTableCursor].iconID],
                              easedBarX + 5, 0x66, &D_8008D2AC->backBuffer->primMem,
                              D_8008D2AC->pushBuffer_UI.ptrOT, damageColorMp, damageColorMp,
                              damageColorMp, damageColorMp, 1, 0x9d8);
                gtCursor = D_8008D2AC;
                iDriverIdx = iDriverIdx + 1;
                *pRankAnimCursor = easedBarX;
                pRankAnimCursor = pRankAnimCursor + 1;
            } while (iDriverIdx <
                     (int)((unsigned int)(unsigned char)gtCursor->numPlyrCurrGame +
                           (unsigned int)(unsigned char)gtCursor->numBotsNextGame));
        }
        for (pWarpballThread = D_8008D2AC->threadBucket6Thread; pWarpballThread != 0;
             pWarpballThread = pWarpballThread->siblingThread) {
            pWarpballInst = pWarpballThread->inst;
            if (pWarpballInst->model->id == DYNAMIC_WARPBALL) {
                pRestartNode = D_8008D2AC->level1->ptr_restart_points;
                iDriverIdx = 0;
                if ((D_8008D2AC->level1->cnt_restart_points - 1U < 0xff) &&
                    (iTableOfs = (int)(short)(((int)((char *)((struct TrackerWeapon *)
                                                              pWarpballInst->thread->object)
                                                     ->ptrNodeCurr) -
                                               (int)pRestartNode) * -0x55555555 >> 2),
                     -1 < iTableOfs)) {
                    pNextNode = pRestartNode + (unsigned char)pRestartNode[iTableOfs].nextIndex_forward;
                    warpX = (short)pWarpballInst->matrix.t[0];
                    warpY = (short)pWarpballInst->matrix.t[1];
                    warpZ = (short)pWarpballInst->matrix.t[2];
                    pRestartNode = pRestartNode + (unsigned char)pNextNode->nextIndex_forward;
                    pathDir.vy = pNextNode->pos[1] - pRestartNode->pos[1];
                    pathDir.vx = pNextNode->pos[0] - pRestartNode->pos[0];
                    pathDir.vz = pNextNode->pos[2] - pRestartNode->pos[2];
                    func_8003D378(&pathDir);
                    relXY = (int)(((warpY - pNextNode->pos[1]) << 0x10) |
                                  ((warpX - pNextNode->pos[0]) & 0xffff));
                    relZ = warpZ - pNextNode->pos[2];
                    gte_ldR11R12(*(int *)&pathDir);
                    gte_ldR13R21((int)pathDir.vz);
                    gte_ldVXY0(relXY);
                    gte_ldVZ0((int)relZ);
                    gte_mvmva();
                    gte_stMAC1(iDriverIdx);
                    uTrackScratch = (unsigned int)(unsigned short)
                                        D_8008D2AC->level1->ptr_restart_points->distToFinish;
                    iRankScratch = (unsigned int)(unsigned short)pNextNode->distToFinish * 8 +
                                   (iDriverIdx >> 0xc);
                    iTableOfs = uTrackScratch << 3;
                    iDriverIdx = iRankScratch % iTableOfs;
                }
                if (iDriverIdx != 0) {
                    uTrackLen = (unsigned int)(unsigned short)
                                    D_8008D2AC->level1->ptr_restart_points->distToFinish * 8;
                    uTrackScratch = uTrackLen / 0x1d1;
                    func_80022EC4(D_8008D2AC->ptrIcons[0xe],
                                  (short)((uTrackLen - iDriverIdx) / (int)uTrackScratch) + 5, 0x66,
                                  &D_8008D2AC->backBuffer->primMem,
                                  D_8008D2AC->pushBuffer_UI.ptrOT, 1, 0x8aa, 1);
                }
            }
        }
    }
    return;
}
