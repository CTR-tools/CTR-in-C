// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef unsigned int uint;
typedef unsigned short ushort;
void fun_80020334(int param_1, int param_2, int param_3)
{
  int uVar1;
  int flagScrub;
  int iVar3;
  int *piVar4;
  int iVar5;
  int offset;
  uVar1 = *((short *) (param_3 + 0x22));
  if (param_1 == 0)
  {
    *((ushort *) (param_3 + 0x22)) = uVar1 & 0xffdf;
    *((short *) (param_3 + 0xe)) = 0;
    *((int *) (param_3 + 0x2c0)) = 0;
    return;
  }
  iVar3 = (*((int *) (param_3 + 0x2c0))) + (-1);
  if ((-1) < iVar3)
  {
    flagScrub = uVar1 | 0x20;
    iVar5 = (iVar3 * 0xc) + 0x20c;
    do
    {
      piVar4 = (int *) (param_3 + iVar5);
      if (((*piVar4) == param_1) && (piVar4[1] == param_2))
      {
        iVar3 = piVar4[2];
        if (iVar3 < 0x401)
        {
          iVar3 = iVar3 + 0x100;
          piVar4[2] = iVar3;
        }
        *((ushort *) (param_3 + 0x22)) = flagScrub;
        *((short *) (param_3 + 0xe)) = (short) iVar3;
        return;
      }
      iVar3 = iVar3 + (-1);
      iVar5 = iVar5 + (-0xc);
    }
    while ((-1) < iVar3);
  }
  offset = ((*((int *) (param_3 + 0x2c0))) * 0xc) + 0x20c;
  piVar4 = (int *) (param_3 + offset);
  *piVar4 = param_1;
  piVar4[1] = param_2;
  piVar4[2] = 0;
  *((ushort *) (param_3 + 0x22)) = uVar1 & 0xffdf;
  *((short *) (param_3 + 0xe)) = 0;
  *((int *) (param_3 + 0x2c0)) = (*((int *) (param_3 + 0x2c0))) + 1;
}
