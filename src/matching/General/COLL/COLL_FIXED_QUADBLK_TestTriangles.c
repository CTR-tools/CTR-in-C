/*
 * COLL_FIXED_QUADBLK_TestTriangles @ 0x8001F41C (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 12840, Branch Differences 0, Stack Differences 0,
 * Register Differences 96 — a genuine gcc-2.8.1 register-allocation wall. Retail swaps
 * the two args (quadBlock -> $t9, sps -> $a0) and keeps quadBlock live in the caller-
 * saved $t9 across the COLL_FIXED_TRIANGL_TestPoint (func_8001EF50) calls; natural C
 * from a `(quadBlock, sps)` signature keeps it in a callee-saved reg instead, which
 * cascades into the register allocation of the eight call-argument setups. A 150-iter
 * -j4 grind only got below 12840 via a barred `int *p = &sps;` pointer-to-local hack
 * (rejected per the no-hacky-match mandate). This is the NATURAL clean form.
 * Finished from the "aibox" prior grind set.
 *
 * Behavior (verified vs asm + Ghidra): store quadBlock at sps+0x64; a 7-condition AABB
 * overlap test between quadBlock's bbox (hi16 at +0x2c/+0x30/+0x34) and sps's search
 * bbox (+0x30..+0x3a), gated on flag-mask agreement (sps+0x24 must intersect, sps+0x28
 * must not, quadFlags at quadBlock+0x12). On overlap, sps+0x22 & 2 selects the two-sided
 * quad path (optional setup func_8001F6F0 unless sps+0x22 & 8, then 4 or 8 triangle
 * tests) vs the one-sided path (func_8001F67C setup, then 1 or 2 triangle tests); the
 * second triangle set runs only when the quad is non-degenerate (quadBlock+4 hi16 != lo16).
 *
 * saphi COLL_FIXED_QUADBLK_TestTriangles audited CLEAN vs this asm + Ghidra.
 */
typedef unsigned int uint;
typedef unsigned short ushort;

extern void func_8001EF50(int sps, int vertA, int vertB, int normalOut); /* COLL_FIXED_TRIANGL_TestPoint */
extern void func_8001F67C(int sps, int quadBlock);                       /* one-sided quad setup */
extern void func_8001F6F0(int sps, int quadBlock);                       /* two-sided quad setup  */

void COLL_FIXED_QUADBLK_TestTriangles(int quadBlock, int sps)
{
  *(int *)(sps + 100) = quadBlock;

  if (((*(uint *)(sps + 0x24) & (int)*(short *)(quadBlock + 0x12)) != 0) &&
      ((*(uint *)(sps + 0x28) & (int)*(short *)(quadBlock + 0x12)) == 0) &&
      ((*(int *)(quadBlock + 0x2c) >> 0x10) - (int)*(short *)(sps + 0x38) < 1) &&
      ((int)(short)*(int *)(quadBlock + 0x2c) - (int)*(short *)(sps + 0x36) < 1) &&
      ((int)*(short *)(sps + 0x30) - (*(int *)(quadBlock + 0x30) >> 0x10) < 1) &&
      ((int)(short)*(int *)(quadBlock + 0x30) - (int)*(short *)(sps + 0x3a) < 1) &&
      ((int)*(short *)(sps + 0x34) - (*(int *)(quadBlock + 0x34) >> 0x10) < 1))
  {
    if ((int)*(short *)(sps + 0x32) - (int)(short)*(int *)(quadBlock + 0x34) < 1)
    {
      if ((*(ushort *)(sps + 0x22) & 2) != 0)
      {
        if ((*(ushort *)(sps + 0x22) & 8) == 0)
        {
          func_8001F6F0(sps, quadBlock);
        }

        func_8001EF50(sps, sps + 0xf0, sps + 0x140, sps + 0x154);
        func_8001EF50(sps, sps + 0x140, sps + 0x168, sps + 0x154);
        func_8001EF50(sps, sps + 0x168, sps + 0x140, sps + 0x104);
        func_8001EF50(sps, sps + 0x154, sps + 0x168, sps + 0x118);
        if (*(uint *)(quadBlock + 4) >> 0x10 != (*(uint *)(quadBlock + 4) & 0xffff))
        {
          func_8001EF50(sps, sps + 0x190, sps + 0x168, sps + 0x17c);
          func_8001EF50(sps, sps + 0x17c, sps + 0x12c, sps + 0x190);
          func_8001EF50(sps, sps + 0x104, sps + 0x17c, sps + 0x168);
          func_8001EF50(sps, sps + 0x118, sps + 0x168, sps + 0x190);
          return;
        }
      }
      else
      {
        func_8001F67C(sps, quadBlock);

        func_8001EF50(sps, sps + 0xf0, sps + 0x104, sps + 0x118);
        if (*(uint *)(quadBlock + 4) >> 0x10 != (*(uint *)(quadBlock + 4) & 0xffff))
        {
          func_8001EF50(sps, sps + 0x104, sps + 0x12c, sps + 0x118);
        }
      }
    }
  }
}
