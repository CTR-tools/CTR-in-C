// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef char undefined;
typedef short undefined2;
typedef int undefined4;
uint VehCalc_FastSqrt(uint param_1, uint param_2)
{
  uint uVar1;
  uint uVar2;
  int new_var;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uVar5 = 0;
  uVar3 = 1 << param_2;
  uVar4 = 1;
  uVar6 = 0;
  while ((uVar3 < param_1) && ((uVar3 << 2) != 0))
  {
    uVar4 = uVar4 + 1;
    uVar3 = uVar3 << 2;
  }

  new_var = param_2 - 1;
  uVar1 = 1 << (uVar4 + new_var);
  while (uVar3 != 0)
  {
    uVar2 = uVar5 << uVar4;
    if (((int) uVar4) < 0)
    {
      uVar2 = uVar5 >> (-uVar4);
    }
    uVar2 = (uVar6 + uVar3) + uVar2;
    if (uVar2 <= param_1)
    {
      uVar6 = uVar2;
      uVar5 = uVar5 + uVar1;
    }
    uVar1 = uVar1 >> 1;
    uVar3 = uVar3 >> 2;
    uVar4 = uVar4 - 1;
  }

  return uVar5;
}
