
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef char undefined;
typedef short undefined2;
typedef int undefined4;
extern int func_80059070(int, int);
extern int func_8007173C(int, int);
void VehPhysCrash_ConvertVecToSpeed(int param_1, int *param_2)
{
  int extraout_var_00;
  int extraout_var_01;
  int x;
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  iVar4 = func_80059070(((*param_2) * (*param_2)) + (param_2[2] * param_2[2]), 0x10);
  x = iVar4;
  *((undefined2 *) (param_1 + 0x38c)) = ((uint) func_80059070((((*param_2) * (*param_2)) + (param_2[1] * param_2[1])) + (param_2[2] * param_2[2]), 0x10)) >> 8;
  x = func_8007173C(param_2[1] << 8, x);
  *((undefined2 *) (param_1 + 0x394)) = (short) x;
  x = func_8007173C(*param_2, param_2[2]);
  *((undefined2 *) (param_1 + 0x396)) = (short) x;
  iVar4 = ((((*param_2) * ((int) (*((short *) (param_1 + 0x312))))) + (param_2[1] * ((int) (*((short *) (param_1 + 0x318)))))) + (param_2[2] * ((int) (*((short *) (param_1 + 0x31e)))))) >> 0xc;
  iVar1 = (((int) (*((short *) (param_1 + 0x312)))) * iVar4) >> 0xc;
  iVar2 = (((int) (*((short *) (param_1 + 0x318)))) * iVar4) >> 0xc;
  iVar3 = (((int) (*((short *) (param_1 + 0x31e)))) * iVar4) >> 0xc;
  extraout_var_00 = ((uint) func_80059070(((iVar1 * iVar1) + (iVar2 * iVar2)) + (iVar3 * iVar3), 0x10)) >> 8;
  *((short *) (param_1 + 0x390)) = extraout_var_00;
  if (iVar4 < 0)
  {
    *((short *) (param_1 + 0x390)) = -extraout_var_00;
  }
  iVar1 = (*param_2) - iVar1;
  iVar2 = param_2[1] - iVar2;
  iVar3 = param_2[2] - iVar3;
  extraout_var_01 = ((uint) func_80059070(((iVar1 * iVar1) + (iVar2 * iVar2)) + (iVar3 * iVar3), 0x10)) >> 8;
  *((short *) (param_1 + 0x38e)) = extraout_var_01;
  if ((((iVar1 * (*((short *) (param_1 + 0x314)))) + (iVar2 * (*((short *) (param_1 + 0x31a))))) + (iVar3 * (*((short *) (param_1 + 0x320))))) < 0)
  {
    *((short *) (param_1 + 0x38e)) = -extraout_var_01;
  }
  return;
}
