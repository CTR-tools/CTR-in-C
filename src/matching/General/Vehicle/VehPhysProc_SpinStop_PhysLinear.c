// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef char undefined;
typedef short undefined2;
typedef int undefined4;

extern void func_8006181C(int, int);

void VehPhysProc_SpinStop_PhysLinear(undefined4 param_1, int param_2)
{
  func_8006181C(param_1, param_2);
  *(undefined2 *)(param_2 + 0x39c) = 0;
  *(undefined2 *)(param_2 + 0x39e) = 0;
  return;
}
