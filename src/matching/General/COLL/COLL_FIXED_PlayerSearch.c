/*
 * COLL_FIXED_PlayerSearch @ 0x8001D944 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact NATURAL reference: base score 7885, Branch 0 / Stack 0, Register 277
 * — a genuine gcc-2.8.1 whole-function register-allocation wall on this 616-line GTE
 * player-collision AABB-search routine. The GTE cop2 sequence is written as inline asm
 * (retail is hand-scheduled; inline asm permitted for GTE code); all barred codegen
 * hacks removed (PERM_RANDOMIZE ×2 unwrapped, volatile locals/extern + long-long return
 * puns dropped) — honest score rose from the hack-forced 6560 but the code is natural
 * and correct. Byte-exact would need whole-function register pinning (out of scope).
 * Generic decompiler local names left (large function); struct-offset accesses documented
 * by the sibling COLL_FIXED_BotsSearch delivery + saphi game/COLL.c (audited CLEAN).
 * The search struct is built in scratchpad @0x1F800108 (fixed-address ptr, gcc-expressible).
 * vs SCUS_944.26.exe; from aibox staging.
 */
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
extern void func_8001F41C(unsigned int, void *);
extern void func_8001EBEC(int, void *, void *, void *);
extern uint func_80059070(int, int);
extern uint func_80057C68(uint);
extern uint func_80058F9C(int, int, int, int, int);
extern void func_800284D0(uint, int, uint);
extern void func_80028468(int, int);
extern int func_800671B0(int, int);
extern int func_8007173C(int, int);
extern void func_8001F5F0(void);
extern byte *D_8008D2AC;
extern uint D_8008D000;
extern unsigned char D_800845A0;
void COLL_FIXED_PlayerSearch(int param_1, int param_2)
{
  char cVar1;
  int new_var11;
  char *mybase = (char *) 0x1F800108;
  short new_var3;
  int new_var12;
  unsigned char new_var15;
  ushort uVar2;
  byte *puVar3;
  byte bVar4;
  short sVar5;
  char *new_var;
  uint uVar6;
  uint uVar7;
  ushort *new_var8;
  long new_var7;
  int lVar8;
  int iVar9;
  uint uVar10;
  short *new_var6;
  int iVar11;
  uint uVar12;
  int iVar13;
  int new_var2;
  short new_var16;
  int iVar14;
  int new_var14;
  int *new_var5;
  int iVar15;
  short new_var13;
  int iVar13_sgn;
  unsigned char *new_var10;
  int new_var4;
  int iVar9_sgn;
  int iVar13_y;
  ushort *new_var9;
  int iVar9_y;
  int iVar9_s;
  int local_arr[3];

  *((uint *) (param_2 + 0x2c8)) = (*((uint *) (param_2 + 0x2c8))) & 0xfffeffff;
  new_var = mybase;
  *((short *) (new_var + 0x10)) = (short) ((*((int *) (param_2 + 0x2d4))) >> 8);
  iVar13_y = ((*((int *) (param_2 + 0x2d8))) >> 8) + 0x80;
  *((short *) (new_var + 0x12)) = (short) iVar13_y;
  iVar9_s = (*((int *) (param_2 + 0x2dc))) >> 8;
  new_var2 = iVar9_s;
  *((short *) (new_var + 0x14)) = (short) new_var2;
  iVar9_s = (*((int *) (param_2 + 0x2d4))) >> 8;
  *((short *) new_var) = (short) (new_var11 = iVar9_s);
  new_var15 = 0;
  iVar9_y = ((*((int *) (param_2 + 0x2d8))) >> 8) + (-0x100);
  *((short *) (new_var + 0x2)) = (short) iVar9_y;
  *((short *) (new_var + 0x4)) = (short) (iVar11 = ((iVar9_sgn = *((int *) (param_2 + 0x2dc))) >> 5) >> 3);
  *((int *) (new_var + 0x2c)) = *(*((int **) (D_8008D2AC + 0x160)));
  *((uint *) (new_var + 0x28)) = 0x10;
  *((uint *) (new_var + 0x24)) = 0x3000;
  *((ushort *) (new_var + 0x22)) = 0;
  if (D_8008D2AC[0x1ca8] < 3)
  {
    *((ushort *) (new_var + 0x22)) = 2;
  }
  new_var14 = param_2;
  *((short *) (new_var + 0x3e)) = 0;
  *((short *) (new_var + 0x3c)) = 0;
  if ((*((short *) (new_var + 0x10))) > (*((short *) new_var)))
  {
    *((short *) (new_var + 0x30)) = *((short *) new_var);
    *((short *) (new_var + 0x30)) = (new_var3 = *((short *) (new_var + 0x10)));
  }
  iVar9_sgn = (iVar9_y * 0x10000) >> 0x10;
  iVar13_sgn = (iVar13_y * 0x10000) >> 0x10;
  *((short *) (new_var + 0x32)) = (short) iVar13_y;
  if (iVar9_sgn < iVar13_sgn)
  {
    *((short *) (new_var + 0x32)) = (short) (new_var7 = (new_var11 = iVar9_y));
    return;
  }
  if ((*((short *) (new_var + 0x4))) < (*((short *) (new_var + 0x14))))
  {
    *((short *) (new_var + 0x34)) = *((short *) (new_var + 0x14));
    do
    {
      *((short *) (new_var + 0x34)) = (new_var3 = *((short *) (new_var + 0x4)));
    }
    while (0);
  }
  iVar13 = (short) iVar13_y;
  new_var2 = param_2;
  if ((*((short *) (new_var + 0x10))) < (*((short *) new_var)))
  {
    *((short *) (new_var + 0x36)) = *((short *) (new_var + 0x10));
    *((short *) (new_var + 0x36)) = (uVar7 = *((short *) new_var));
  }
  *((short *) (new_var + 0x38)) = iVar13;
  new_var4 = *((short *) (new_var + 0x4));
  if (iVar13_sgn < iVar9_sgn)
  {
    new_var2 = (short) iVar9_y;
    *((short *) (new_var + 0x38)) = new_var2;
  }
  if ((*((short *) (new_var + 0x14))) < (*((short *) (new_var + 0x4))))
  {
    *((short *) (new_var + 0x3a)) = new_var3;
  }
  *((short *) (new_var + 0x1c)) = uVar7;
  *((short *) (new_var + 0x1e)) = (short) new_var11;
  *((short *) (new_var + 0x20)) = new_var4;
  if ((*((int *) (param_2 + 0x350))) != 0)
  {
    func_8001F41C(*((int *) (param_2 + 0x350)), (void *) new_var);
  }
  if ((((*((short *) (new_var + 0x3e))) == 0) && ((*((int *) (new_var + 0x2c))) != 0)) && ((*((int *) ((*((int *) (new_var + 0x2c))) + 0x18))) != 0))
  {
    func_8001EBEC(*((int *) ((*((int *) (new_var + 0x2c))) + 0x18)), (void *) (new_var + 0x30), (void *) func_8001F5F0, (void *) new_var);
  }
  iVar9_s = *((int *) (param_1 + 0x34));
  if ((*((short *) (new_var + 0x3e))) != 0)
  {
    *((uint *) (iVar9_s + 0x70)) = (((((*((ushort *) (new_var + 0x70))) >> 6) & 0xff) | (((*((ushort *) (new_var + 0x72))) << 2) & 0xff00)) | ((((uint) ((*((ushort *) (new_var + 0x74))) >> 6)) & 0xff) << 0x10)) | ((((uint) (*((byte *) (param_2 + 0x4a)))) + 1) * 0x1000000);
    *((int *) (param_2 + 0x2d0)) = ((int) (*((short *) (new_var + 0x1e)))) << 8;
    *((ushort *) (param_2 + 0xaa)) = (*((ushort *) (param_2 + 0xaa))) | 4;
    cVar1 = *((char *) ((*((int *) (new_var + 0x80))) + 0x38));
    do
    {
      if (((cVar1 == '\x0e') || (cVar1 == '\x04')) || (cVar1 == '\r'))
      {
        *((ushort *) (iVar9_s + 0x56)) = 0;
        *((uint *) (iVar9_s + 0x28)) = (*((uint *) (iVar9_s + 0x28))) | 0x2000;
      }
      ;
      if (D_8008D2AC[0x1ca8] < 2)
      {
        uVar2 = *((ushort *) ((*((int *) (new_var + 0x80))) + 0x12));
        if ((uVar2 & 0x2000) == 0)
        {
          if ((uVar2 & 1) != 0)
          {
            *((uint *) (iVar9_s + 0x28)) = (*((uint *) (iVar9_s + 0x28))) | 0x4000;
            *((ushort *) (iVar9_s + 0x56)) = *((ushort *) ((*((int *) (D_8008D2AC + 0x160))) + 0x186));
          }
          else
            if ((uVar2 & 4) != 0)
          {
            iVar14 = iVar11;
            *((uint *) (iVar9_s + 0x28)) = (*((uint *) (iVar9_s + 0x28))) | 0x4000;
            *((ushort *) (iVar9_s + 0x56)) = *((ushort *) ((*((int *) (D_8008D2AC + 0x160))) + 0x184));
          }
          else
          {
            *((uint *) (iVar9_s + 0x28)) = (*((uint *) (iVar9_s + 0x28))) & 0xffffbfff;
          }
        }
      }
      uVar10 = ((int) (*((short *) (param_2 + 0x372)))) * 5;
      iVar15 = ((int) (*((short *) (new_var2 + 0x370)))) * 5;
      iVar15 = iVar15 + (((int) (*((short *) (new_var + 0x70)))) * 3);
      iVar14 = uVar10 + (((int) (*((short *) (new_var + 0x72)))) * 3);
      iVar13 = ((int) (*((short *) (param_2 + 0x374)))) * 5;
      iVar13 = iVar13 + (((int) ((short) (*((ushort *) (new_var + 0x74))))) * 3);
      uVar6 = func_80059070(((iVar15 * iVar15) + (iVar14 * iVar14)) + (iVar13 * iVar13), 0x18);
      new_var16 = iVar15 * 0x1000;
      uVar6 = uVar6 >> 0xc;
      *((ushort *) (param_2 + 0x370)) = (short) (new_var16 / ((int) uVar6));
      *((ushort *) (param_2 + 0x372)) = (short) ((iVar14 * 0x1000) / ((int) uVar6));
      *((ushort *) (param_2 + 0x374)) = (short) ((iVar13 * 0x1000) / ((int) uVar6));
      if (((*((ushort *) ((*((int *) (new_var + 0x80))) + 0x12))) & 0x80) != 0)
      {
        *((uint *) (param_2 + 0x2c8)) = (*((uint *) (param_2 + 0x2c8))) | 0x10000;
      }
      *((int *) (param_2 + 0x350)) = *((int *) (new_var + 0x80));
      if (((*((int *) (param_2 + 0x2d8))) <= ((*((int *) (param_2 + 0x2d0))) + 0x1000)) || (((*((char *) ((*((int *) (new_var + 0x80))) + 0x38))) == '\x0e') && ((*((int *) (param_2 + 0x2d8))) < 1)))
      {
        if (((*((ushort *) ((*((int *) (new_var + 0x80))) + 0x12))) & 0x1000) != 0)
        {
          *((uint *) (param_2 + 0xa4)) = *((uint *) (new_var + 0x70));
          do
          {
            *((ushort *) (param_2 + 0xaa)) = (*((ushort *) (param_2 + 0xaa))) | 8;
            uVar2 = *((ushort *) (new_var + 0x74));
            *((short *) (new_var + 0x3a)) = *((short *) (new_var + 0x14));
          }
          while (0);
          *((ushort *) (param_2 + 0xa8)) = uVar2;
        }
        ;
        if ((*((int *) (param_2 + 0xa0))) == 0)
        {
          *((int *) (param_2 + 0xa0)) = *((int *) (new_var + 0x80));
          *((uint *) (param_2 + 0x360)) = (iVar13_sgn = *((uint *) (new_var + 0x70)));
          *((ushort *) (param_2 + 0x364)) = *((ushort *) (new_var + 0x74));
        }
        if ((((iVar13_y = *((int *) (new_var + 0xcc))) != 0) && ((*((int *) (new_var + 0xd0))) != 0)) && ((*((int *) (new_var + 0xd4))) != 0))
        {
          iVar14 = (int) (*((short *) (new_var + 0xc8)));
          iVar13 = (int) (*((short *) (new_var + 0xca)));
          uVar12 = (uint) (*((byte *) (iVar13_y + 8)));
          uVar10 = (uint) (*((byte *) (iVar13_y + 9)));
          uVar6 = (uint) (*((byte *) (iVar13_y + 10)));
          new_var7 = iVar14 * ((*((byte *) ((*((int *) (new_var + 0xd0))) + 8))) - uVar12);
          iVar15 = ((((int) new_var7) >> 0xc) + (((int) (iVar13 * ((*((byte *) ((uVar7 = *((int *) (new_var + 0xd4))) + 8))) - uVar12))) >> 0xc)) + uVar12;
          iVar11 = ((((int) (iVar14 * ((*((byte *) ((*((int *) (new_var + 0xd0))) + 9))) - uVar10))) >> (uVar7 = 0xc)) + (((int) (iVar13 * ((*((byte *) (uVar7 + 9))) - uVar10))) >> 0xc)) + uVar10;
          iVar13 = ((((int) (iVar14 * ((*((byte *) ((*((int *) (new_var + 0xd0))) + 10))) - uVar6))) >> 0xc) + (((int) (iVar13 * ((*((byte *) ((*((int *) (new_var + 0xd4))) + 10))) - uVar6))) >> 0xc)) + uVar6;
          if (iVar15 < 0)
          {
            iVar15 = 0;
          }
          if (0xff < iVar15)
          {
            iVar15 = 0xff;
          }
          if (iVar11 < 0)
          {
            iVar14 = 0;
          }
          if (0xff < iVar11)
          {
            iVar14 = 0xff;
          }
          if (iVar13 < 0)
          {
            iVar11 = 0;
          }
          if (0xff < iVar13)
          {
            iVar11 = 0xff;
          }
          iVar14 = (((((iVar15 * 0x4c) >> 8) + ((iVar14 * 0x96) >> 8)) + ((iVar11 * 0x1e) >> 8)) * (-0x20)) + 0xc00;
          if (iVar14 < 0)
          {
            iVar13 = 0;
          }
          iVar15 = iVar13 << 3;
          ;
          if ((0x1000 + 1) <= iVar14)
          {
            iVar13 = 0x1000;
          }
          ;
          *((ushort *) (param_2 + 0x508)) = (short) ((((*((ushort *) (param_2 + 0x508))) * 200) + (((iVar13 << 3) - iVar13) * 8)) >> 8);
          *((ushort *) (iVar9_s + 0x22)) = (short) ((((*((ushort *) (iVar9_s + 0x22))) * 200) + (((iVar13 << 3) - iVar13) * 8)) >> 8);
        }
      }
    }
    while (0);
  }
  else
  {
    *((uint *) (iVar9_s + 0x70)) = ((((uint) (*((byte *) (param_2 + 0x4a)))) + 1) * 0x1000000) | 0x4000;
    *((uint *) (iVar9_s + 0x28)) = (*((uint *) (iVar9_s + 0x28))) & 0xffffbfff;
    *((int *) (param_2 + 0x2d0)) = (*((int *) (new_var14 + 0x2d8))) + (-0x10000);
  }
  if (((*((int *) (param_2 + 0x2d0))) + 0x8000) < (*((int *) (param_2 + 0x2d8))))
  {
    uVar7 = func_80057C68(10);
    *((uint *) (param_2 + 0x35c)) = uVar7;
  }
  if ((*((int *) (param_2 + 0x2d8))) < (((((int) (*((short *) ((*((int *) ((*(*((int **) (D_8008D2AC + 0x160)))) + 0x18))) + 6)))) + (-0x40)) * 32) * 8))
  {
    *((ushort *) (param_2 + 0xaa)) = (*((ushort *) (param_2 + 0xaa))) | 1;
  }
  iVar9 = *((int *) (param_2 + 0x8c));
  iVar9 = iVar9 - (*((int *) (param_2 + 0x3a4)));
  iVar9_sgn = *((int *) (param_2 + 0xa0));
  if (((iVar9_sgn != 0) && (((*((ushort *) (param_2 + 0xaa))) & 9) == 0)) && ((*((char *) (param_2 + 0x376))) != '\x05'))
  {
    *((int *) (param_2 + 0x88)) = (*((int *) (param_2 + 0x88))) + (((int) (((uint) (*((ushort *) (param_2 + 0x360)))) << 0x10)) >> 0x11);
    *((int *) (param_2 + 0x8c)) = (*((int *) (param_2 + 0x8c))) + (((int) (((uint) (*((ushort *) (param_2 + 0x362)))) << 0x10)) >> 0x11);
    *((int *) (param_2 + 0x90)) = (*((int *) (param_2 + 0x90))) + (((int) (((uint) (*((ushort *) (param_2 + 0x364)))) << 0x10)) >> 0x11);
  }
  iVar13 = *((int *) (param_2 + 0xa0));
  *((uint *) (param_2 + 0x3a0)) = *((uint *) (param_2 + 0x88));
  *((uint *) (param_2 + 0x3a4)) = *((uint *) (param_2 + 0x8c));
  ;
  new_var4 = *((uint *) (param_2 + 0x90));
  *((uint *) (param_2 + 0x3a8)) = new_var4;
  if (iVar13 != 0)
  {
    if (((*((ushort *) (iVar13 + 0x12))) & 0x200) == 0)
    {
      if ((((*((char *) (param_2 + 0x376))) != '\x05') && (((*((ushort *) (param_2 + 0xaa))) & 1) == 0)) && (((*((ushort *) (iVar13 + 0x12))) & 0x1000) != 0))
      {
        if ((*((byte *) (iVar13 + 0x3e))) != 0xff)
        {
          iVar13 = (uint) (*((byte *) (iVar13 + 0x3e)));
          iVar13 = (*((int *) ((*((int *) (D_8008D2AC + 0x160))) + 0x14c))) + ((6 * iVar13) * 2);
          new_var8 = (ushort *) (param_2 + 0xaa);
          if (((((*((uint *) (param_2 + 0x2c8))) & 0x1000000) == 0) && (1 < (*((byte *) (iVar13 + 8))))) && (((int) (new_var12 = (*((int *) (param_2 + 0x48c))) - (((uint) (*((ushort *) (iVar13 + 6)))) * 8))) >= (((int) (new_var3 = ((*((ushort *) ((*((int *) ((*((int *) (D_8008D2AC + 0x160))) + 0x14c))) + 6))) >> 2) << 3)) + 1)))
          {
            *new_var8 = (*((ushort *) (param_2 + 0xaa))) | 1;
          }
          else
          {
            uVar2 = *((ushort *) ((*((int *) ((*((int *) (D_8008D2AC + 0x160))) + 0x14c))) + 6));
            uVar7 = 5 * (((uint) uVar2) * 3);
            if (((((uint) (new_var2 = *((ushort *) (iVar13 + 6)))) < ((uint) (((int) uVar7) >> 4))) && ((bVar4, (*((byte *) ((*((int *) (param_2 + 0x354))) + 0x3e))) != 0xff))) && (((((uint) (*((ushort *) (((((uint) (*((byte *) ((*((int *) (param_2 + 0x354))) + 0x3e)))) * 0xc) + (*((int *) ((*((int *) (D_8008D2AC + 0x160))) + 0x14c)))) + 6)))) + ((uint) (uVar2 >> 2))) + 1) <= ((((uint) new_var2) - 1) + 1)))
            {
              goto LAB_8001e340;
            }
            *((uint *) (param_2 + 0x354)) = *((uint *) (param_2 + 0xa0));
          }
        }
        else
          if (((*((uint *) (D_8008D2AC + 0x1a10))) - 0x19U) < 5)
        {
          *((int *) (param_2 + 0x354)) = iVar13;
        }
      }
    }
    else
    {
      LAB_8001e340:
      *((ushort *) (param_2 + 0xaa)) = (*((ushort *) (param_2 + 0xaa))) | 1;

    }
    *((short *) (param_2 + 0x3fc)) = 0;
    *((uint *) (param_2 + 0x2c8)) = (*((uint *) (param_2 + 0x2c8))) & 0xfff7ffbf;
    if (((*((ushort *) (param_2 + 0xaa))) & 8) == 0)
    {
      goto LAB_8001e4d0;
    }
    if (((*((short *) (param_2 + 0x50a))) == 2) || (((*((uint *) (D_8008D2AC + 8))) & 0x80000) != 0))
    {
      *((char *) (param_2 + 0xc2)) = 6;
      uVar6 = 6;
    }
    else
    {
      bVar4 = *((byte *) ((*((int *) (param_2 + 0xa0))) + 0x38));
      uVar6 = (uint) bVar4;
      if ((bVar4 != 6) && ((*((char *) (param_2 + 0xc2))) == '\x06'))
      {
        *((short *) (param_2 + 0x414)) = 0xfec0;
      }
    }
    uVar7 = func_80057C68(uVar6);
    *((uint *) (param_2 + 0x358)) = uVar7;
    *((uint *) (param_2 + 0x35c)) = uVar7;
    *((short *) (param_2 + 0x3f4)) = 0xa0;
    *((ushort *) (new_var + 0x22)) = 0;
    ;
    *((uint *) (param_2 + 0x2c8)) = (*((uint *) (param_2 + 0x2c8))) | 1;
    if (((*((uint *) (param_2 + 0x2cc))) & 1) == 0)
    {
      *((short *) (param_2 + 0x414)) = 0x140;
      *((uint *) (param_2 + 0x2c8)) = (*((uint *) (param_2 + 0x2c8))) | 0x83;
      new_var4 = func_80058F9C(iVar9, 0x100, 0x3c00, 0x78, 0xfa);
      uVar6 = new_var4;
      if ((*((char *) (param_2 + 0x376))) != '\x05')
      {
        uVar6 = (uVar6 & 0xff) << 0x10;
        if (((*((uint *) (param_2 + 0x2c8))) & 0x10000) != 0)
        {
          uVar6 = uVar6 | 0x8080;
        }
        func_800284D0(7, 1, (((uVar6 & 0xFFFFu) & 0xFFFFu) & 0xFFFFu) | 0x1008080);
      }
    }
    iVar9 = 6;
  }
  else
  {
    LAB_8001e4d0:
    if ((*((short *) (param_2 + 0x3f2))) != 0)
    {
      *((uint *) (param_2 + 0x2c8)) = (*((uint *) (param_2 + 0x2c8))) | 0x80000;
    }

    do
    {
      lVar8 = *((uint *) (param_2 + 0x2c8));
      if ((*((short *) (param_2 + 0x3fa))) != 0)
      {
        *((uint *) (param_2 + 0x2c8)) = 0x40;
        *((uint *) (param_2 + 0x2c8)) = *((uint *) (param_2 + 0x2c8));
        *((uint *) (param_2 + 0x2c8)) = lVar8 | (*((uint *) (param_2 + 0x2c8)));
      }
      uVar7 = func_80057C68(10);
      do
      {
        *((uint *) (param_2 + 0x358)) = uVar7;
        *((char *) (param_2 + 0xc2)) = 10;
        puVar3 = D_8008D2AC;
        *((uint *) (param_2 + 0x2c8)) = (*((uint *) (param_2 + 0x2c8))) & 0xfffffffe;
      }
      while (0);
      *((short *) (param_2 + 0x3fc)) = (*((short *) (param_2 + 0x3fc))) + (*((short *) (puVar3 + 0x1d04)));
    }
    while (0);
    iVar9 = ((uint) (*((ushort *) (param_2 + 0x3f4)))) - ((uint) (*((ushort *) (puVar3 + 0x1d04))));
    iVar9 = -iVar9;
    if (((-iVar9) * 0x10000) < 0)
    {
      *((short *) (param_2 + 0x3f4)) = (short) iVar9;
      *((short *) (param_2 + 0x3f4)) = 0;
    }
    iVar9 = (*((int *) ((&D_800845A0) + ((uVar6 & 0x3ff) * 4)))) >> 0x10;
    iVar9 = 7;
    if ((*((short *) (param_2 + 0x3f4))) == 0)
    {
      *((short *) (param_2 + 0x3f2)) = 0;
      *((short *) (param_2 + 0x3fa)) = 0;
      iVar11 = uVar6;
    }
  }
  ;
  local_arr[0] = ((iVar9 * (*((short *) (param_2 + 0x368)))) + ((8 - iVar9) * (*((short *) (param_2 + 0xa4))))) >> 3;
  iVar11 = iVar9 * (*((short *) (param_2 + 0x36a)));
  local_arr[1] = (iVar11 + ((8 - iVar9) * (*((short *) (param_2 + 0xa6))))) >> 3;
  local_arr[2] = ((iVar9 * (*((short *) (param_2 + 0x36c)))) + ((8 - iVar9) * (*((short *) (param_2 + 0xa8))))) >> 3;
  if (0 < ((int) (*((short *) (param_2 + 0xe)))))
  {
    uVar6 = ((int) (*((short *) (param_2 + 0xe)))) * 0xc;
    new_var10 = &D_800845A0;
    new_var5 = (int *) (new_var10 + ((uVar6 & 0x3ff) * 4));
    new_var12 = *((int *) new_var5);
    sVar5 = (short) new_var12;
    if ((uVar6 & 0x400) == 0)
    {
      iVar13 = (int) sVar5;
      if ((iVar11 & 0x800) != 0)
      {
        iVar14 = -iVar9;
        goto LAB_8001e680;
      }
    }
    else
    {
      iVar14 = (int) sVar5;
      iVar13 = new_var13;
      if ((iVar11 & 0x800) == 0)
      {
        iVar9 = -iVar14;
      }
      else
      {
        LAB_8001e680:
        iVar13 = -iVar13;

        iVar9 = iVar14;
      }
    }
    new_var7 = (iVar13 * 0x19) >> 10;
    {
      int _tmp_z = (iVar9 * 0x19) >> 10;
      int _tmp_xy = new_var7 & 0xffff;
      asm volatile("mtc2 %0, $0" : : "r"(_tmp_xy));
      asm volatile("mtc2 %0, $1" : : "r"(_tmp_z));
    }
    asm volatile(".word 0x4A486012");
    do
    {
      asm volatile("mfc2 %0, $25" : "=r"(iVar9));
      asm volatile("mfc2 %0, $26" : "=r"(iVar14));
      asm volatile("mfc2 %0, $27" : "=r"(iVar13));
    }
    while (0);
    local_arr[0] = local_arr[0] + iVar9;
    local_arr[1] = local_arr[1] + iVar14;
    local_arr[2] = local_arr[2] + iVar13;
  }
  uVar6 = func_80059070(((local_arr[0] * local_arr[0]) + (local_arr[1] * local_arr[1])) + (local_arr[2] * local_arr[2]), 0x18);
  uVar6 = uVar6 >> 0xc;
  new_var4 = uVar6;
  *((short *) (param_2 + 0x368)) = (short) ((local_arr[0] << 0xc) / ((int) new_var4));
  *((short *) (param_2 + 0x36a)) = (short) ((local_arr[1] << 0xc) / ((int) uVar6));
  uVar10 = (uint) (*((short *) (param_2 + 0x39a)));
  *((short *) (param_2 + 0x36c)) = (short) ((local_arr[2] << 0xc) / ((int) new_var4));
  iVar14 = *((int *) ((&D_800845A0) + ((uVar10 & 0x3ff) * 4)));
  sVar5 = (short) iVar14;
  if ((0x400 & uVar10) == 0)
  {
    iVar13 = (int) sVar5;
    if ((uVar10 & 0x800) != 0)
    {
      iVar14 = -iVar9;
      goto LAB_8001e84c;
    }
  }
  else
  {
    iVar14 = (int) sVar5;
    iVar9 = iVar14;
    iVar13 = new_var13;
    if ((uVar10 & 0x800) == 0)
    {
      iVar9 = -iVar14;
    }
    else
    {
      LAB_8001e84c:
      iVar13 = (unsigned short) (-iVar13);

    }
  }
  lVar8 = func_8007173C((((-((int) (*((short *) (param_2 + 0x368))))) * iVar9) + ((*((short *) (param_2 + 0x36c))) * iVar13)) >> 0xc, (int) (*((short *) (param_2 + 0x36a))));
  *((short *) (param_2 + 0x2f0)) = (short) lVar8;
  if ((*((short *) (param_2 + 0xe))) >= 1)
  {
    ;
    if (0 > ((int) (*((signed char *) (param_2 + 0x377)))))
    {
      iVar9 = -iVar9;
    }
    new_var7 = *((char *) (param_2 + 0x376));
    if ((iVar9 < 4) && ((*((short *) (param_2 + 0x3ee)) = 4, new_var7 != '\x03')))
    {
      ;
      if (((int) (*((signed char *) (param_2 + 0x377)))) < 1)
      {
        goto code_r0x8001e96c;
        *((byte *) (param_2 + 0xc2)) = bVar4;
      }
    }
    func_80028468(0x10, 1);
  }
  else
  {
    if ((*((uint *) (param_2 + 0x2c8)) & 1) != 0)
    {
      ;
      if (((int) (*((short *) (param_2 + 0x38c)))) < 0)
      {
        ;
      }
      if (0x1000 < (-((int) (*((short *) (param_2 + 0x38c))))))
      {
        ;
        if (((int) (*((signed char *) (param_2 + 0x377)))) < 0)
        {
          ;
        }
        lVar8 = 0x3ee;
        if (((-((int) (*((signed char *) (param_2 + 0x377))))) < 4) && (((*((uint *) ((*((int *) (param_2 + 0x358))) + 4))) & 1) != 0))
        {
          *((short *) (param_2 + lVar8)) = 4;
        }
        goto LAB_8001e964;
      }
    }
    *((short *) (param_2 + 0x3ee)) = 0;
  }
  LAB_8001e964:
  iVar9 = (int) (*((signed char *) (param_2 + 0x377)));

  code_r0x8001e96c:
  if (((int) (*((signed char *) (param_2 + 0x377)))) < 0)
  {
    iVar9 = -iVar9;
  }

  new_var12 = *((int *) (param_2 + 0x358));
  new_var9 = &(*((ushort *) ((*((int *) (param_2 + 0x358))) + 0x32)));
  if ((unsigned short) (9 < iVar9))
  {
    *((short *) (param_2 + 0x3ee)) = 0;
  }
  if ((*((short *) (param_2 + 0x3ee))) != 0)
  {
    *((short *) (param_2 + 0x3ee)) = (*((short *) (param_2 + 0x3ee))) + (-1);
    *((signed char *) (param_2 + 0x377)) = (*((signed char *) (param_2 + 0x377))) + '\x03';
  }
  else
  {
    bVar4 = (*((byte *) (param_2 + 0x377)) = (*((signed char *) (param_2 + 0x377))) - 4);
    if (('\0' < (*((signed char *) (param_2 + 0x377)))) && ((*((byte *) (param_2 + 0x377)) = bVar4, uVar12 = ((int) (((uint) bVar4) << 0x18)) < 1)))
    {
      *((signed char *) (param_2 + 0x377)) = 0;
      if (((*((uint *) (new_var12 + 4))) & 0x20) != 0)
      {
        uVar7 = 0x808080;
        lVar8 = *new_var9;
        if (((*((uint *) (param_2 + 0x2c8))) & 0x10000) != 0)
        {
          uVar7 = 0x1808080;
        }
        func_800284D0((uint) lVar8, 0, uVar7);
      }
    }
    bVar4 = (*((signed char *) (param_2 + 0x377))) - 4;
    if (((int) (((uint) bVar4) << 0x18)) < 0)
    {
      *((signed char *) (param_2 + 0x377)) = 0;
    }
  }
  ;
  if (((*((int *) (param_2 + (uVar12 = 0x2d8)))) < (-0x8000)) && (((*((uint *) ((*((int *) (D_8008D2AC + 0x160))) + 0xdc))) & 2) != 0))
  {
    *((ushort *) (param_2 + 0xaa)) = (*((ushort *) (param_2 + 0xaa))) | 1;
  }
  if ((((((*((char *) (param_2 + 0x376))) != '\x05') && (((*((ushort *) (param_2 + 0xaa))) & 1) != 0)) && ((*((int *) (param_2 + 0x354))) != 0)) && ((D_8008D000 & 0x1000) == 0)) && (((*((uint *) (param_2 + 0xbc))) & 8) == 0))
  {
    func_800671B0(param_1, param_2);
  }
}
