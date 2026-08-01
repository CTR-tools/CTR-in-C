/*
 * INSTANCE_LevInitAll @ 0x80030AD4 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 3490, Branch Differences 0, Stack Differences 0,
 * Register Differences 98 — a genuine gcc-2.8.1 register-allocation/scheduling wall on
 * this 256-instruction loader (the permuter's 220s grind only reached 3485). From the
 * aibox staging set; NOT byte-close, but behaviorally correct and verified vs the asm.
 *
 * FALSE-FRIEND FIX: the aibox seed scored a lower 3110 by being behaviorally WRONG —
 * it never initialized `instData` (used `instData[7] = inst` as its first, uninitialized
 * use), and hid the real read of a local behind a `*(&x)` pointer-to-local trick. The
 * retail asm is unambiguous: `addiu $s1, $s2, 0x10` sets instData = instDef + 0x10 ONCE
 * before the loop, and both $s1/$s2 advance +0x40 per iteration. Restored that init and
 * removed the hack; honest score rose but the behavior is correct.
 *
 * Copies each InstDef (from the LEV) into a freshly-allocated Instance pool slot, sets up
 * its header/reflection/rotation-matrix, links inst<->instDef, wires the per-player draw
 * records (idpp, stride 0x88, one per numPlyrCurrGame), optionally calls the model's
 * LevInstDef-birth callback, then applies per-gamemode visibility masks (time-trial /
 * relic-race / battle time-crate rules) that clear the draw flags for certain model IDs.
 *
 * saphi INSTANCE_LevInitAll (game/INSTANCE.c:207) audited CLEAN vs this asm + Ghidra.
 */

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char undefined;
typedef unsigned int undefined4;
typedef unsigned short undefined2;

extern char *D_8008D2AC;                                /* gGT */
extern int func_8003186C(void *pool);                   /* JitPool_Add (alloc instance) */
extern void func_8006C2A4(int matrix, int rotation_data); /* ConvertRotToMatrix */
extern int func_8001D094(int modelID);                  /* COLL_LevModelMeta */

