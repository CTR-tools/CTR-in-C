// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef char undefined;
typedef short undefined2;
typedef int undefined4;

extern short D_80086E84[];
int D_8008D9F4 = 0;
extern void func_8002E760(uint, int);
extern void func_8002CBE8(int, int, int);
extern void func_80028468(int, int);

int VehPhysCrash_Attack(int param_1, int param_2, int param_3, int param_4)
{
  int iVar1;

  if ((*(uint *)(param_1 + 0x2c8) & 0x800000) == 0) {
    if ((*(uint *)(param_2 + 0x2c8) & 0x800000) != 0) {
      *(undefined *)(param_1 + 0x4ff) = 2;
      *(undefined *)(param_1 + 0x504) = 6;
      *(int *)(param_1 + 0x500) = param_2;
      if (((param_3 != 0) && (*(char *)(param_1 + 0x376) != '\x06')) &&
         (*(int *)(param_1 + 0x24) == 0)) {
        func_8002E760((uint)*(ushort *)(param_1 + 0x2ca) & 1, 0xff);
        func_8002CBE8(1, (int)(short)D_80086E84[*(byte *)(param_1 + 0x4a)], 0x10);
      }
    }
    if ((*(int *)(param_2 + 0x14) != 0) && (*(int *)(param_1 + 0x14) == 0)) {
      iVar1 = *(int *)(*(int *)(*(int *)(param_2 + 0x14) + 0x6c) + 0x30);
      *(ushort *)(iVar1 + 6) = *(ushort *)(iVar1 + 6) | 8;
      *(undefined4 *)(param_2 + 0x14) = 0;
      *(undefined *)(param_1 + 0x4ff) = 2;
      *(undefined *)(param_1 + 0x504) = 0;
      *(int *)(param_1 + 0x500) = param_2;
      if ((param_3 != 0) &&
          ((*(char *)(param_1 + 0x376) != '\x06' && (*(int *)(param_1 + 0x24) == 0)))) {
        func_8002E760((uint)*(ushort *)(param_1 + 0x2ca) & 1, 0xff);
        if (param_4 != 0) {
          func_80028468(0x4f, 1);
        }
        func_8002CBE8(1, (int)(short)D_80086E84[*(byte *)(param_1 + 0x4a)], 0x10);
      }
    }
    if ((((0xa00 < D_8008D9F4) &&
          (*(short *)(param_2 + 0x3e2) != 0)) &&
         ((*(uint *)(param_2 + 0x2c8) & 0x200) != 0)) &&
        (*(short *)(param_1 + 0x3e2) == 0)) {
      *(undefined *)(param_2 + 0x366) = 2;
      *(undefined *)(param_1 + 0x4ff) = 3;
      *(undefined *)(param_1 + 0x504) = 5;
      *(int *)(param_1 + 0x500) = param_2;
    }
  }
  return param_3;
}
