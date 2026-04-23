// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef unsigned int uint;
typedef unsigned char byte;
void func_8001D0C4(int, int *);
void fun_8001d610(int param_1, int param_2)
{
  int iVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  int *temp;
  temp = *((int **) (param_1 + 0x14));
  if (temp == ((int *) 0x0)) return;
  piVar3 = temp;
  piVar4 = piVar3 + 2;
  while (*piVar3 != 0)
  {
    iVar1 = (*((int *) (param_2 + 0xc4))) + (-1);
    if ((-1) < iVar1)
    {
      iVar2 = (iVar1 * 4) + param_2;
      do
      {
        iVar1 = iVar1 + (-1);
        do { if (piVar3 == (*((int **) (iVar2 + 0x88)))) { goto LAB_8001d750; } iVar2 = iVar2 + (-4); } while (0);
      }
      while ((-1) < iVar1);
    }
    if ((((((((*((byte *) piVar3)) & 0x80) == 0) || ((piVar3 + 2)[5] == 0)) || (((*((uint *) ((*((int *) ((piVar3 + 2)[5] + 0x2c))) + 0x28))) & 0xf) != 0)) && (((*((short *) (param_2 + 0x30))) <= (*((short *) (((int) (piVar3 + 2)) + 2)))) && ((*((short *) ((piVar3 + 2) + (-1)))) <= (*((short *) (param_2 + 0x36)))))) && (((*((short *) (param_2 + 0x32))) <= (*((short *) ((piVar3 + 2) + 1)))) && (((*((short *) (((int) (piVar3 + 2)) + (-2)))) <= (*((short *) (param_2 + 0x38)))) && ((*((short *) (param_2 + 0x34))) <= (*((short *) (((int) (piVar3 + 2)) + 6))))))) && ((*((short *) (piVar3 + 2))) <= (*((short *) (param_2 + 0x3a)))))
    {
      func_8001D0C4(param_2, piVar3);
    }
    LAB_8001d750:
    piVar3 = piVar3 + 8;
    piVar4 = piVar4 + 8;
  }
}
