/*
 * INSTANCE_BirthWithThread @ 0x800309A4 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 230, Branch 0 / Stack 0, Register 6 residual
 * (a gcc-2.8.1 register-allocation / shift-scheduling wall — retail keeps the two
 * per-branch `sll ,16` in distinct registers with the case-B shift in a jump delay
 * slot; gcc-from-source either hoists the shift or splits it, neither of which the
 * permuter can bridge). vs SCUS_944.26.exe; finished from the "near completed" set.
 *
 * ** BUG FOUND & FIXED in the prior "near completed" reference **: the reseeded
 * score-140 snapshot shifted the size field LEFT BY 16 TWICE ((x<<12)<<4 then
 * *0x10000), i.e. << 32 == 0, so the object size was silently dropped from the
 * pool-allocation flags. That lower score was pure instruction-similarity, not
 * correctness. This version does a single << 16 per branch (verified by disasm).
 * saphi's equivalent is NOT affected (see below) — this was a matching-only artifact.
 *
 * Looks up gGT->modelPtr[modelID] (gGT+0x2160); returns 0 if absent. Rounds objSize
 * up to a multiple of 4, packs (poolType | (size<<16) | bucket) as the PROC creation
 * flags, spawns the thread (func_8004205C), records modelID at thread+0x44, births
 * the 3D instance (func_8003086C), and stores it at thread+0x34.
 *
 * saphi INSTANCE_BirthWithThread audited CLEAN vs this asm + Ghidra: its
 * SIZE_RELATIVE_POOL_BUCKET(a,b,c,d) == (a<<16)|b|c|d does a single <<16, and its
 * alignment (objSize & 0xfffc) + 4 == objSize - ((objSize&3) - 4). No bug in saphi.
 */

typedef unsigned int uint;
typedef unsigned int undefined4;

extern char *D_8008D2AC;                                          /* gGT */
extern int func_8004205C(uint flags, undefined4 thtick, undefined4 name, undefined4 parent); /* PROC_BirthWithObject */
extern int func_8003086C(int model, undefined4 name, int thread); /* INSTANCE_Birth3D */

undefined4 INSTANCE_BirthWithThread(int modelID, undefined4 name, uint poolType, uint bucket,
                                    undefined4 funcThTick, uint objSize, undefined4 parent)
{
  undefined4 ret;
  int four;
  char *modelSlot;
  short modelIDs;
  undefined4 nameCopy;
  int thread;
  int model;
  int off34;
  int modelKeep;
  short unused;
  modelIDs = (short) modelID;
  nameCopy = name;
  four = 4;
  modelSlot = (D_8008D2AC - (-(modelID * four))) + 0x2160;         /* &gGT->modelPtr[modelID] */
  model = *((int *) modelSlot);
  if (model == 0)
  {
    ret = 0;
  }
  else
  {
    modelKeep = model;
    if ((3 & objSize) == 0)
    {
      objSize = (objSize << 12) << four;                            /* size << 16 */
    }
    else
    {
      objSize = objSize;
      objSize = objSize - ((objSize & 3) - four);                   /* align up to 4 */
      objSize = objSize * 0x10000;                                  /* << 16 */
    }
    ret = poolType | objSize;
    thread = func_8004205C(ret | bucket, funcThTick, nameCopy, parent);
    *((short *) (thread + 0x44)) = modelIDs;
    ret = func_8003086C(modelKeep, nameCopy, thread);
    *((undefined4 *) (thread + (off34 = 0x34))) = ret;
  }
  return ret;
}
