/*
 * VehStuckProc_MaskGrab_FindDestPos @ 0x8006677C (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 29170, Branch 0 / Stack 0, Register 202.
 * The large residual is a genuine gcc-2.8.1 register-allocation/scheduling wall on
 * this scratchpad-heavy + call-heavy 196-line routine — byte-exact would need a
 * dedicated hand-match, but the behavior and every offset/constant are verified
 * correct vs the retail asm + Ghidra + saphi (game/Vehicle/VehStuckProc.c:43, CLEAN).
 * From the aibox staging set.
 *
 * NOTE on `volatile`: every DAT_1f800xxx below is a PS1 SCRATCHPAD (0x1F800000 region)
 * memory-mapped accessor. `volatile` is REQUIRED and CORRECT here (prevents the compiler
 * eliding/reordering the scratchpad reads/writes) — it is NOT a codegen hack. Retail uses
 * the scratchpad as the search-parameter struct (`sps`): the DAT_1f800108.. block is the
 * sps that gets passed to COLL_SearchBSP_CallbackPARAM. This is scratchpad-as-DATA (gcc-
 * matchable), not the hand-asm scratchpad-register-spill pattern.
 *
 * Behavior: if there is no valid restart-point list (or this quad has no checkpoint),
 * place posCurr at the midpoint of quad verts 0 and 3. Otherwise walk the restart-point
 * ring from this quad's checkpoint, projecting each candidate onto the ground via a BSP
 * quadblock search (staged through the scratchpad sps), and stop at the first candidate
 * that touches a ground quadblock, is not on a kill-plane, and is >= 0x2000 (in X or Z)
 * from every other player. Finally set cameraDC[driverID].flags |= 1.
 */

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef unsigned short undefined2;
typedef unsigned int undefined4;

#define gGT (*(volatile unsigned char**)0x8008d2ac)          /* sdata->gGT */

/* --- scratchpad search-parameter struct (sps), based at 0x1F800108 --- */
#define sps_Input1_hitRadius   (*(volatile unsigned short*)0x1f80010e)  /* sps+0x06 */
#define sps_probe_x            (*(volatile short*)0x1f800108)           /* sps+0x00 Input1.pos.x */
#define sps_probe_y            (*(volatile short*)0x1f80010a)           /* sps+0x02 Input1.pos.y */
#define sps_probe_z            (*(volatile unsigned short*)0x1f80010c)  /* sps+0x04 Input1.pos.z */
#define sps_bboxMinX           (*(volatile short*)0x1f800138)           /* sps+0x30 */
#define sps_bboxMinY           (*(volatile unsigned short*)0x1f80013a)  /* sps+0x32 */
#define sps_bboxMinZ           (*(volatile unsigned short*)0x1f80013c)  /* sps+0x34 */
#define sps_bboxMaxX           (*(volatile short*)0x1f80013e)           /* sps+0x36 */
#define sps_bboxMaxY           (*(volatile unsigned short*)0x1f800140)  /* sps+0x38 */
#define sps_bboxMaxZ           (*(volatile unsigned short*)0x1f800142)  /* sps+0x3a */
#define sps_hitPosX            (*(volatile short*)0x1f800124)           /* sps+0x1c QuadBlockColl.hitPos.x */
#define sps_hitPosY            (*(volatile short*)0x1f800126)           /* sps+0x1e */
#define sps_searchFlags        (*(volatile unsigned short*)0x1f80012a)  /* sps+0x22 */
#define sps_hitPosZ            (*(volatile unsigned short*)0x1f800128)  /* sps+0x20 */
#define sps_hitRadiusSq        (*(volatile unsigned int*)0x1f800110)    /* sps+0x08 Input1.hitRadiusSquared */
#define sps_qbc_hitRadius      (*(volatile unsigned short*)0x1f80011c)  /* sps+0x16 QuadBlockColl.hitRadius (packed w/ pos.x) */
#define sps_qbc_hitRadius_hi   (*(volatile unsigned short*)0x1f80011e)  /* sps+0x16 hi half */
#define sps_qbc_hitRadiusSq    (*(volatile unsigned int*)0x1f800120)    /* sps+0x18 */
#define sps_ptr_mesh_info      (*(volatile unsigned int*)0x1f800134)    /* sps+0x2c */
#define sps_quadFlagsIgnored   (*(volatile unsigned int*)0x1f800130)    /* sps+0x28 */
#define sps_quadFlagsWanted    (*(volatile unsigned int*)0x1f80012c)    /* sps+0x24 */
#define sps_numBspHitboxesHit  (*(volatile unsigned int*)0x1f800146)    /* sps+0x3e boolDidTouchQuadblock */
#define sps_stepFlagsClr       (*(volatile unsigned int*)0x1f800144)    /* sps+0x3c */
#define sps_hitFraction        (*(volatile unsigned int*)0x1f80018c)    /* sps+0x84 */
#define sps_stepFlags          (*(volatile unsigned int*)0x1f8002ac)    /* sps+0x1a4 */
#define sps_qbc_pos            (*(volatile int*)0x1f800118)             /* sps+0x10 QuadBlockColl.pos (packed x|y) */
#define sps_qbc_pos_x          (*(volatile short*)0x1f800118)           /* sps+0x10 lo */
#define sps_qbc_pos_y          (*(volatile unsigned short*)0x1f80011a)  /* sps+0x12 hi */
#define sps_qbc_posZ           (*(volatile int*)0x1f80011c)             /* sps+0x14 (packed) */
#define sps_qbc_posZ_lo        (*(volatile unsigned short*)0x1f80011c)  /* sps+0x14 lo */

