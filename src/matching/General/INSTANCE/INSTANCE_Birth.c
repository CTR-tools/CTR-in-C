/*
 * INSTANCE_Birth @ 0x80030778 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 155, Branch 0 / Stack 0, Register 11 residual
 * (a gcc-2.8.1 register-allocation / constant-materialization wall: retail loads
 * the small init constants 0xFE/0xC/1/0x1000 with individual `addiu`s and reuses
 * registers slightly differently). Delivered as NATURAL C — the prior "near
 * completed" score-140 snapshot reached its number with `*(&x)` pointer-deref and
 * `if(1)` noise; this natural form (score 155) is preferred per the no-hacky-match
 * rule. vs SCUS_944.26.exe; finished from the "near completed" prior grind set.
 *
 * Initialises a freshly-allocated instance: copies up to 15 name bytes into
 * inst[8..0x16] and null-pads through inst[0x17]; sets the fixed header bytes at
 * 0x50..0x53 (0xFE,0xC,0,1); stores model (0x18), unit scale 0x1000 (0x1C/0x1E/0x20),
 * drawFlags (0x28), colour 0x7F7F7F (0x58), thread (0x6C), and zeroes 0x22/0x24/0x2C/
 * 0x54/0x56/0x70. Then, for each active player (gGT->numPlyrCurrGame), points that
 * player's per-player draw slot (inst+0x74, stride 0x88) at gGT + 0x168 + k*0x110
 * and zeroes inst+0xE0 / inst+0xB8.
 *
 * saphi INSTANCE_Birth audited vs this asm + Ghidra (Phase B).
 */

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned char undefined;
typedef unsigned int undefined4;
typedef unsigned short undefined2;

extern char *D_8008D2AC;                                   /* gGT */

void INSTANCE_Birth(int inst, undefined4 model, undefined *name, undefined4 thread, undefined4 drawFlags)
{
  undefined *cursor;
  undefined *nameEnd;
  char *gGT;
  int pbOff;
  int plyr;
  cursor = (undefined *) (inst + 8);
  nameEnd = (undefined *) (inst + 0x17);
  if (cursor < nameEnd)
  {
    do
    {
      *cursor = *name;
      cursor = cursor + 1;
      name = name + 1;
    }
    while (cursor < nameEnd);
  }
  while (!(nameEnd < cursor))
  {
    *cursor = 0;
    cursor = cursor + 1;
  }

  *((undefined *) (inst + 0x50)) = 0xFE;
  *((undefined *) (inst + 0x51)) = 0xC;
  *((undefined *) (inst + 0x52)) = 0;
  *((undefined *) (inst + 0x53)) = 1;
  gGT = D_8008D2AC;
  *((undefined4 *) (inst + 0x18)) = model;
  *((undefined2 *) (inst + 0x1C)) = 0x1000;
  *((undefined2 *) (inst + 0x1E)) = 0x1000;
  *((undefined2 *) (inst + 0x20)) = 0x1000;
  *((undefined4 *) (inst + 0x28)) = drawFlags;
  *((undefined2 *) (inst + 0x22)) = 0;
  *((undefined4 *) (inst + 0x24)) = 0;
  *((undefined4 *) (inst + 0x2C)) = 0;
  *((undefined2 *) (inst + 0x54)) = 0;
  *((undefined2 *) (inst + 0x56)) = 0;
  *((undefined4 *) (inst + 0x58)) = 0x7F7F7F;
  *((undefined4 *) (inst + 0x6C)) = thread;
  *((undefined4 *) (inst + 0x70)) = 0;
  plyr = 0;
  if (gGT[0x1CA8] != 0)
  {
    pbOff = 0x168;
    do
    {
      *((undefined4 *) (inst + 0xE0)) = 0;
      *((char **) (inst + 0x74)) = gGT + pbOff;
      *((undefined4 *) (inst + 0xB8)) = 0;
      pbOff = pbOff + 0x110;
      inst = inst + 0x88;
      plyr = plyr + 1;
    }
    while (plyr < ((int) ((uint) ((byte) gGT[0x1CA8]))));
  }
}
