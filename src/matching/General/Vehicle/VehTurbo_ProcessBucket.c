/*
 * VehTurbo_ProcessBucket @ 0x80069284 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-EXACT match: base score 1140, Branch Differences 0, Stack Differences 0
 * (Register Differences 48). Delivered NATURAL — the reseeded score-85 "near completed"
 * snapshot was a FALSE-FRIEND: it drove the `primary` (turboThread->inst) destination
 * cursor through an UNINITIALIZED local (`new_var7`) for the instFlags/otRange/depthOffset
 * stores and only used the real node->0x34 pointer for the last (depthOffset[1]) store,
 * relying on gcc happening to register-alias the garbage local to retail's $a3 cursor.
 * That is undefined behavior, not a match — rejected per the no-hacky-match mandate.
 *
 * The honest residual is a genuine gcc-2.8.1 scheduling/reg-alloc wall: retail interleaves
 * the numPlyr load into the middle of the source-address chain and colors the three idpp
 * cursors differently than gcc-from-source; a 170-iter grind only reached 1000 and only via
 * a barred `volatile` return-type hack + caching the per-iteration numPlyr reload, both
 * rejected. Control flow / stack / behavior are identical to retail.
 *
 * For each turbo thread in the sibling list, copies the driver's per-player draw state
 * (instFlags masked by ~DRAW_SUCCESSFUL, OT ranges, depth offsets) into the turbo's two
 * instances' per-player draw records, for every active split-screen player, but only while
 * the driver's pushbuffer does not yet exist (instFlags & PUSHBUFFER_EXISTS == 0).
 *
 * saphi VehTurbo_ProcessBucket (game/Vehicle/VehTurbo.c) audited CLEAN vs this asm + Ghidra.
 */

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;

extern char *D_8008D2AC;   /* sdata->gGT */

/* Per-player draw-record (InstDrawPerPlayer) field offsets, stride 0x88:
 *   0xB8 instFlags   0xE4 otRangeNormal   0xE8 otRangeSecondary
 *   0xDC depthOffset[0]   0xDE depthOffset[1]
 * PUSHBUFFER_EXISTS = 0x100, ~DRAW_SUCCESSFUL = 0xFFFFFFBF (~0x40). */

void VehTurbo_ProcessBucket(int turboThread)
{
    char *gGT;
    char *gGTReload;
    int driver;      /* source cursor: turbo->driver->instSelf idpp   (asm $a1) */
    int secondary;   /* dest cursor:   turbo->inst idpp                (asm $a2) */
    int primary;     /* dest cursor:   turboThread->inst idpp          (asm $a3) */
    int *turbo;
    int i;
    uint driverFlags;
    int otRange;
    ushort depth;

    if (turboThread != 0)
    {
        gGT = D_8008D2AC;
        do
        {
            turbo = *((int **) (turboThread + 0x30));           /* turboThread->object */
            primary = *((int *) (turboThread + 0x34));          /* turboThread->inst  (idpp) */
            secondary = turbo[0];                               /* turbo->inst        (idpp) */
            driver = *((int *) (turbo[1] + 0x1c));              /* turbo->driver->instSelf (idpp) */
            if (gGT[0x1ca8] != 0)                               /* gGT->numPlyrCurrGame */
            {
                gGTReload = D_8008D2AC;
                i = 0;
                do
                {
                    driverFlags = *((uint *) (driver + 0xb8));
                    if ((driverFlags & 0x100) == 0)             /* !(instFlags & PUSHBUFFER_EXISTS) */
                    {
                        *((uint *) (secondary + 0xb8)) &= driverFlags | 0xffffffbf;
                        *((uint *) (primary + 0xb8)) &= *((uint *) (driver + 0xb8)) | 0xffffffbf;
                        otRange = *((int *) (driver + 0xe4));
                        *((int *) (primary + 0xe4)) = otRange;
                        *((int *) (secondary + 0xe4)) = otRange;
                        otRange = *((int *) (driver + 0xe8));
                        *((int *) (primary + 0xe8)) = otRange;
                        *((int *) (secondary + 0xe8)) = otRange;
                        depth = *((ushort *) (driver + 0xdc));
                        *((ushort *) (primary + 0xdc)) = depth;
                        *((ushort *) (secondary + 0xdc)) = depth;
                        depth = *((ushort *) (driver + 0xde));
                        *((ushort *) (primary + 0xde)) = depth;
                        *((ushort *) (secondary + 0xde)) = depth;
                    }
                    primary += 0x88;
                    secondary += 0x88;
                    i++;
                    driver += 0x88;
                } while (i < (int)(byte) gGTReload[0x1ca8]);
            }
            turboThread = *((int *) (turboThread + 0x10));      /* turboThread->siblingThread */
        } while (turboThread != 0);
    }
    return;
}
