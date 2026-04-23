// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef char undefined;
typedef short undefined2;
typedef int undefined4;
extern char *D_8008D2AC;
extern short func_80058F54(int, int, int);
extern void func_8005F89C(int, int, int);
void VehPhysProc_SpinStop_PhysAngular(undefined4 param_1, int param_2)
{
  undefined *puVar1;
  short new_var2;
  int new_var;
  ushort uVar2;
  int new_var3;
  undefined2 uVar3;
  new_var3 = ((int) (*((short *) (param_2 + 0xc0)))) * (*((int *) (D_8008D2AC + 0x1d04)));
  new_var = param_2 + 0x39a;
  puVar1 = D_8008D2AC;
  uVar2 = ((*((short *) new_var)) + ((short) (new_var3 >> 0xd))) & 0xfff;
  *((ushort *) new_var) = uVar2;
  *((short *) (param_2 + 0x2ee)) = (uVar2 + (*((short *) (param_2 + 0x3c6)))) + (*((short *) (param_2 + 0x3d4)));
  uVar3 = func_80058F54((int) (*((short *) (param_2 + 0x2f2))), ((*((int *) (puVar1 + 0x1d04))) << 5) >> 5, 0);
  puVar1 = D_8008D2AC;
  *((undefined2 *) (param_2 + 0x2f2)) = uVar3;
  uVar3 = func_80058F54((int) (*((short *) (param_2 + 0x3c6))), ((*((int *) (puVar1 + 0x1d04))) << 7) >> 5, 0);
  *((undefined2 *) (param_2 + 0x3c6)) = uVar3;
  func_8005F89C(param_2 + 0x310, param_2 + 0x360, (int) (new_var2 = *((short *) new_var)));
  return;
}
