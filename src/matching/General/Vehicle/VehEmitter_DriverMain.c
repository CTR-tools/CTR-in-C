/*
 * VehEmitter_DriverMain @ 0x80059A18 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact: base score 9960, Branch 0 / Stack 0 (Register 276) — a genuine
 * gcc-2.8.1 WHOLE-FUNCTION register-coloring wall on this 926-instruction GTE
 * particle-emitter driver. Byte-exact is not reachable without pinning nearly every
 * local; a 250-iteration PERM_RANDOMIZE reorder grind never beat this floor. From
 * the aibox staging set. (Inline-asm GTE is used for the cop2 SetRotMatrix sequence,
 * permitted per user for GTE code.)
 *
 * TWO false-friend bugs in the original Ghidra-derived base were FIXED to match retail
 * (verified vs asm + saphi game/Vehicle/VehEmitter.c:703):
 *   - rumble gate: `(int)(kartState-4) < 2` (signed slti — fires for kartState 0..5)
 *     corrected to `(unsigned)(kartState-4) < 2` (retail `sltiu` @0x8005AA3C — {REV_ENGINE=4,
 *     MASK_GRABBED=5} only).
 *   - non-rumble tail: the shared JogCon2(func_800263FC) call passed a2=0x100 for BOTH
 *     paths; retail's `addiu $a2,0x100` (@0x8005AAE8) is rumble-only (non-rumble skips it),
 *     so the non-rumble path is JogCon2(d, 0, 0). Moved the 0x100 into the rumble branch.
 * saphi already had both correct; the bugs lived only in this matching seed.
 *
 * Generic decompiler locals left as-is (926-line fn, per large-function precedent).
 */
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef char undefined;
typedef short undefined2;
typedef int undefined4;
typedef struct 
{
  short m[3][3];
  short pad;
  int t[3];
} MATRIX;
typedef struct 
{
  int vx;
  int vy;
  int vz;
  int pad;
} VECTOR;
extern undefined *D_8008D2AC;
extern undefined4 D_8008980C;
extern undefined4 D_80089584;
extern undefined4 D_800896C8;
extern int func_80058F9C(int, int, int, int, int);
extern void func_8002E690(int, int, int);
extern volatile long func_80026440(int, uint, uint);
extern void func_80026540(int, uint, uint);
extern void func_800264C0(int, volatile unsigned short, int);
extern int func_80040308(int, undefined4, void *);
extern void func_80059344(int, void *);
extern void func_80059558(int, int);
extern void func_80059780(int, void *);
extern void func_80028808(undefined *);
extern int func_8003D1C0(int);
extern int func_8003D184(int);
extern int func_80042394(void *, int);
extern void func_80059100(int, VECTOR *, void *);
extern volatile unsigned short func_800263FC(volatile unsigned int, int, int);
extern void func_800263A0(int, int, int);
void func_80059A18(int param_1, int param_2, int param_3, uint param_4)
{
  unsigned char new_var7;
  int new_var4;
  byte bVar1;
  int iVar28;
  int new_var22;
  short sVar2;
  short sVar3;
  int new_var24;
  int new_var18;
  short sVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  ushort uVar8;
  undefined4 *new_var12;
  int iVar9;
  int new_var8;
  MATRIX *pMVar10;
  unsigned int iVar11;
  short *new_var10;
  int new_var23;
  undefined *puVar12;
  uint uVar13;
  undefined4 uVar14;
  unsigned int new_var6;
  int new_var11;
  int new_var29;
  int new_var13;
  unsigned long long new_var28;
  long long new_var27;
  short *new_var2;
  int new_var19;
  int new_var31;
  register undefined2 uVar15 asm("$9");
  int new_var3;
  int iVar16;
  int iVar17;
  int new_var9;
  VECTOR *r0;
  int iVar15;
  int iVar18;
  char new_var26;
  undefined4 uVar19;
  int iVar20;
  int new_var32;
  undefined *puVar21;
  int iVar22;
  int iVar23;
  int iVar24;
  unsigned short cVar25;
  register byte bVar26 asm("$22");
  int new_var17;
  long new_var21;
  int new_var30;
  int new_var16;
  register int iVar27 asm("$23");
  int new_var15;
  byte *new_var;
  byte bVar2;
  undefined auStack_68[32];
  uint new_var20;
  uint local_30;
  int new_var14;
  new_var24 = param_1;
  iVar27 = *((int *) (new_var24 + 0x34));
  
  *((uint *) (param_2 + 0x2c4)) = ((*((uint *) (param_2 + 0x2c4))) << 4) & 0xffffff;
  *((byte *) (param_2 + 0xc3)) = ((*((char *) (param_2 + 0xc3))) - 1U) & 7;
  new_var20 = 0x18;
  new_var16 = 0x30;
  new_var21 = 0x30;
  local_30 = *((uint *) (iVar22 + 4));
  iVar23 = (int) (*((short *) (iVar22 + 0x30)));
  new_var28 = 0x100;
  
  if ((*((short *) (param_1 + 0x44))) == new_var20)
  {
    iVar18 = -1;
    if ((((*((uint *) (param_2 + 0x2c8))) & 1) != 0) && (((*((uint *) (iVar22 + 4))) & 0x20) == 0))
    {
      iVar18 = (int) (*((short *) (iVar22 + 0x32)));
    }
    
    iVar11 = (int) (*((short *) (param_2 + 0x38e)));
    if (iVar11 < 0)
    {
      iVar11 = -iVar11;
    }
    iVar9 = func_80058F9C(iVar11, 0, 5000, 0, 200);
    iVar11 = (int) (*((short *) (param_2 + 0x38e)));
    new_var17 = iVar11 < 0;
    if (new_var17)
    {
      iVar11 = -iVar11;
    }
    iVar11 = func_80058F9C(iVar11, 0, 12000, 0x6c, 0xd2);
    
    
    new_var14 = param_2 + 0x304;
    uVar13 = iVar11 << 8;
    new_var30 = new_var14;
    new_var15 = iVar9 << 0x10;
    if (((*((uint *) (param_2 + 0x2c8))) & 0x10000) != 0)
    {
      uVar13 = uVar13 | 0x1000000;
    }
    uVar14 = ((unsigned long long) new_var15) | uVar13;
    func_8002E690(new_var30, iVar18, uVar14 | 0x80);
    
    if (((*((uint *) (param_2 + 0x2c8))) & 0x100000) == 0)
    {
      int tmp_a = (int) (*((short *) (param_2 + 0x38e)));
      if (tmp_a < 0)
      {
        tmp_a = -tmp_a;
      }
      if (0x200 < tmp_a)
      {
        func_80026440(param_2, (uint) (*((byte *) (iVar22 + 0x2c))), (uint) (*((byte *) (iVar22 + 0x2d))));
        func_80026540(param_2, (uint) (*((byte *) (iVar22 + 0x2e))), (uint) (*((byte *) (iVar22 + 0x2f))));
      }
      if (((*((uint *) (param_2 + 0x2c8))) & 2) != 0)
      {
        int tmp_b = (int) (*((short *) (param_2 + 0x392)));
        if (tmp_b < 0)
        {
          tmp_b = -tmp_b;
        }
        if (0x1600 < tmp_b)
        {
          func_800264C0(param_2, 3, 0xFF);
        }
      }
    }
  }
  if (((byte) D_8008D2AC[0x1ca8]) < 2)
  {
    if (((int) (*((short *) (param_2 + 0x38c)))) < 0)
    {
      iVar18 = -iVar18;
    }
    new_var9 = *((int *) (param_2 + 0x358));
    new_var24 = 0x30;
    if ((0x500 < iVar18) && ((*((char *) (param_2 + 0xc2))) == '\x0e'))
    {
      new_var8 = 0;
      if (((*((uint *) (param_2 + 0x2c8))) & 2) != 0)
      {
        do
        {
          iVar15 = func_80040308(0, *((undefined4 *) (D_8008D2AC + 0x2148)), &D_8008980C);
          if ((iVar20 = iVar15) != 0)
          {
            *((undefined *) (iVar15 + 0x18)) = *((undefined *) ((*((int *) (param_2 + 0x1c))) + 0x50));
            *((undefined4 *) (iVar20 + 0x20)) = *((undefined4 *) (param_2 + 0x1c));
            bVar2 = *((byte *) (param_2 + 0x4a));
            new_var13 = bVar2;
            *((byte *) (iVar20 + 0x19)) = new_var13;
            *((int *) (iVar20 + 0x24)) = (*((int *) (iVar20 + 0x24))) + (((int) (*((short *) (iVar15 + 0x28)))) * 0x10);
            *((int *) (iVar15 + 0x34)) = (*((int *) (iVar15 + 0x34))) + (((int) (*((short *) (iVar15 + 0x38)))) * 0x10);
            *((short *) (iVar15 + 0x2a)) = (*((short *) (iVar15 + 0x2a))) - ((*((short *) (iVar15 + 0x28))) >> 4);
            *((short *) (iVar15 + 0x3a)) = (*((short *) (new_var13 = iVar20 + 0x3a))) - ((*((short *) (iVar15 + 0x38))) >> 4);
          }
          iVar18 = iVar18 + (-1);
        }
        while (iVar18 != new_var8);
      }
      else
      {
        new_var22 = func_80040308(0, *((undefined4 *) (D_8008D2AC + 0x2148)), &D_8008980C);
        new_var10 = (short *) (param_2 + 0x4a);
        iVar18 = new_var22;
        if (iVar18 != 0)
        {
          *((undefined *) (iVar18 + 0x18)) = *((undefined *) ((*((int *) (param_2 + 0x1c))) + 0x50));
          *((undefined4 *) (iVar18 + 0x20)) = *((undefined4 *) (param_2 + 0x1c));
          uVar14 = (bVar2 = *new_var10);
          sVar3 = uVar14;
          *((byte *) (iVar18 + 0x19)) = sVar3;
          *((int *) (iVar18 + 0x24)) = (*((int *) (iVar18 + 0x24))) + (((int) (*((short *) (iVar18 + 0x28)))) * 0x10);
          *((int *) (iVar18 + 0x34)) = (*((int *) (iVar18 + 0x34))) + (((int) (*((short *) (iVar18 + 0x38)))) * 0x10);
          *((short *) (iVar18 + 0x2a)) = (*((short *) (iVar18 + 0x2a))) - ((*((short *) (iVar18 + 0x28))) >> 4);
          *((short *) (iVar18 + 0x3a)) = (*((short *) (iVar18 + 0x3a))) - ((*((short *) (iVar18 + 0x38))) >> 4);
          return;
        }
      }
    }
    if (((byte) D_8008D2AC[0x1ca8]) < 2)
    {
      if (((local_30 & 0x40) != 0) && (((*((uint *) (param_2 + 0x2c8))) & 2) != 0))
      {
        int tmp_c = (int) (*((short *) (param_2 + 0x38e)));
        if (tmp_c < 0)
        {
          tmp_c = -tmp_c;
        }
        if (0x600 < tmp_c)
        {
          int tmp_d = (int) (*((short *) (param_2 + 0x392)));
          if (tmp_d < 0)
          {
            *((undefined2 *) (param_2 + 0x50)) = 0;
            goto LAB_8005a094;
          }
          tmp_d = -tmp_d;
          new_var12 = &D_80089584;
          if (0x1600 < tmp_d)
          {
            {
              int *_m = (int *) (iVar27 + new_var16);
              register int _a asm("$12");
              register int _b asm("$13");
              register int _c asm("$14");
              _a = _m[0];
              _b = _m[1];
              asm volatile("ctc2 %0, $0" : : "r"(_a));
              asm volatile("ctc2 %0, $1" : : "r"(_b));
              _a = _m[2];
              _b = _m[3];
              _c = _m[4];
              asm volatile("ctc2 %0, $2" : : "r"(_a));
              asm volatile("ctc2 %0, $3" : : "r"(_b));
              asm volatile("ctc2 %0, $4" : : "r"(_c));
              _a = _m[5];
              _b = _m[6];
              asm volatile("ctc2 %0, $5" : : "r"(_a));
              _c = _m[7];
              asm volatile("ctc2 %0, $6" : : "r"(_b));
              _m = (int *) (iVar27 + new_var24);
              asm volatile("ctc2 %0, $7" : : "r"(_c));
            }
            func_80059344(param_2, new_var12);
          }
        }
      }
      iVar22 = new_var9 + 0x18;
      if ((*((int *) iVar22)) != 0)
      {
        {
          int *_m;
          register int _a asm("$12");
          register int _b asm("$13");
          register int _c asm("$14");
          _a = _m[0];
          _b = _m[1];
          asm volatile("ctc2 %0, $0" : : "r"(_a));
          asm volatile("ctc2 %0, $1" : : "r"(_b));
          _a = _m[2];
          _b = _m[3];
          _c = _m[4];
          asm volatile("ctc2 %0, $2" : : "r"(_a));
          asm volatile("ctc2 %0, $3" : : "r"(_b));
          asm volatile("ctc2 %0, $4" : : "r"(_c));
          _a = _m[5];
          _b = _m[6];
          asm volatile("ctc2 %0, $5" : : "r"(_a));
          _c = _m[7];
          asm volatile("ctc2 %0, $6" : : "r"(_b));
          asm volatile("ctc2 %0, $7" : : "r"(_c));
        }
        iVar16 = (uVar19 = *((int *) (iVar22 + 0x1c)));
        iVar18 = iVar16;
        if ((iVar18 == 0) || (((*((uint *) (D_8008D2AC + 0x1cec))) & 1) == 0))
        {
          iVar18 = *((int *) (iVar22 + 0x18));
          func_80059558(param_2, iVar18);
        }
      }
      if (((*((short *) (param_2 + 0x3fe))) == 0xf0) && ((*((char *) (param_2 + 0x376))) != '\x05'))
      {
        {
          int *_m = (int *) (iVar27 + new_var21);
          register int _a asm("$12");
          register int _b asm("$13");
          register int _c asm("$14");
          _a = _m[0];
          _b = _m[1];
          asm volatile("ctc2 %0, $0" : : "r"(_a));
          asm volatile("ctc2 %0, $1" : : "r"(_b));
          _a = _m[2];
          _b = _m[3];
          _c = _m[4];
          asm volatile("ctc2 %0, $2" : : "r"(_a));
          asm volatile("ctc2 %0, $3" : : "r"(_b));
          asm volatile("ctc2 %0, $4" : : "r"(_c));
          _a = _m[5];
          _b = _m[6];
          asm volatile("ctc2 %0, $5" : : "r"(_a));
          _c = _m[7];
          asm volatile("ctc2 %0, $6" : : "r"(_b));
          asm volatile("ctc2 %0, $7" : : "r"(_c));
        }
        func_80059780(param_2, &D_800896C8);
        uVar19 = 0x14;
        sVar7 = (*((short *) (param_2 + 0x496))) + 0x14;
        *((short *) (param_2 + 0x496)) = sVar7;
        if (0xff < sVar7)
        {
          *((undefined2 *) (param_2 + 0x496)) = 0xff;
        }
      }
      else
      {
        do
        {
          if ((*((short *) (param_2 + 0x3fe))) == 0)
          {
            goto LAB_8005a9d8;
          }
          uVar8 = (*((short *) (param_2 + 0x496))) - 0x14;
          *((ushort *) (param_2 + 0x496)) = uVar8;
          iVar17 = uVar8;
          if (((int) (((uint) iVar17) << 0x10)) < 0)
          {
            *((undefined2 *) (param_2 + 0x496)) = 0;
          }
        }
        while (0);
        uVar19 = 0x14;
        if ((*((short *) (param_2 + 0x496))) == 0)
        {
          uVar19 = 0xffffffff;
        }
      }
      if ((*((short *) ((*((int *) (new_var24 + 0x68))) + 0x44))) == 0x18)
      {
        new_var20 = ((int) (*((short *) (param_2 + 0x496)))) << 0x10;
        uVar13 = new_var20;
        do
        {
          if (((*((uint *) (param_2 + 0x2c8))) & 0x10000) == 0)
          {
            param_3 = uVar13 | 0x8080;
          }
          else
          {
            param_3 = uVar13 | 0x1008080;
          }
        }
        while (0);
        func_8002E690(param_2 + 0x308, (int) uVar19, param_3);
        iVar22 = func_80058F9C(iVar22, 2000, 12000, 0x92, 0x78);
      }
    }
  }
  if ((local_30 & 8) != 0)
  {
    *((uint *) (param_2 + 0x2c8)) = (*((uint *) (param_2 + 0x2c8))) | 0x1800;
  }

  
  bVar26 = *((byte *) (param_2 + 0x4c));
  if (bVar26 == 1)
  {
    *((uint *) (param_2 + 0x2c8)) = (*((uint *) (param_2 + 0x2c8))) | 0x800;
    LAB_8005a094:
    *((uint *) (param_2 + 0x2c8)) = (*((uint *) (param_2 + 0x2c8))) & 0xffffefff;

  }
  

  if ((((*((uint *) (param_2 + 0x2c8))) & 1) == 0) || (((*((uint *) (param_2 + 0x2c8))) & 0x1800) == 0))
  {
    LAB_8005a73c:
    puVar12 = *((undefined **) (param_2 + 0x300));

    if (puVar12 != ((undefined *) 0x0))
    {
      func_80028808(puVar12);
      *((undefined4 *) (param_2 + 0x300)) = 0;
    }
  }
  else
  {
    new_var10 = &(*((short *) (param_2 + 0x38e)));
    uVar14 = *new_var10;
    iVar11 = uVar14;
    iVar22 = (int) iVar11;
    if (iVar22 < 0)
    {
      iVar22 = -iVar22;
    }
    if (iVar22 <= 0x200)
    {
      goto LAB_8005a73c;
    }
    bVar26 = 0;
    if ((*((short *) (param_1 + 0x44))) == 0x18)
    {
      iVar18 = (int) (*((signed char *) (param_2 + 0x4b)));
      if (iVar18 < 0)
      {
        iVar18 = -iVar18;
      }
      iVar11 = func_80058F9C(iVar22, 2000, 12000, 0x14, 0xaa);
      new_var8 = iVar11 + (iVar18 >> 1);
      iVar28 = new_var8 * 0x10000;
      new_var14 = *new_var10;
      if (((int) new_var14) < 0)
      {
        iVar22 = -iVar22;
      }
      sVar5 = 0x80U;
      uVar19 = param_2;
      if ((*((char *) (param_2 + 0x376))) == '\x02')
      {
        iVar9 = (int) (*((short *) (param_2 + 0x3d4)));
        do
        {
          if (iVar9 < 0)
          {
            iVar9 = -iVar9;
          }
          iVar22 = iVar22 - iVar9;
          if (iVar22 < 0)
          {
            iVar22 = 0;
          }
        }
        while (0);
      }
      iVar22 = iVar22 + iVar18;
      if (0x92 < iVar22)
      {
        iVar23 = 0x1a;
        new_var13 = sVar5;
        new_var14 = new_var13 - (((int) (((uint) (*((byte *) (param_2 + 0x4b)))) << 0x18)) >> iVar23);
      }
      uVar13 = iVar22 << 8;
      if (((*((uint *) (param_2 + 0x2c8))) & 0x10000) != 0)
      {
        uVar13 = uVar13 | 0x1000000;
      }
      new_var22 = param_2 + 0x300;
      new_var27 = new_var14 | (uVar13 | iVar28);
      func_8002E690(new_var22, (int) new_var23, new_var27);
    }
    if (((*((uint *) (iVar27 + 0x28))) & 0x2000) == 0)
    {
      cVar25 = *((char *) (iVar27 + 0x50));
    }
    else
    {
      cVar25 = *((char *) (iVar27 + 0x51));
    }
    cVar25 = cVar25 + '\x02';
    func_8003D1C0((int) (*((short *) (param_2 + 0x396))));
    bVar2 = 0xc3;
    {
      static int mul_15 = 0xf;
      static int mul_10 = 10;
      new_var23 = mul_15;
      iVar22 = 0x92;
      new_var23 = iVar23;
      iVar22 = func_8003D184((int) (*((short *) (param_2 + 0x396))));
      iVar22 = iVar22 * new_var23;
      iVar23 = func_8003D1C0((int) (*((short *) (param_2 + 0x396))));
      func_8003D184((int) (*((short *) (param_2 + 0x396))));
      new_var31 = mul_15;
      new_var18 = func_8003D1C0((int) (*((short *) (param_2 + 0x396))));
      iVar18 = new_var18;
      func_8003D184((int) (*((short *) (param_2 + 0x396))));
      iVar23 = iVar23 * new_var31;
      iVar24 = mul_10;
      iVar17 = iVar24;
      iVar17 = iVar18;
      iVar17 = (iVar17 * iVar17) >> 0xc;
      func_8003D1C0((int) (*((short *) (param_2 + 0x396))));
      new_var30 = (int) (*((short *) (param_2 + 0x396)));
      puVar12 = (undefined *) new_var30;
      iVar24 = iVar23 >> 0xc;
      iVar18 = func_8003D184((int) (*((short *) (param_2 + 0x396))));
      iVar20 = iVar22 >> 0xc;
      iVar16 = (-(iVar18 * iVar24)) >> 0xc;
    }
    do
    {
      {
        int *_m = (int *) (iVar27 + new_var16);
        register int _a asm("$12");
        register int _b asm("$13");
        register int _c asm("$14");
        _a = _m[0];
        _b = _m[1];
        asm volatile("ctc2 %0, $0" : : "r"(_a));
        asm volatile("ctc2 %0, $1" : : "r"(_b));
        _a = _m[2];
        _b = _m[3];
        _c = _m[4];
        asm volatile("ctc2 %0, $2" : : "r"(_a));
        asm volatile("ctc2 %0, $3" : : "r"(_b));
        asm volatile("ctc2 %0, $4" : : "r"(_c));
        _a = _m[5];
        _b = _m[6];
        asm volatile("ctc2 %0, $5" : : "r"(_a));
        _c = _m[7];
        asm volatile("ctc2 %0, $6" : : "r"(_b));
        asm volatile("ctc2 %0, $7" : : "r"(_c));
      }
    }
    while (0);
    sVar4 = (short) (iVar16 >> 1);
    if (((*((uint *) (param_2 + 0x2c8))) & 0x800) != 0)
    {
      *((uint *) (param_2 + 0x2c4)) = (*((uint *) (param_2 + 0x2c4))) | 1;
      asm volatile("mtc2 %0, $0" : : "r"(0xffe2));
      asm volatile("mtc2 %0, $1" : : "r"(0xffffffec));
      do
      {
        asm volatile("nop\nnop\n.word 0x1200484A");
        asm volatile("mfc2 %0, $25" : "=r"(param_3));
        asm volatile("mfc2 %0, $26" : "=r"(uVar15));
        asm volatile("mfc2 %0, $27" : "=r"(param_4));
      }
      while (0);
      iVar23 = ((int) param_4) - (iVar23 >> 0xd);
      iVar16 = param_3 - (iVar22 >> 0xd);
      bVar1 = *((byte *) (param_2 + bVar2));
      new_var18 = (param_2 + (((uint) bVar1) * 0x40)) + 0xd0;
      sVar6 = (short) iVar16;
      *((short *) ((param_2 + (((uint) bVar1) * 0x40)) + 0xc4)) = sVar6 + ((short) iVar17);
      sVar2 = (short) iVar23;
      *((undefined2 *) ((param_2 + (((uint) bVar1) * 0x40)) + 0xc6)) = uVar15;
      iVar28 = (uint) bVar1;
      *((short *) ((param_2 + (iVar28 * 0x40)) + 200)) = sVar2 + ((short) iVar16);
      iVar18 = -1;
      *((short *) ((param_2 + (iVar28 * 0x40)) + 0xcc)) = sVar6 - ((short) iVar17);
      *((undefined2 *) ((param_2 + (iVar28 * 0x40)) + 0xce)) = uVar15;
      *((short *) new_var18) = sVar2 - ((short) iVar16);
      *((char *) ((param_2 + (iVar28 * 0x40)) + 0xca)) = cVar25;
      if ((local_30 & 8) != 0)
      {
        *((byte *) ((param_2 + (iVar28 * 0x40)) + 0xcb)) = bVar26 | 1;
      }
      else
      {
        *((byte *) ((param_2 + (iVar28 * 0x40)) + 0xcb)) = bVar26;
      }
      iVar23 = iVar23 + iVar24;
      iVar16 = iVar16 + iVar20;
      uVar13 = (bVar1 - 1) & 7;
      new_var18 = (param_2 + (uVar13 * 0x40)) + 0xce;
      *((short *) ((param_2 + (uVar13 * 0x40)) + 0xc4)) = ((short) iVar16) + ((short) (iVar17 >> 1));
      *((undefined2 *) ((param_2 + (uVar13 * 0x40)) + 0xc6)) = uVar15;
      *((short *) ((param_2 + (uVar13 * 0x40)) + 200)) = ((short) iVar23) + ((short) (iVar16 >> 1));
      *((short *) ((param_2 + (uVar13 * 0x40)) - (-0xcc))) = ((short) iVar16) - ((short) (iVar17 >> 1));
      *((undefined2 *) new_var18) = uVar15;
      *((short *) ((param_2 + (uVar13 * 0x40)) + 0xd0)) = ((short) iVar23) - ((short) (iVar16 >> 1));
      *((char *) ((param_2 + (uVar13 * 0x40)) + 0xca)) = cVar25;
      new_var30 = 2 * uVar13;
      if ((local_30 & 8) != 0)
      {
        *((byte *) ((param_2 + (new_var30 * 32)) + 0xcb)) = bVar26 | 1;
      }
      else
      {
        *((byte *) ((param_2 + ((32 * uVar13) * 2)) + 0xcb)) = bVar26;
      }
      *((uint *) (param_2 + 0x2c4)) = (*((uint *) (param_2 + 0x2c4))) | 2;
      asm volatile("mtc2 %0, $0" : : "r"(0x1e));
      asm volatile("mtc2 %0, $1" : : "r"(0xffffffec));
      asm volatile("nop\nnop\n.word 0x1200484A");
      do
      {
        asm volatile("mfc2 %0, $25" : "=r"(iVar16));
        asm volatile("mfc2 %0, $26" : "=r"(uVar15));
        asm volatile("mfc2 %0, $27" : "=r"(iVar23));
      }
      while (0);
      uVar13 = (uVar13 + 1) & 7;
      sVar6 = (short) (iVar16 - (iVar20 >> 1));
      *((short *) ((param_2 + (uVar13 * 0x40)) - (-0xd4))) = (short) iVar17;
      iVar22 = iVar20;
      *((short *) ((param_2 + (uVar13 * 0x40)) - (-0xd4))) = sVar6 + (*((short *) ((param_2 + (uVar13 * 0x40)) - (-0xd4))));
      uVar14 = iVar24 >> 1;
      new_var2 = (short *) (iVar22 + 0xd8);
      sVar2 = (short) (iVar23 - uVar14);
      *new_var2 = sVar2 + ((short) iVar16);
      new_var22 = (param_2 + (uVar13 * 0x40)) + 0xdc;
      *((undefined2 *) ((param_2 + (uVar13 * 0x40)) + 0xd6)) = uVar15;
      new_var17 = param_2 + (uVar13 * 0x40);
      *((short *) new_var22) = sVar6 - ((short) iVar17);
      *((short *) (new_var17 + 0xe0)) = sVar2 - ((short) iVar16);
      *((char *) (new_var17 + 0xda)) = cVar25;
      new_var6 = iVar16 - (iVar22 >> 1);
      iVar23 = (iVar23 - (iVar24 >> 1)) + iVar24;
      if ((local_30 & 8) == 0)
      {
        *((byte *) (new_var17 + 0xdb)) = bVar26;
      }
      else
      {
        *((byte *) (new_var17 + 0xdb)) = bVar26 | 1;
        param_3 = (int) (iVar22 + new_var6);
      }
      param_4 = iVar23;
      uVar13 = (uVar13 - 1) & 7;
      iVar22 = param_2 + (uVar13 * 0x40);
      *((short *) (iVar22 + 0xd4)) = ((short) param_3) + ((short) (iVar17 >> 1));
      *((undefined2 *) (iVar22 + 0xd6)) = (sVar6 = uVar15);
      *((short *) (iVar22 + 0xd8)) = ((short) param_4) + ((short) (iVar16 >> 1));
      *((short *) (iVar22 + 0xdc)) = (short) ((int) ((undefined *) (param_3 - (iVar17 >> 1))));
      *((undefined2 *) (iVar22 + 0xde)) = sVar6;
      *((short *) (iVar22 + 0xe0)) = ((short) param_4) - ((short) (iVar16 >> 1));
      *((char *) (iVar22 + 0xda)) = cVar25;
      if ((local_30 & 8) != 0)
      {
        *((byte *) (iVar22 + 0xdb)) = bVar26 | 1;
      }
      else
      {
        *((undefined2 *) (iVar22 + 0xde)) = uVar15;
      }
    }
    *((byte *) (iVar22 + 0xdb)) = bVar26;
    new_var29 = (*((uint *) (param_2 + 0x2c8))) & 0x1000;
    if (new_var29 != 0)
    {
      *((uint *) (param_2 + 0x2c4)) |= 4;
      asm volatile("mtc2 %0, $0" : : "r"(0xffe2));
      asm volatile("mtc2 %0, $1" : : "r"(0x28));
      asm volatile("nop\nnop\n.word 0x1200484A");
      do
      {
        asm volatile("mfc2 %0, $25" : "=r"(param_3));
        asm volatile("mfc2 %0, $26" : "=r"(uVar15));
        asm volatile("mfc2 %0, $27" : "=r"(param_4));
      }
      while (0);
      bVar1 = *((byte *) (param_2 + bVar2));
      uVar19 = param_2 + (((uint) bVar1) * 0x40);
      iVar22 = uVar19;
      new_var26 = param_3;
      new_var26 = new_var26 - ((uVar19 = iVar20) >> 1);
      sVar6 = (short) new_var26;
      bVar2 = param_3 - (uVar19 >> 1);
      *((short *) (iVar22 + 0xe4)) = ((short) bVar2) + ((short) iVar17);
      sVar2 = (short) (((int) param_4) - (iVar24 >> 1));
      *((short *) (iVar22 + 0xe8)) = sVar2 + ((short) iVar16);
      new_var3 = iVar22;
      new_var4 = param_3 - (uVar19 >> 1);
      new_var4 = iVar24 >> 1;
      new_var3 = ((int) param_4) - new_var4;
      *((short *) (new_var3 + 0xec)) = sVar6 - ((short) iVar17);
      *((undefined2 *) (new_var3 + 0xe6)) = sVar6;
      *((undefined2 *) (new_var3 + 0xee)) = sVar6;
      *((short *) (new_var3 + 0xf0)) = sVar2 - ((short) iVar16);
      *((char *) (new_var3 + 0xea)) = cVar25;
      if ((local_30 & 8) != 0)
      {
        *((byte *) (new_var3 + 0xeb)) = bVar26 | 1;
      }
      else
      {
        *((byte *) (new_var3 + 0xeb)) = bVar26;
      }
      iVar20 = new_var3 + iVar24;
      iVar16 = iVar20;
      iVar23 = iVar20;
      iVar23 = new_var4 + iVar23;
      uVar13 = (bVar1 - 1) & 7;
      iVar22 = param_2 + (uVar13 * 0x40);
      *((short *) (iVar22 + 0xe4)) = ((short) iVar23) + ((short) (iVar17 >> 1));
      *((undefined2 *) (iVar22 + 0xe6)) = uVar15;
      *((short *) (iVar22 + 0xe8)) = ((short) iVar16) + ((short) (iVar16 >> 1));
      *((short *) (iVar22 + 0xec)) = ((short) iVar23) - ((short) (iVar17 >> 1));
      {
        *((undefined2 *) (iVar22 + 0xee)) = uVar15;
        *((short *) (iVar22 + 0xf0)) = ((short) iVar16) - (iVar16 >> 1);
        *((char *) (iVar22 + 0xea)) = cVar25;
        if ((local_30 & 8) != 0)
        {
          *((byte *) (iVar22 + 0xeb)) = (new_var7 = bVar26 | 1);
        }
        else
        {
          *((byte *) (iVar22 + 0xeb)) = bVar26;
          iVar20 = iVar20 + iVar20;
        }
        *((uint *) (param_2 + 0x2c4)) = (*((uint *) (param_2 + 0x2c4))) | 8;
        asm volatile("mtc2 %0, $0" : : "r"(0x1e));
        asm volatile("mtc2 %0, $1" : : "r"(0x28));
        asm volatile("nop\nnop\n.word 0x1200484A");
        do
        {
          asm volatile("mfc2 %0, $25" : "=r"(iVar23));
          asm volatile("mfc2 %0, $26" : "=r"(uVar15));
          asm volatile("mfc2 %0, $27" : "=r"(iVar16));
        }
        while (0);
        uVar13 = (uVar13 + 1) & 7;
        new_var19 = param_2 + ((uVar13 * 0x40) & 0xFFFF);
        sVar6 = sVar6 - ((short) (iVar24 >> 1));
        *((short *) (new_var19 + 0xf4)) = sVar2 + ((short) iVar17);
        *((short *) (new_var19 + 0xf8)) = sVar6 + ((short) iVar16);
        *((byte *) (new_var19 + 0xfb)) = bVar26;
        *((short *) (new_var19 + 0xfc)) = sVar2 - ((short) iVar17);
        *((undefined2 *) (new_var19 + 0xf6)) = uVar15;
        *((undefined2 *) (new_var19 + 0xfe)) = sVar6;
        *((short *) (new_var19 + 0x100)) = sVar6 - ((short) iVar16);
        *((char *) (new_var19 + 0xfa)) = cVar25;
        iVar20 = (iVar24 = iVar23);
      }
      iVar20 = iVar20 - (iVar20 >> 1);
      if ((local_30 & 8) == 0)
      {
        *((byte *) (new_var19 + 0xfb)) = bVar26;
      }
      else
      {
        *((byte *) (new_var19 + 0xfb)) = bVar26 | 1;
        sVar6 = sVar6 + ((short) iVar24);
      }
      iVar11 = 0xfc;
      uVar19 = param_2;
      uVar19 = uVar19 + ((uVar13 = (uVar13 - 1) & 7) * 0x40);
      iVar22 = uVar19;
      *((short *) (iVar22 + 0xf4)) = ((short) iVar20) + ((short) (iVar17 >> 1));
      *((undefined2 *) (iVar22 + 0xf6)) = uVar15;
      *((short *) (iVar22 + 0xf8)) = sVar6 + ((short) (iVar16 >> 1));
      *((short *) (iVar22 + iVar11)) = (short) ((int) ((undefined *) (iVar20 - (iVar17 >> 1))));
      *((undefined2 *) (iVar22 + 0xfe)) = uVar15;
      *((short *) (iVar22 + 0x100)) = sVar6 - ((short) (iVar16 >> 1));
      *((char *) (iVar22 + 0xfa)) = cVar25;
      if ((local_30 & 8) != 0)
      {
        *((byte *) (iVar22 + 0xfb)) = bVar26 | 1;
      }
      else
      {
      }
      *((byte *) (iVar22 + 0xfb)) = bVar26;
    }
  }
  if ((*((short *) (param_1 + 0x44))) == 0x3f)
  {
    if (((*((uint *) (D_8008D2AC + 0x1cec))) & 3) != ((*((byte *) (param_2 + 0x4a))) & 3))
    {
      LAB_8005a858:
      if ((*((char *) (param_2 + 0x381))) != '\0')
      {
        do
        {
        }
        while (0);
        goto LAB_8005a868;
      }

      goto LAB_8005a9d8;
    }
  }
  else
  {
    new_var = (byte *) (param_2 + 0x4a);
    if (((*((char *) (param_2 + 0x4fe))) == '\x02') || (((*((uint *) (D_8008D2AC + 0x1cec))) & 3, (1 < ((uint) ((byte) D_8008D2AC[0x1ca8]))) && (((((uint) ((byte) D_8008D2AC[0x1ca8])) != 2) || (((uint) ((byte) D_8008D2AC[0x1ca8]), ((*((uint *) (D_8008D2AC + 0x1cec))) & 1) != ((uint) (*new_var))))) && ((uVar13 = (uint) (*((byte *) (param_2 + 0x4a))), ((*((uint *) (D_8008D2AC + 0x1cec))) & 3) != ((unsigned short) uVar13)))))))
    {
      goto LAB_8005a9d8;
    }
    puVar12 = D_8008D2AC;
    if ((*((char *) (param_2 + 0x381))) == '\0')
    {
      new_var11 = (uint) (*((short *) (param_2 + 0x3dc)));
      uVar13 = new_var11;
      if ((((int) uVar13) < 0x81) || (((int) (((*((byte *) (param_2 + 0x477))) + 2) * 0x20)) < ((int) uVar13)))
      {
        puVar12 = *((undefined **) (param_1 + 0x14));
        if (func_80042394(puVar12, 0x2c) != 0)
        {
          goto LAB_8005a9d8;
        }
      }
      goto LAB_8005a858;
    }
    LAB_8005a868:
    *((char *) (param_2 + 0x381)) = (*((char *) (param_2 + 0x381))) - 1;

  }
  {
    int *_m = (int *) (iVar27 + new_var16);
    register int _a asm("$12");
    register int _b asm("$13");
    register int _c asm("$14");
    _a = _m[0];
    _b = _m[1];
    asm volatile("ctc2 %0, $0" : : "r"(_a));
    asm volatile("ctc2 %0, $1" : : "r"(_b));
    _a = _m[2];
    _b = _m[3];
    _c = _m[4];
    asm volatile("ctc2 %0, $2" : : "r"(_a));
    asm volatile("ctc2 %0, $3" : : "r"(_b));
    asm volatile("ctc2 %0, $4" : : "r"(_c));
  }
  asm volatile("mtc2 %0, $0" : : "r"(0x4000000));
  asm volatile("mtc2 %0, $1" : : "r"(0xfffffc00));
  asm volatile("nop\nnop\n.word 0x1260484A");
  do
  {
    asm volatile("mfc2 %0, $25" : "=r"(pMVar10));
    asm volatile("mfc2 %0, $26" : "=r"(uVar13));
    asm volatile("mfc2 %0, $27" : "=r"(puVar12));
  }
  while (0);
  puVar21 = (undefined *) (((char *) auStack_68) - 0x18);
  *((MATRIX **) (puVar21 + 0x28)) = pMVar10;
  *((uint *) (puVar21 + 0x2c)) = uVar13;
  *((undefined **) (puVar21 + 0x30)) = puVar12;
  sVar2 = 1;
  {
    int _vxy0 = ((((*((short *) (iVar27 + 0x1c))) * 9) >> 3) & 0xffffU) | ((((*((short *) (iVar27 + 0x1e))) * 7) >> sVar2) << 0x10);
    int _vz0;
    asm volatile("mtc2 %0, $0" : : "r"(_vxy0));
    _vz0 = ((*((short *) (iVar27 + 0x20))) * (-0x38)) >> 4;
    asm volatile("mtc2 %0, $1" : : "r"(_vz0));
  }
  asm volatile("nop\nnop\n.word 0x1260484A");
  r0 = (VECTOR *) (puVar21 + 0x18);
  asm volatile("swc2 $25, 0(%0)\n swc2 $26, 4(%0)\n swc2 $27, 8(%0)\n" : : "r"(r0) : "memory");
  puVar21 = puVar21 + 0x28;
  func_80059100(param_2, r0, puVar21);
  {
    int _vxy0 = ((((*((short *) (iVar27 + 0x1c))) * (-0x12)) >> 4) & 0xffffU) | ((((*((short *) (iVar27 + 0x1e))) * 7) >> 1) << 0x10);
    int _vz0;
    asm volatile("mtc2 %0, $0" : : "r"(_vxy0));
    _vz0 = ((*((short *) (iVar27 + 0x20))) * (-0x38)) >> 4;
    asm volatile("mtc2 %0, $1" : : "r"(_vz0));
  }
  asm volatile("nop\nnop\n.word 0x1260484A");
  asm volatile("swc2 $25, 0(%0)\n swc2 $26, 4(%0)\n swc2 $27, 8(%0)\n" : : "r"(r0) : "memory");
  func_80059100(param_2, r0, puVar21);
  LAB_8005a9d8:
  if ((*((short *) (param_2 + 0x402))) != 0)
  {
    *((undefined2 *) (param_2 + 0x508)) = 0x1000;
    *((undefined2 *) ((*((int *) (param_2 + 0x1c))) + 0x22)) = 0x1000;
  }

  new_var20 = (local_30 = 0x22);
  local_30 = param_2;
  if ((*((int *) (param_2 + 0x28))) != 0)
  {
    *((undefined2 *) (iVar27 + new_var20)) = 0x1000;
  }
  if (((*((char *) (param_2 + 0x376))) != '\0') && ((*((char *) (param_2 + 0x376))) != '\x02'))
  {
    *((uint *) (local_30 + 0x2c8)) = (*((uint *) (param_2 + 0x2c8))) & 0xfff7ffff;
  }
  iVar11 = (*((byte *) (param_2 + 0x376))) - 4;
  new_var7 = iVar11;
  new_var14 = ((unsigned int) new_var7 < 2) || (((*((uint *) (param_2 + 0x2c8))) & 1) != 0);
  new_var11 = new_var14;
  if (new_var11)
  {
    func_800263FC(param_2, 0x27, 0);
    if ((*((short *) (param_2 + 0x3d4))) == 0)
    {
      return;
    }
    if (((*((uint *) (D_8008D2AC + 0x1cec))) & 3) == 0)
    {
      uVar19 = 0x27;
    }
    else
    {
      uVar19 = 0xf0;
      sVar2 = iVar23 - (iVar20 >> 1);
    }
    uVar14 = new_var28;
  }
  else
  {
    if ((*((short *) (param_2 + 0x3fc))) < 0x80)
    {
      new_var32 = 0x4b;
      uVar19 = 0x12;
      if (((*((signed char *) (param_2 + 0x4b))) < '\0') || ((uVar19, '\0' < (*((signed char *) (param_2 + new_var32))))))
      {
        func_800263A0(param_2, (int) local_30, 0x20);
      }
    }
    uVar19 = 0;
    uVar14 = (undefined4) uVar19;
  }
  func_800263FC(param_2, (int) uVar19, (int) uVar14);
}
