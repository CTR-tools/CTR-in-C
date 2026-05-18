// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef unsigned int uint;
typedef unsigned int undefined4;
typedef unsigned char byte;
extern char *D_8008D2AC;
short D_8008D990 = 0;
extern void func_8004E37C(undefined4 p1, int p2, int p3, int p4, undefined4 p5, int p6, int p7, int p8, int p9);
void func_80050AF8(short param_1, short param_2, short param_3, int param_4)
{
  char *new_var4;
  short *new_var2;
  short *new_var;
  int iVar1;
  int iVar2;
  int new_var3;
  if ((*((int *) (param_4 + 0x4E0))) != 0)
  {
    *((int *) (param_4 + 0x4E0)) += -1;
  }
  iVar2 = (int) param_3;
  new_var = &param_1;
  D_8008D990 = D_8008D990 + 0x100;
  new_var4 = D_8008D2AC + (((uint) (*((byte *) (param_4 + 0x4A)))) * 0x110);
  iVar1 = (iVar2 * 0xD000) >> 0x10;
  new_var2 = &param_2;
  func_8004E37C(*((undefined4 *) (D_8008D2AC + 0x1FB0)), (int) (*new_var), (int) (*new_var2), (*((int *) (D_8008D2AC + 0x10))) + 0x74, *((undefined4 *) (new_var4 + 0x25C)), 2, iVar2, iVar1, 0xFF0000);
  new_var3 = ((uint) (*((byte *) (param_4 + 0x4A)))) * 0x110;
  func_8004E37C(*((undefined4 *) (D_8008D2AC + 0x1FB0)), (int) (*new_var), (int) (*new_var2), (*((int *) (D_8008D2AC + 0x10))) + 0x74, *((undefined4 *) ((D_8008D2AC + new_var3) + 0x25C)), 3, iVar2, iVar1, 0xFF0000);
}
