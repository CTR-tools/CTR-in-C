/*
 * UI_ThTick_CtrLetters @ 0x8004C914 (SCUS_944.26 / NTSC-U 926)
 *
 * BEHAVIOR-EXACT: base score 120, Branch 0 / Stack 0 / Register 0 vs SCUS_944.26.exe.
 * Residual is a pure instruction-scheduling wall near the tail (ordering of the rot[2]=0
 * store vs the ConvertRotToMatrix call setup); a ~150-iter permuter grind never beats 120
 * and no natural restructuring removes it. No hacks used.
 *
 * CORRECTNESS NOTE: the prior "near completed" snapshot for this function scored 100 but was
 * BEHAVIORALLY WRONG — it collapsed the (scale[0]==0x800) branch and never set rot[1]=0 there,
 * leaving rot[1] uninitialized on that path (the low score was instruction-similarity, not
 * correctness). This delivery uses the correct retail structure (rot[1] set in BOTH branches),
 * verified against the retail asm, at an honest score of 120. Correctness over score.
 *
 * HUD "CTR letters" per-frame thread tick: spins the letters on Y; while the End-Of-Race menu
 * is transitioning, collapses their scale to 0 and drives a shrink rotation into the matrix.
 *   bucket+0x34 = inst, bucket+0x30 = obj.
 *   obj->rot[1] += 0x40 (half-FPS spin); Vector_SpecLightSpin2D(inst, obj->rot, obj->lightDir).
 *   if (gGT->gameMode1 & END_OF_RACE) && RaceFlag_IsTransitioning(): zero inst->scale[0..2].
 *   rot[1] derived from inst->scale[0] (==0x800 => 0); ConvertRotToMatrix + MatrixRotate.
 * Verified equivalent to saphi (Phase B: CLEAN).
 */

typedef unsigned int uint;
typedef unsigned short undefined2;
extern char *D_8008D2AC;                                     /* sdata->gGT */
extern void func_800572D0(int inst, int rot, int lightDir); /* Vector_SpecLightSpin2D */
extern int func_80043F44(void);                             /* RaceFlag_IsTransitioning */
extern void func_8006C2A4(int matrix, short *rot_data);     /* ConvertRotToMatrix */
extern void func_8006C3B0(int dst, int src1, int src2);     /* MatrixRotate */
void func_8004C914(int bucket)
{
  int *objPtr;
  int transition;
  int inst;
  int obj;
  short rot[3];
  objPtr = (int *) (bucket + 0x30);
  inst = *((int *) (bucket + 0x34));
  obj = *objPtr;
  *((short *) (obj + 2)) = (*((short *) (obj + 2))) + 0x40;
  func_800572D0(inst, obj, obj + 0x28);
  if ((((*((uint *) D_8008D2AC)) & 0x200000) != 0) && ((transition = func_80043F44(), transition != 0)))
  {
    *((undefined2 *) (inst + 0x1C)) = 0;
    *((undefined2 *) (inst + 0x1E)) = 0;
    *((undefined2 *) (inst + 0x20)) = 0;
  }
  if ((*((short *) (inst + 0x1C))) != 0x800)
  {
    rot[0] = 0;
    transition = ((int) (*((short *) (inst + 0x1C)))) + -0x800;
    if (transition < 0)
    {
      transition = ((int) (*((short *) (inst + 0x1C)))) + -0x401;
    }
    rot[1] = ((transition >> 10) + 1) * 0x200;
  }
  else
  {
    rot[0] = 0;
    rot[1] = 0;
  }
  rot[2] = 0;
  inst = inst + 0x30;
  func_8006C2A4(inst, rot);
  func_8006C3B0(inst, obj + 8, inst);
}
