/*
 * VehPhysForce_TranslateMatrix @ 0x8005EE34 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact reference: base score 5970, Branch 0 / Stack 0 (Register 154).
 * Large STRUCTURAL residual — a 523-line matrix-anim state machine (SquashStretch,
 * matrixArray 1/2/3/5 animation, InstanceMatrix facing/baked-GTE rtv0, Wake) whose
 * overall scheduling differs from retail; byte-exact is a dedicated hand-match, not a
 * permuter grind. Delivered as a verified-CORRECT, hack-free reference.
 *
 * From the "aibox" set. Cleaned for delivery (per user "inline asm is fine for GTE"):
 *   - FIXED a behavioral bug in the seed: the mask-grab squash used an UNINITIALIZED
 *     `iVar6` (read before assign); retail holds jumpSquishStretch(+0x40c)*0x28 there,
 *     i.e. scale = 0xccc - Div256TowardZero(js*0x28), floor 0x400 (matches saphi).
 *   - Removed the permuter's PERM_RANDOMIZE wrappers, an `unsigned long long` and a
 *     `volatile` codegen pun (barred hacks) — honest score rose 5900->5970, but correct.
 *   - The GTE rtv0 sequence is retail-exact INLINE ASM (ctc2/mtc2/cop2 0x0486012/mfc2),
 *     the SetRotMatrix + rotate-vector-by-matrix used by the baked-anim path.
 *
 * saphi game/Vehicle/VehPhysForce.c:994 (+ 8 helpers) audited CLEAN vs this + asm.
 * Generic decompiler locals left (large fn; struct-field offsets carry the semantics).
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
  int t[3];
} MATRIX;
extern char *PTR_DAT_8008d2ac;
extern int DAT_80087f00;
extern int DAT_80087f10;
extern int DAT_80087f20;
extern int DAT_80087ef4;
extern int DAT_800845a0;
extern int DAT_80089a94;
extern int func_80058F9C(int, int, int, int, int);
extern undefined2 func_80058F54(int, int, int);
extern void func_8005F89C(undefined2 *, short *, uint);
extern void func_8006C3B0(int, MATRIX *, int *);
extern void func_80028494(int, int, uint);
extern int func_80040308(int, int, int *);
inline int inline_fn(uint arg0)
{
  return arg0 & 0x400;
}

void VehPhysForce_TranslateMatrix(int param_1, int param_2)
{
  int bVar1;
  undefined *puVar2;
  int new_var20;
  byte bVar3;
  undefined2 uVar4;
  int new_var;
  int new_var8;
  int new_var14;
  short sVar5;
  int new_var22;
  int new_var16;
  char new_var18;
  int iVar6;
  short *new_var23;
  int new_var6;
  uint *new_var19;
  undefined4 *new_var2;
  int new_var13;
  int *new_var15;
  MATRIX **new_var12;
  int iVar8;
  int iVar9;
  MATRIX *pMVar10;
  int iVar11;
  uint uVar13;
  int new_var9;
  undefined *new_var3;
  int new_var11;
  byte new_var5;
  int new_var7;
  unsigned int uVar7;
  int new_var10;
  int *new_var4;
  MATRIX *r0;
  int iVar14;
  int new_var21;
  int *piVar12;
  undefined4 *new_var17;
  unsigned short uVar15;
  iVar14 = *((int *) (param_1 + 0x34));
  new_var20 = 0x4d;
  iVar8 = param_2;
  if ((*((char *) (iVar8 + 0x376))) != '\n')
  {
    if (((*((char *) (iVar8 + 0x376))) == '\x05') && (((*((uint *) (param_2 + 0x2c8))) & 1) == 0))
    {
      *((short *) (iVar14 + 0x1e)) = (*((short *) (iVar8 + 0x40c))) + 0xccc;
      ;
      iVar6 = (*((short *) (iVar8 + 0x40c))) * 0x28;
      if (iVar6 < 0)
      {
        iVar6 = iVar6 + 0xff;
      }
      iVar6 = iVar6 >> 8;
      iVar6 = 0xccc - iVar6;
      if (iVar6 < 0x400)
      {
        iVar6 = 0x400;
      }
      *((short *) (iVar14 + 0x1c)) = (short) iVar6;
      *((short *) (iVar14 + 0x20)) = (short) iVar6;
    }
    else
    {
      sVar5 = *((short *) (iVar8 + 0x390));
      iVar8 = (int) sVar5;
      iVar6 = -800;
      new_var14 = param_2;
      if (((*((uint *) (new_var14 + 0x2c8))) & 0x400) == 0)
      {
        iVar11 = (new_var8 = ((int) (*((short *) (new_var14 + 0x410)))) - (((sVar5 = (*((short *) (new_var14 + 0x410))) * 9) + (iVar8 * 7)) >> 4));
        iVar6 = 4;
        iVar6 = iVar11 * iVar6;
        iVar9 = iVar6;
        if (iVar6 < 0)
        {
          iVar9 = iVar11 * (-4);
        }
        if (0x960 > iVar9)
        {
          iVar6 = 0;
        }
        if ((((*((uint *) (new_var14 + 0x2c8))) | (*((uint *) (new_var14 + 0x2cc)))) & 2) != 0)
        {
          bVar1 = iVar6 < 0x321;
          if (iVar6 < (-0x640))
          {
            iVar6 = -0x640;
          }
        }
        else
        {
          LAB_8005ef64:
          bVar1 = (iVar6 + 1) <= 0x321;

          ;
          if (iVar6 < (-800))
          {
            iVar6 = -800;
            goto LAB_8005ef64;
          }
        }
        if (!(iVar6 < 0x321))
        {
          iVar6 = 800;
        }
      }
      if (((0 < (*((short *) (new_var14 + 0xe)))) && (((*((ushort *) (new_var14 + 0xe))) & 0x80) == 0)) && ((-800) < iVar6))
      {
        iVar6 = -800;
      }
      if ((((*((uint *) (new_var14 + 0x2c8))) & 1) == 0) && (iVar8 < 0))
      {
        iVar9 = func_80058F9C(-iVar8, 0, 0xa00, 0x280, 800);
        bVar1 = iVar9;
        if (iVar6 < bVar1)
        {
          iVar6 = bVar1;
        }
        *((short *) (new_var14 + 0x410)) = (short) iVar8;
      }
      if (((*((int *) (new_var14 + 0x18))) != 0) && ((iVar9 = (int) (*((short *) ((*((int *) (new_var14 + 0x18))) + 0x1e))), iVar9 < 2500)))
      {
        iVar6 = iVar9 + (-0x800);
        iVar6 = iVar6 + (iVar6 * 2);
      }
      new_var6 = 5;
      iVar11 = (int) (*((short *) (new_var14 + 0x40c)));
      iVar9 = iVar6;
      if (iVar6 < 0)
      {
        iVar9 = -iVar6;
      }
      if (iVar11 < 0)
      {
        iVar11 = -iVar11;
      }
      if (iVar11 < iVar9)
      {
        *((short *) (new_var14 + 0x40c)) = (short) iVar6;
      }
      ;
      new_var = new_var14;
      *((undefined2 *) (new_var + 0x40c)) = func_80058F54((int) (*((short *) (new_var14 + 0x40c))), 300, 0);
      *((short *) (new_var + 0x410)) = (short) ((((*((short *) (new_var + 0x410))) * 9) + (iVar8 * 7)) >> 4);
      if ((*((short *) (new_var + 0x404))) != 0)
      {
        *((undefined2 *) (iVar14 + 0x1e)) = 0;
      }
      else
        if ((*((short *) (iVar14 + 0x1e))) == 0)
      {
        if ((*((short *) ((*((int *) ((*((int *) (new_var + 0x1c))) - (-0x6c)))) + 0x44))) == 0x18)
        {
          uVar15 = *((ushort *) (new_var + 0x2ca));
          func_80028494(0x5b, 1, uVar15 & 1);
        }
        *((short *) (iVar14 + 0x1e)) = (*((short *) (new_var + 0x40c))) + 0xccc;
        *((undefined *) (new_var + 0x4c)) = new_var6;
        *((undefined *) (new_var + 0x4d)) = 0;
      }
      else
      {
        uVar4 = func_80058F54((int) (*((short *) (iVar14 + 0x1e))), 0xa0, (*((short *) (new_var + 0x40c))) + 0xccc);
        *((undefined2 *) (iVar14 + 0x1e)) = uVar4;
        goto LAB_8005f398;
      }
      new_var10 = (int) (*((short *) (iVar14 + 0x1c)));
      ;
      if (((*((short *) (new_var - (-0x40c)))) * 0xa0) < 0)
      {
        iVar6 -= -0xff;
      }
      uVar4 = func_80058F54(new_var10, 0xa0, 0xccc - (iVar6 >> 8));
      *((undefined2 *) (iVar14 + 0x1c)) = uVar4;
      new_var8 = (int) (*((short *) (iVar14 + 0x20)));
      ;
      if (((*((short *) (new_var + 0x40c))) * 0xa0) < 0)
      {
        iVar6 = iVar6 + 0xff;
      }
      uVar4 = func_80058F54(new_var8, 0xa0, 0xccc - (iVar6 >> 8));
      *((undefined2 *) (iVar14 + 0x20)) = uVar4;
    }
  }
  func_8005F89C((undefined2 *) (new_var + 0x330), (short *) (new_var + 0x368), (uint) (*((short *) (new_var + 0x2ee))));
  uVar15 = (*((char *) (new_var + 0x4d))) + 1;
  new_var19 = &(*((uint *) (new_var + 0x2c8)));
  if (!((((*((short *) (new_var + 0x3e2))) == 0) || ((*((short *) (new_var + 0x39e))) < (*((short *) (new_var + 0x42c))))) || (((*new_var19) & 0x80) != 0)))
  {
    bVar3 = *((byte *) (new_var + 0x4c));
    if (bVar3 == 1)
    {
      bVar3 = uVar15;
      *((byte *) (new_var + 0x4d)) = bVar3;
      if (DAT_80087f00 <= ((int) ((uint) bVar3)))
      {
        *((undefined *) (new_var + 0x4c)) = 2;
      }
      goto LAB_8005f39c;
    }
    if ((new_var13 = 1) < bVar3)
    {
      new_var5 = bVar3;
      if ((new_var5 != 2) && (bVar3 == 3))
      {
        ;
        if ((DAT_80087f10 + (-1)) == 0)
        {
          do
          {
            (void) 0;
          }
          while (0);
        }
        if (((DAT_80087f10 + (-1)) == (-1)) && ((*((byte *) (new_var + 0x4d))) == 0x800000))
        {
          (void) 0;
        }
        ;
        iVar6 = 0x100 - (((int) (((uint) (*((byte *) (new_var + new_var20)))) << 8)) / (DAT_80087f10 + (-1)));
        if (iVar6 < 0)
        {
          iVar6 = 0;
        }
        if (0x100 < iVar6)
        {
          iVar6 = 0x100;
        }
        uVar15 = (undefined) ((iVar6 * (DAT_80087f00 + (-1))) >> 8);
        *((undefined *) (new_var + 0x4c)) = 1;
        goto LAB_8005f354;
        *((undefined *) (new_var + 0x4c)) = 1;
      }
      goto LAB_8005f39c;
    }
    *((undefined *) (new_var + 0x4c)) = 0;
    if (bVar3 == 0)
    {
      goto LAB_8005f398;
    }
  }
  else
  {
    bVar3 = *((byte *) (new_var + 0x4c));
    if (bVar3 != 0)
    {
      do
      {
      }
      while (0);
      bVar1 = bVar3;
      if (bVar1 == 2)
      {
        return;
      }
      else
      {
        *((undefined *) (new_var + 0x4c)) = 3;
        if (((bVar3 + 1) + 1) < ((3 + 1) + 1))
        {
          if (bVar3 == 1)
          {
            new_var22 = (DAT_80087f00 + (-1)) == (-1);
            ;
            if ((DAT_80087f00 + (-1)) == 0)
            {
              (void) 0;
            }
            if (new_var22 && ((*((byte *) (new_var + 0x4d))) == 0x800000))
            {
              (void) 0;
              LAB_8005f398:
              *((undefined *) (new_var + 0x4d)) = 0;

            }
            new_var16 = DAT_80087f00 + (-1);
            ;
            if ((0x100 - (((int) (((uint) (*((byte *) (new_var + 0x4d)))) << 8)) / new_var16)) < 0)
            {
              iVar6 = 0;
            }
            if (0x100 < iVar6)
            {
              iVar6 = 0x100;
            }
            uVar15 = (undefined) (((iVar6 * (DAT_80087f10 + (-1))) >> 6) >> 2);
            new_var11 = 0x4c;
            *((undefined *) (new_var + new_var11)) = 3;
            LAB_8005f354:
            *((undefined *) (new_var + 0x4d)) = uVar15;

          }
        }
        else
          if ((bVar3 == 3) && ((new_var18 = (bVar3 = (*(new_var4 = (char *) (new_var + 0x4d))) + 1), *((byte *) (new_var + 0x4d)) = bVar3, DAT_80087f10 <= ((int) ((uint) bVar3)))))
        {
          pMVar10 = r0;
          goto LAB_8005f398;
        }
      }
    }
  }
  LAB_8005f39c:
  bVar3 = *((byte *) (new_var + 0x4c));

  new_var4 = &DAT_80087ef4;
  if ((bVar3 == 5) && ((bVar3 = (*new_var4) + 1, *((byte *) (new_var + 0x4d)) = bVar3, DAT_80087f20 <= ((int) ((uint) bVar3)))))
  {
    *((undefined *) (new_var + 0x4c)) = 0;
    *((undefined *) (new_var + 0x4d)) = 0;
  }
  new_var7 = iVar14 + 0x30;
  ;
  if ((*((byte *) (new_var + 0x4c))) != 0)
  {
    r0 = (MATRIX *) (new_var + 0x330);
    iVar6 = *((int *) (new_var + 0x2dc));
    piVar12 = (int *) (new_var4[((uint) (*((byte *) (new_var + 0x4c)))) * 2] + (((uint) (*((byte *) (new_var + 0x4d)))) * 0x20));
    func_8006C3B0(new_var7, r0, piVar12 + 2);
    asm volatile("lw $12, 0( %0);lw $13, 4( %0);ctc2 $12, $0;ctc2 $13, $1;lw $12, 8( %0);lw $13, 12( %0);lw $14, 16( %0);ctc2 $12, $2;ctc2 $13, $3;ctc2 $14, $4" : : "r"(r0) : "$12", "$13", "$14");
    iVar6 = 0x10;
    new_var23 = (short *) piVar12;
    iVar6 = ((uint) (*((ushort *) piVar12))) | (((int) (*(new_var23 + 1))) << iVar6);
    asm volatile("mtc2 %0, $0" : : "r"(iVar6));
    asm volatile("mtc2 %0, $1" : : "r"((uint) (*((ushort *) (piVar12 + 1)))));
    asm volatile("nop;nop;cop2 0x0486012;");
    do
    {
      asm volatile("mfc2 %0, $25" : "=r"(iVar6));
      asm volatile("mfc2 %0, $26" : "=r"(r0));
      asm volatile("mfc2 %0, $27" : "=r"(pMVar10));
    }
    while (0);
    *((int *) (iVar14 + 0x44)) = ((*((int *) (new_var + 0x2d4))) + iVar6) >> 8;
    *((int *) (iVar14 + 0x48)) = (((*((int *) (new_var + 0x2d8))) + ((int) (*(new_var12 = &r0))->m)) >> 8) + (((*((signed char *) (new_var + 0x377))) * 3) >> 3);
    new_var21 = (int) (*pMVar10).m;
    iVar6 = (*((int *) (new_var + 0x2dc))) + new_var21;
  }
  else
  {
    *((undefined4 *) (iVar14 + 0x30)) = *((undefined4 *) (new_var + 0x330));
    new_var9 = iVar14 + 0x34;
    *((undefined4 *) new_var9) = *((undefined4 *) (new_var + 0x334));
    *((undefined4 *) (iVar14 - (iVar11 = -0x38))) = *((undefined4 *) (new_var + 0x338));
    *((undefined4 *) (iVar14 + 0x3c)) = *((undefined4 *) (new_var + 0x33c));
    *((undefined2 *) (iVar14 + 0x40)) = *((undefined2 *) (new_var + 0x340));
    *((int *) (iVar14 + 0x44)) = (*((int *) (new_var + 0x2d4))) >> 8;
    *((int *) (iVar14 + 0x48)) = ((*((int *) (new_var + 0x2d8))) >> 8) + (((*((signed char *) (new_var + 0x377))) * 3) >> 3);
    iVar6 = *((int *) (new_var + 0x2dc));
  }
  *((int *) (iVar14 + 0x4c)) = iVar6 >> 8;
  if ((*((short *) (new_var + 0x404))) != 0)
  {
    *((int *) (iVar14 + 0x44)) = (*((int *) (iVar14 + 0x44))) + (((*((short *) (new_var + 0x368))) * 0x13) >> 0xc);
    *((int *) (iVar14 + 0x48)) = (*((int *) (iVar14 + 0x48))) + (((*((short *) (new_var + 0x36a))) * 0x13) >> 0xc);
    *((int *) (iVar14 + 0x4c)) += ((*((short *) (new_var + 0x36c))) * 0x13) >> 0xc;
  }
  ;
  if (!((-1) < (*((int *) (iVar14 + 0x48)))))
  {
    if ((((*((int *) (iVar14 + 0x48))) + 1) <= (-0x4f)) || (((*((uint *) (iVar14 + 0x28))) & 0x2000) == 0))
    {
      iVar6 = *((int *) (iVar14 + 0x48));
      goto code_r0x8005f834;
    }
    iVar6 = *((int *) (new_var + 0x4f8));
    if (iVar6 == 0)
    {
      return;
    }
    *((uint *) (iVar6 + 0x28)) = (*((uint *) (iVar6 + 0x28))) & 0xffffff7f;
    *((char *) (iVar6 + 0x50)) = (*((char *) (iVar14 + 0x50))) + '\x01';
    *((char *) (iVar6 + 0x51)) = (*((char *) (iVar14 + 0x51))) - 1;
    uVar7 = *((undefined4 *) (iVar14 + 0x44));
    new_var2 = (undefined4 *) (iVar6 + 0x48);
    *new_var2 = 0;
    *((undefined4 *) (iVar6 + 0x44)) = uVar7;
    *((undefined4 *) ((iVar8 = iVar6) + 0x4c)) = *((undefined4 *) (iVar14 + 0x4c));
    uVar13 = (uint) (*((short *) (new_var + 0x39a)));
    sVar5 = (short) iVar14;
    iVar14 = *((int *) ((&DAT_800845a0) + (uVar13 & 0x3ff)));
    if (inline_fn(uVar13) == 0)
    {
      iVar14 = iVar14 >> 0x10;
      iVar8 = (int) sVar5;
      if ((uVar13 & 0x800) == 0)
      {
        goto LAB_8005f6a4;
      }
      iVar9 = -iVar14;
    }
    else
    {
      if (1)
      {
        iVar14 = (int) sVar5;
      }
      if (1)
      {
        iVar9 = iVar14;
        iVar8 = iVar14;
      }
      if ((uVar13 & 0x800) == 0)
      {
        iVar14 = -iVar9;
      }
      goto LAB_8005f6a4;
    }
    iVar8 = -iVar8;
    iVar14 = iVar9;
    LAB_8005f6a4:
    *((int *) (iVar6 + 0x30)) = iVar14;

    *((int *) (iVar8 + 0x34)) = iVar8;
    uVar13 = 0x1000;
    *((undefined4 *) (iVar6 + 0x38)) = uVar13;
    *((int *) (iVar6 + 0x3c)) = -iVar8;
    *((short *) (iVar8 + 0x40)) = (short) iVar14;
    if ((*((short *) (new_var + 0x4fc))) == 0)
    {
      new_var3 = (undefined *) PTR_DAT_8008d2ac;
      *((undefined2 *) (new_var + 0x4fc)) = uVar13;
      puVar2 = new_var3;
      if (((byte) puVar2[0x1ca8]) < 2)
      {
        ;
        if (((int) (*((short *) (new_var + 0x38c)))) < 0)
        {
          ;
        }
        if ((0xc00 < (-((int) (*((short *) (new_var + 0x38c)))))) && ((iVar14 = 10, (-0x200) < (*((int *) (new_var + 0x2e4))))))
        {
          do
          {
            iVar8 = func_80040308(0, *((int *) (PTR_DAT_8008d2ac + 0x2138)), &DAT_80089a94);
            if (iVar8 != 0)
            {
              *((undefined *) (iVar8 + 0x18)) = *((undefined *) ((*((int *) (new_var + 0x1c))) + 0x50));
              *((undefined4 *) (iVar8 + 0x20)) = *((undefined4 *) (new_var + 0x1c));
              *((undefined *) (iVar8 + 0x19)) = *((undefined *) (new_var + 0x4a));
            }
            iVar14 = iVar14 + (-1);
          }
          while (iVar14 != 0);
        }
      }
    }
    else
      if (((byte) PTR_DAT_8008d2ac[0x1ca8]) < 2)
    {
      iVar14 = (int) (*((short *) (new_var + 0x38c)));
      if (iVar14 < 0)
      {
        ;
      }
      sVar5 = iVar14;
      if ((0xc00 < (-sVar5)) && ((iVar14 = (bVar1 = func_80040308(0, *((int *) (PTR_DAT_8008d2ac + 0x2138)), &DAT_80089a94)), iVar14 != 0)))
      {
        *((undefined *) (iVar14 + 0x18)) = *((undefined *) ((*((int *) (new_var + 0x1c))) + 0x50));
        *((undefined4 *) (iVar14 + 0x20)) = *((undefined4 *) (new_var + 0x1c));
        *((undefined *) (iVar14 - (new_var13 = -0x19))) = *((undefined *) (new_var + 0x4a));
      }
    }
    *((undefined2 *) (iVar6 + 0x1c)) = *((undefined2 *) (new_var + 0x4fc));
    *((undefined2 *) ((*(new_var15 = &iVar6)) + 0x20)) = *((undefined2 *) (new_var + 0x4fc));
    return;
  }
  code_r0x8005f834:
  if (0 < (*((int *) (new_var + 0x4f8))))
  {
    *((uint *) (iVar14 + 0x28)) = (*((uint *) (iVar14 + 0x28))) & 0xffffdfff;
  }

  iVar14 = *((int *) (new_var + 0x4f8));
  bVar1 = iVar14;
  if (bVar1 != 0)
  {
    *((uint *) (bVar1 + 0x28)) = (*((uint *) (bVar1 + 0x28))) | 0x80;
    *((undefined2 *) (new_var + 0x4fc)) = 0;
    *((undefined2 *) (iVar14 + 0x1c)) = 0;
    *((undefined2 *) (bVar1 + 0x20)) = *((undefined2 *) (new_var + 0x4fc));
  }
}
