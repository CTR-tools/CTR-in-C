/*
 * COLL_FIXED_BotsSearch @ 0x8001D77C (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 1275, Branch 0 / Stack 0, Register 55 — a genuine
 * gcc-2.8.1 register-allocation wall on this register-heavy setup routine.
 * Delivered NATURAL from the aibox staging set:
 *   - REJECTED the seed's `prevVal++; prevVal--;` no-op pair (barred codegen-steering
 *     hack; it forced the score to 305 but retail has no such addiu; not natural C).
 *   - REJECTED a permuter false-friend at score 505 that inlined `currVal - radius` into
 *     the compare while storing the UN-subtracted `currVal` — the retail `subu`+`addu`
 *     stores `pos - radius`, so the 505 form was behaviorally WRONG.
 *
 * Builds the search bounding box for a bot AABB query: writes radius² and the two
 * positions into the scratchpad, computes per-axis min(cur-r, prev-r) / max(cur+r, prev+r),
 * clears the result fields, and kicks off the BSP search with the TestInstance callback.
 * Offsets map to saphi struct ScratchpadStruct (game/COLL.c:602 COLL_FIXED_BotsSearch):
 *   [3]=Input1.hitRadius  [4]=Input1.hitRadiusSquared  [0..2]=Input1.pos
 *   [8..10]=Union.QuadBlockColl.pos(=posPrev)  [0xb]=hitRadius  [0xc]=hitRadiusSquared
 *   [0xe..0x10]=Union.QuadBlockColl.hitPos(=posCurr)  [0x16]=ptr_mesh_info
 *   [0x18..0x1a]=bbox.min  [0x1b..0x1d]=bbox.max  [0x42]=hitFraction(0x1000)
 *   [0x1e]=numTrianglesTested/boolDidTouchHitbox  [0x62]=numBspHitboxesHit  [0xd2]=collision.stepFlags
 *
 * saphi COLL_FIXED_BotsSearch audited CLEAN vs this asm + Ghidra.
 */

extern void func_8001D610(int node, int sps);                          /* COLL_FIXED_BSPLEAF_TestInstance */
extern void func_8001EBEC(int bspRoot, short *bbox,
                          void (*cb)(int, int), short *sps);           /* COLL_SearchBSP_CallbackPARAM */

void COLL_FIXED_BotsSearch(short *posCurr, short *posPrev, short *sps)
{
    int bspRoot;
    int lo;
    int cand;
    short *radius;

    *((int *) (sps + 4)) = sps[3] * sps[3];      /* hitRadiusSquared = hitRadius^2 */
    *sps = *posCurr;
    sps[1] = posCurr[1];
    sps[2] = posCurr[2];
    sps[0xf] = sps[1];
    sps[0xe] = *sps;
    radius = &sps[3];
    sps[0x10] = sps[2];
    sps[0xb] = *radius;
    *((int *) (sps + 0xc)) = *((int *) (sps + 4));
    sps[8] = *posPrev;
    sps[9] = posPrev[1];
    sps[10] = posPrev[2];

    /* per-axis lower bound: min(posPrev - r, posCurr - r) */
    bspRoot = (*sps) - (*radius);
    lo = sps[8] - sps[0xb];
    cand = bspRoot;
    if (cand < lo)
        lo = cand;
    sps[0x18] = lo;
    lo = sps[9] - sps[0xb];
    cand = sps[1];
    cand = cand - (*radius);
    if (cand < lo)
        lo = cand;
    sps[0x19] = lo;
    lo = sps[10] - (cand = sps[0xb]);
    cand = sps[2] - (*radius);
    if (cand < lo)
        lo = cand;
    sps[0x1a] = lo;

    /* per-axis upper bound: max(posPrev + r, posCurr + r) */
    lo = sps[0xb] + sps[8];
    cand = (*sps) + (*radius);
    if (lo < cand)
        lo = cand;
    sps[0x1b] = lo;
    lo = sps[0xb] + sps[9];
    cand = sps[1] + (*radius);
    if (lo < cand)
        lo = cand;
    sps[0x1c] = lo;
    lo = sps[0xb] + sps[10];
    cand = sps[2] + (*radius);
    if (lo < cand)
        lo = cand;

    *((int *) (sps + 0x42)) = 0x1000;            /* hitFraction = COLL_FRACTION_ONE */
    bspRoot = (*((int *) (sps + 0x16))) + 0x18;  /* &ptr_mesh_info->bspRoot */
    sps[0x1d] = lo;
    sps[0x21] = 0;
    sps[0x1e] = 0;
    *((int *) (sps + 0xd2)) = 0;                 /* collision.stepFlags = 0 */
    *((int *) (sps + 0x62)) = 0;                 /* numBspHitboxesHit = 0 */
    func_8001EBEC(*((int *) bspRoot), sps + 0x18, func_8001D610, sps);
}