void INSTANCE_LevInitAll(undefined4 *instDef, int numInst)
{
  int depthBias;
  ushort modelIDu;
  undefined *gGTb;
  undefined4 *copyDst;
  int instLink;
  int *unusedPtr;
  short cmpVal;
  int inst;
  undefined4 *copySrc;
  undefined4 *copyNext;
  undefined *pushBufPtr;
  undefined4 *copyEnd;
  int modelID;
  undefined4 *copyCur;
  int tmpModelSub;
  int idppCursor;
  int *posZPtr;
  short posZ;
  int levModelMeta;
  int pushBufByteOff;
  int inst2;
  undefined4 w2;
  undefined4 w1;
  int ttVisible;
  int five;
  undefined4 w3;
  int *instData;
  int i;

  i = 0;
  if (0 < numInst)
  {
    i = i + 1;
    instData = (int *) instDef + 4;                     /* = instDef + 0x10 (skip name[0x10]) */
    do
    {
      inst = func_8003186C(D_8008D2AC + 0x18F8);         /* JitPools.instance */
      instData[7] = inst;                                /* instDef->ptrInstance (instDef+0x2c) */
      if (inst == 0)
      {
        return;                                          /* pool exhausted: OOPS more instdefs than instances */
      }
      copyEnd = instDef - (-8);                          /* instDef + 8 */
      copyNext = instDef;
      copyDst = (undefined4 *) (inst + 8);               /* skip Prev/Next */
      do                                                 /* copy 32 bytes of InstDef -> instance */
      {
        do
        {
          copyCur = copyDst;
          copySrc = copyNext;
          w1 = copySrc[1];
          w2 = copySrc[2];
          w3 = copySrc[3];
          *copyDst = *copySrc;
          copyCur[1] = w1;
          copyDst[2] = w2;
          copyDst[3] = w3;
        }
        while (0);
        copyNext = copySrc + 4;
        copyDst = copyCur + 4;
      }
      while (copyNext != copyEnd);
      five = 5;
      w1 = copySrc[five];
      w2 = copySrc[6];
      copyDst[4] = *copyNext;
      copyDst[5] = w1;
      copyDst[6] = w2;
      cmpVal = -2;
      *((char *) (inst + 0x50)) = (*((char *) (instData + 5))) + cmpVal;
      depthBias = *((char *) (instData + 5));
      *((undefined4 *) (inst + 0x58)) = 0x7F7F7F;        /* reflect color */
      inst2 = inst;
      *((undefined *) (inst + 0x52)) = (*((undefined2 *) (inst2 + 0x56)) = 0);
      instLink = 0x2C;
      *((undefined2 *) (inst + 0x54)) = 0;
      *((undefined4 **) (inst + instLink)) = instDef;    /* inst->instDef (link both ends) */
      *((undefined *) (inst + 0x53)) = 1;
      *((undefined4 *) (inst + 0x70)) = 0;
      *((char *) (inst + 0x51)) = depthBias + 0xC;
      func_8006C2A4(inst + 0x30, w3 = ((int) instDef) + 0x36); /* rotation shorts -> matrix */
      gGTb = (undefined *) D_8008D2AC;
      idppCursor = inst;
      *((int *) (inst + 0x44)) = (int) (*((short *) (instData + 8)));  /* posX */
      *((int *) (inst + 0x48)) = (int) (*((short *) (((int) instData) + 0x22))); /* posY */
      modelIDu = 0;
      posZPtr = instData + 9;
      posZ = *((short *) posZPtr);
      modelID = posZ;
      *((undefined4 *) (inst + 0x6C)) = 0;               /* inst->thread = NULL */
      *((int *) (inst + 0x4C)) = (int) modelID;          /* posZ */
      levModelMeta = 0;
      if (gGTb[0x1CA8] != 0)                             /* numPlyrCurrGame */
      {
        pushBufByteOff = 0x168;
        do
        {
          pushBufPtr = gGTb + pushBufByteOff;
          pushBufByteOff = pushBufByteOff + 0x110;
          *((undefined4 *) (idppCursor + 0xE0)) = modelIDu;   /* LOD ModelHeader = 0 */
          do
          {
            *((undefined **) (idppCursor + 0x74)) = pushBufPtr; /* idpp[p].pushBuffer */
            levModelMeta = levModelMeta + 1;
          }
          while (0);
          idppCursor = 0x88 + idppCursor;
        }
        while (levModelMeta < ((int) ((uint) ((byte) gGTb[0x1CA8]))));
      }
      /* LEV instances enabled AND model has a LevInstDef-birth callback -> call it */
      if (((((*((uint *) (D_8008D2AC + 8))) & 0x100) == 0) &&
           ((levModelMeta = func_8001D094((int) (*((short *) ((*instData) + 0x10)))), levModelMeta != 0))) &&
          ((*((int *) (levModelMeta + 4))) != 0))
      {
        (*((void (**)(int)) (levModelMeta + 4)))(inst);
      }
      /* Time-trial: hide time-crate / pickup models unless the gamemode rules keep them */
      if ((((*((uint *) D_8008D2AC)) & 0x20000) == 0) ||
          (ttVisible = ((modelIDu = *((ushort *) ((*instData) + 0x10)), (1 < (((uint) modelIDu) - 7)) && (modelIDu != 2))) &&
                       (((modelIDu != 0x5C) && (modelIDu != 100)) && (modelIDu != 0x65))))
      {
        if (((*((uint *) D_8008D2AC)) & 0x4000000) == 0)     /* not relic race */
        {
          modelID = *((short *) ((*instData) + 0x10));
          if ((modelID != 0x5C) && ((cmpVal = 0x65, modelID != 100)))
          {
            goto LAB_80030D94;
          }
          goto LAB_80030D9C;
        }
        modelID = *((short *) ((*instData) + 0x10));
        if ((modelID != 0x5C) && ((modelID != 100) && (modelID != 0x65)))
        {
          cmpVal = 2;
          tmpModelSub = (*((short *) ((*instData) + 0x10))) - 7U;
          {
            goto LAB_80030D94;
          }
          goto LAB_80030D9C;
        }
        *((int *) (D_8008D2AC + 0x1E2C)) = (*((int *) (D_8008D2AC + 0x1E2C))) + 1; /* count time crates */
        goto LAB_80030DAC;
        LAB_80030D94:
        if (modelID != cmpVal)
        {
          goto LAB_80030DAC;
        }

        LAB_80030D9C:
        *((uint *) (inst + 0x28)) = (*((uint *) (inst + 0x28))) & 0xFFFFFFF0;  /* hide */

      }
      LAB_80030DAC:
      if (((*((uint *) D_8008D2AC)) & 0x8000000) == 0)
      {
        modelID = *((short *) ((*instData) + 0x10));
        if ((modelID != 0x60) && ((cmpVal = 0x27, modelID != 6)))
        {
          do
          {
          }
          while (0);
          goto LAB_80030E18;
        }
      }
      else
      {
        modelID = *((short *) ((*instData) + 0x10));
        do
        {
          ;
          if (modelID == 0x60)
          {
            *((int *) (D_8008D2AC + 0x1E28)) = (*((int *) (D_8008D2AC + 0x1E28))) + 1;
          }
          else
          {
            LAB_80030E18:
            if (modelID == 7)
            {
              goto LAB_80030E20;
              LAB_80030E20:
              *((uint *) (inst + 0x28)) = (*((uint *) (inst + 0x28))) & 0xFFFFFFF0;

            }

          }
        }
        while (0);
      }

      if (((((*((uint *) D_8008D2AC)) & 0x80000) == 0) || (((*((uint *) (D_8008D2AC + 8))) & 8) == 0)) &&
          ((((uint) (*((ushort *) ((*instData) + 0x10)))) - 0x93) < 3))
      {
        *((uint *) (inst + 0x28)) = (*((uint *) (inst + 0x28))) & 0xFFFFFFF0;
      }
      instData = instData + 0x10;
      instDef = instDef + 0x10;
    }
    while (i < numInst);
  }
}
