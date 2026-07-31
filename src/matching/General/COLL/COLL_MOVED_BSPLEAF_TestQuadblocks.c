/*
 * COLL_MOVED_BSPLEAF_TestQuadblocks @ 0x800202A8 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: score 1075, Branch Differences 0, Stack Differences 0.
 * The residual is a genuine gcc-2.8.1 register-allocation wall: retail keeps the
 * advancing quadblock cursor in the caller-saved temp $t9 ACROSS the
 * QUADBLK_TestTriangles call (relying on the callee to preserve it), so it needs
 * only 3 callee-saved registers. Natural C cannot hold a value in a caller-saved
 * register across a call, so gcc allocates a 4th saved register ($s3) and a larger
 * frame. Forcing retail's layout would require a `register ... asm("$t9")` pin,
 * barred by the no-hacky-match rule; a ~170s permuter grind never beat this without
 * a reuse/pin hack. Control flow, stack, and behavior are identical to retail.
 *
 * Tests every quadblock under a BSP leaf against MOVED (dynamic) collision geometry
 * (sibling of COLL_FIXED_BSPLEAF_TestQuadblocks, decomp COLL_12): if the leaf's
 * water flag (bit 1) is set, OR 0x8000 into the scratchpad output word (+0x1A4);
 * iterate node->numQuads quadblocks (stride 0x5C) through QUADBLK_TestTriangles;
 * then if the scratchpad searchFlags (+0x22) low bit is set, call BSPLEAF_TestInstance.
 *
 * NOTE: the reseeded "near completed" snapshot for this function was a FALSE-FRIEND
 * (score 295 but behaviorally WRONG) — it called TestTriangles with the constant
 * initial cursor instead of the per-iteration advancing one, and read searchFlags
 * from +0x5C instead of +0x22. This version is rebuilt correct against the retail asm.
 *
 * saphi COLL_MOVED_BSPLEAF_TestQuadblocks (game/COLL.c:2117) audited CLEAN vs this asm.
 * Finished from the "near completed" prior grind set.
 */
typedef unsigned int uint;

extern void func_80020064(int quadblock, int sps);   /* COLL_MOVED_QUADBLK_TestTriangles */
extern void func_8001D610(uint *node, int sps);      /* COLL_MOVED_BSPLEAF_TestInstance  */

void COLL_MOVED_BSPLEAF_TestQuadblocks(uint *node, int sps)
{
    int numQuads;
    int quadblock;
    short searchFlags;

    if ((node[0] & 2) != 0)                          /* leaf water flag */
        *((uint *) (sps + 0x1A4)) |= 0x8000;

    quadblock = node[7];                             /* leaf.ptrQuadBlockArray */
    numQuads  = node[6];                             /* leaf.numQuads          */
    do
    {
        func_80020064(quadblock, sps);
        numQuads--;
        quadblock += 0x5C;                           /* sizeof(QuadBlock) */
    } while (numQuads > 0);

    searchFlags = *((short *) (sps + 0x22));
    if ((searchFlags & 1) != 0)
        func_8001D610(node, sps);
}
