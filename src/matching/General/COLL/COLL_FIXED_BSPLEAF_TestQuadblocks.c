/*
 * COLL_FIXED_BSPLEAF_TestQuadblocks @ 0x8001F5F0 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-EXACT match: Branch Differences 0, Stack Differences 0 (correct control
 * flow + stack). Register residual only — base score 1185 (Reg 37). This is a genuine
 * gcc-2.8.1-vs-retail register-allocation wall, NOT a byte-exact match:
 *   Retail keeps `ptrQuad` in the caller-saved temporary $t9 live ACROSS the
 *   jal to COLL_FIXED_QUADBLK_TestTriangles (t9 is loaded, passed, and post-incremented
 *   around the call). That is only safe because the callee happens to preserve $t9;
 *   gcc-2.8.1 cannot assume that, so it allocates a 4th callee-saved register (s3)
 *   for ptrQuad — adding an extra save/restore + a larger frame and shifting the whole
 *   register assignment. No natural C removes this; it is a compiler-allocation
 *   difference, not a code difference.
 *
 * The "near completed" reseeded snapshot scored 295 but was a FALSE-FRIEND (read an
 * uninitialised pointer, do/while(0) contortions) — it only matched more instructions
 * by dropping a live variable, i.e. by being wrong. Per the correctness-over-score
 * mandate it was discarded and this correct, natural, behavior-exact version delivered.
 * (The permuter's own lower-scoring candidates hoist the TestTriangles call out of the
 * loop — also rejected as behaviorally wrong.)
 *
 * Behavior: a BSP-leaf collision test over FIXED (static level) geometry —
 *   - if the leaf's water flag (BSP_LEAF_FLAG_WATER=2) is set, OR the water bit
 *     (COLL_STEP_FLAG_WATER_BSP=0x8000) into sps->collision.stepFlags (sps+0x1A4);
 *   - iterate the leaf's quadblock array (numQuads @node+0x18, ptrQuad @node+0x1C,
 *     stride 0x5C), calling COLL_FIXED_QUADBLK_TestTriangles(quad, sps) once per quad
 *     (do-while: runs >=1 time, signed >0 test — matches retail bgtz);
 *   - if sps->QuadBlockColl.searchFlags (sps+0x22, COLL_SEARCH_TEST_INSTANCES=1) is set,
 *     call COLL_FIXED_BSPLEAF_TestInstance(node, sps).
 *
 * saphi COLL_FIXED_BSPLEAF_TestQuadblocks (game/COLL.c) audited CLEAN vs this asm +
 * Ghidra: offsets 0x1A4/0x22 and constants 0x2/0x8000/0x1 all confirmed by
 * CTR_STATIC_ASSERT; do-while structure identical. No saphi bug.
 */

typedef unsigned int uint;

extern void COLL_FIXED_QUADBLK_TestTriangles(int quadBlock, int sps);  /* func_8001F41C */
extern void COLL_FIXED_BSPLEAF_TestInstance(uint *node, int sps);      /* func_8001D610 */

void COLL_FIXED_BSPLEAF_TestQuadblocks(uint *node, int sps)
{
    int numQuads;
    int quadBlock;

    if ((node[0] & 2) != 0)                       /* BSP_LEAF_FLAG_WATER */
        *((uint *)(sps + 0x1a4)) |= 0x8000;       /* stepFlags |= COLL_STEP_FLAG_WATER_BSP */

    numQuads  = node[6];                           /* node->data.leaf.numQuads (0x18) */
    quadBlock = node[7];                           /* node->data.leaf.ptrQuadBlockArray (0x1C) */

    do {
        COLL_FIXED_QUADBLK_TestTriangles(quadBlock, sps);
        quadBlock += 0x5c;                        /* ptrQuad++ (QuadBlock stride 0x5C) */
    } while (--numQuads > 0);

    if ((*((short *)(sps + 0x22)) & 1) != 0)      /* searchFlags & COLL_SEARCH_TEST_INSTANCES */
        COLL_FIXED_BSPLEAF_TestInstance(node, sps);
}