extern long ratan2(int, int);
extern void COLL_SearchBSP_CallbackPARAM(unsigned int bspRoot, void *bbox, void *leafCallback, void *sps);

void VehStuckProc_MaskGrab_FindDestPos(int driver, short *quad)
{
  byte checkpointIndex;
  short respawnPosZ;
  long angleY;
  int diffX;
  int cursor;      /* reused: vertArray ptr / restart_points ptr / player-thread ptr / next-thread */
  int otherDriver;
  short *respawn;
  short *nextRespawn;

  if (((*(int *)(*(int *)(gGT + 0x160) + 0x148) < 1) ||
       (*(int *)(*(int *)(gGT + 0x160) + 0x14c) == 0)) ||
      (*(char *)(quad + 0x1f) == -1))
  {
    /* No restart points / no checkpoint: place at midpoint of quad verts 0 and 3. */
    *(int *)(driver + 0x2d4) =
      ((int)*(short *)((int)*quad * 0x10 + *(int *)(**(int **)(gGT + 0x160) + 0x10)) +
       (int)*(short *)((int)quad[3] * 0x10 + *(int *)(**(int **)(gGT + 0x160) + 0x10))) * 0x80;

    cursor = *(int *)(**(int **)(gGT + 0x160) + 0x10);
    *(int *)(driver + 0x2d8) =
      ((int)*(short *)((int)*quad * 0x10 + cursor + 2) +
       (int)*(short *)((int)quad[3] * 0x10 + cursor + 2) + 0x80) * 0x80;

    cursor = *(int *)(**(int **)(gGT + 0x160) + 0x10);
    *(int *)(driver + 0x2dc) =
      ((int)*(short *)((int)*quad * 0x10 + cursor + 4) +
       (int)*(short *)((int)quad[3] * 0x10 + cursor + 4)) * 0x80;
  }
  else
  {
    /* Set up the scratchpad search-parameter struct from this driver's thread. */
    sps_Input1_hitRadius = *(undefined2 *)(*(int *)(*(int *)(driver + 0x1c) + 0x6c) + 0x42);
    sps_hitRadiusSq      = *(undefined4 *)(*(int *)(*(int *)(driver + 0x1c) + 0x6c) + 0x38);
    sps_qbc_hitRadius_hi = *(undefined2 *)(*(int *)(*(int *)(driver + 0x1c) + 0x6c) + 0x42);
    sps_qbc_hitRadiusSq  = *(undefined4 *)(*(int *)(*(int *)(driver + 0x1c) + 0x6c) + 0x38);

    sps_ptr_mesh_info    = **(unsigned int **)(gGT + 0x160);
    sps_quadFlagsIgnored = 0x4010;   /* NO_CAMERA_RESPAWN_PROBE | NO_COLLISION_RESPONSE */
    sps_quadFlagsWanted  = 0x1000;   /* GROUND */

    checkpointIndex = *(byte *)(quad + 0x1f);
    cursor = *(int *)(*(int *)(gGT + 0x160) + 0x14c);          /* restart_points */
    *(undefined4 *)(driver + 0x490) = 0;                      /* distanceDrivenBackwards */
    respawn = (short *)(cursor + (uint)checkpointIndex * 0xc);

    do {
      do {
        nextRespawn = (short *)(*(int *)(*(int *)(gGT + 0x160) + 0x14c) +
                                (uint)*(byte *)(respawn + 4) * 0xc);

        *(int *)(driver + 0x2d4) = (int)*respawn << 8;
        *(int *)(driver + 0x2d8) = ((int)respawn[1] + 0x80) * 0x100;

        respawnPosZ = respawn[2];
        *(undefined2 *)(driver + 0x2ec) = 0;                  /* rotCurr.x */
        *(int *)(driver + 0x2dc) = (int)respawnPosZ << 8;

        angleY = ratan2((int)*nextRespawn - (int)*respawn, (int)nextRespawn[2] - (int)respawn[2]);
        *(undefined2 *)(driver + 0x2ee) = (short)angleY;      /* rotCurr.y */
        *(undefined2 *)(driver + 0x2f0) = 0;                  /* rotCurr.z */

        /* stage probe point + bbox into the scratchpad sps */
        sps_qbc_pos_x = (short)((uint)*(undefined4 *)(driver + 0x2d4) >> 8);
        sps_qbc_pos_y = (ushort)((uint)*(undefined4 *)(driver + 0x2d8) >> 8);
        sps_qbc_posZ_lo = (ushort)((uint)*(undefined4 *)(driver + 0x2dc) >> 8);
        sps_probe_x = (short)((uint)*(undefined4 *)(driver + 0x2d4) >> 8);
        sps_probe_y = (short)((uint)*(undefined4 *)(driver + 0x2d8) >> 8) - 0x100;

        sps_searchFlags = 0;
        sps_probe_z = (ushort)((uint)*(undefined4 *)(driver + 0x2dc) >> 8);

        if ((byte)gGT[0x1ca8] < 3) {          /* numPlyrCurrGame < 3 -> HIGH_LOD */
          sps_searchFlags = 2;
        }

        sps_numBspHitboxesHit = 0;
        sps_stepFlagsClr = 0;
        sps_hitFraction = 0x1000;
        sps_stepFlags = 0;

        sps_bboxMinX = (short)sps_qbc_pos;
        if (sps_probe_x < (short)sps_qbc_pos) {
          sps_bboxMinX = sps_probe_x;
        }
        sps_bboxMinY = sps_qbc_pos_y;
        if ((int)((uint)sps_probe_y << 0x10) < (int)((uint)sps_qbc_pos_y << 0x10)) {
          sps_bboxMinY = sps_probe_y;
        }
        sps_bboxMinZ = (ushort)sps_qbc_posZ;
        if ((int)((uint)sps_probe_z << 0x10) < (int)((uint)(ushort)sps_qbc_posZ << 0x10)) {
          sps_bboxMinZ = sps_probe_z;
        }
        sps_bboxMaxX = (short)sps_qbc_pos;
        if ((short)sps_qbc_pos < sps_probe_x) {
          sps_bboxMaxX = sps_probe_x;
        }
        sps_bboxMaxY = sps_qbc_pos_y;
        if ((int)((uint)sps_qbc_pos_y << 0x10) < (int)((uint)sps_probe_y << 0x10)) {
          sps_bboxMaxY = sps_probe_y;
        }
        sps_bboxMaxZ = (ushort)sps_qbc_posZ;
        if ((int)((uint)(ushort)sps_qbc_posZ << 0x10) < (int)((uint)sps_probe_z << 0x10)) {
          sps_bboxMaxZ = sps_probe_z;
        }

        sps_hitPosX = sps_probe_x;
        sps_hitPosY = sps_probe_y;
        sps_hitPosZ = sps_probe_z;

        COLL_SearchBSP_CallbackPARAM(
          *(unsigned int *)(sps_ptr_mesh_info + 0x18),        /* bspRoot */
          (void *)&sps_bboxMinX,
          (void *)0x8001f5f0,                                 /* COLL_FIXED_BSPLEAF_TestQuadblocks */
          (void *)&sps_probe_x);                              /* sps */

        respawn = nextRespawn;

      } while ((sps_numBspHitboxesHit == 0) || ((sps_stepFlags & 0x4000) != 0));   /* !touched || KILL_PLANE */

      cursor = *(int *)(gGT + 0x1b2c);        /* threadBuckets[PLAYER].thread */

      if (cursor == 0) break;

      do {
        otherDriver = *(int *)(cursor + 0x30);

        if (otherDriver != driver) {
          diffX = *(int *)(driver + 0x2d4) - *(int *)(otherDriver + 0x2d4);
          if (diffX < 0) diffX = -diffX;
          if (diffX < 0x2000) break;

          otherDriver = *(int *)(driver + 0x2dc) - *(int *)(otherDriver + 0x2dc);
          if (otherDriver < 0) otherDriver = -otherDriver;
          if (otherDriver < 0x2000) break;
        }

        cursor = *(int *)(cursor + 0x10);     /* siblingThread */

      } while (cursor != 0);

    } while (cursor != 0);
  }

  *(uint *)(gGT + (uint)*(byte *)(driver + 0x4a) * 0xdc + 0x1508) =    /* cameraDC[driverID].flags */
    *(uint *)(gGT + (uint)*(byte *)(driver + 0x4a) * 0xdc + 0x1508) | 1;
  return;
}
