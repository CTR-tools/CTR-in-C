
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef char undefined;
typedef short undefined2;
typedef int undefined4;
ushort D_8008D9EC = 0;
ushort D_8008D9EE = 0;
ushort D_8008D9F0 = 0;
extern ushort D_8009AE38[3][3];
extern int D_8009AE28[3];
extern void func_8006C2A4(int *, int *);
void VehPhysCrash_AI(int param_1, int *param_2)
{
  int iVar1;
  int iVar2;
  int *new_var;
  D_8008D9EC = ((ushort) (*((byte *) ((*((int *) (param_1 + 0x5a4))) + 6)))) << 4;
  D_8008D9EE = ((ushort) (*((byte *) ((*((int *) (param_1 + 0x5a4))) + 7)))) << 4;
  D_8008D9F0 = ((ushort) (*((byte *) ((*((int *) (param_1 + 0x5a4))) + 8)))) << 4;
  func_8006C2A4((int *) D_8009AE38, (int *) (&D_8008D9EC));
  D_8009AE28[0] = ((int) (((uint) D_8009AE38[0][2]) << 0x10)) >> 0x14;
  D_8009AE28[1] = ((int) (((uint) D_8009AE38[1][2]) << 0x10)) >> 0x14;
  D_8009AE28[2] = ((int) (((uint) D_8009AE38[2][2]) << 0x10)) >> 0x14;
  iVar1 = (((D_8009AE28[0] * (*param_2)) + (D_8009AE28[1] * param_2[1])) + (D_8009AE28[2] * param_2[2])) >> 8;
  *((int *) (param_1 + 0x5d4)) = iVar1;
  *((int *) (param_1 + 0x5d8)) = (*param_2) - ((D_8009AE28[0] * (*(new_var = &iVar1))) >> 8);
  iVar1 = D_8009AE28[2] * iVar1;
  iVar2 = param_2[2];
  *((uint *) (param_1 + 0x5b0)) = (*((uint *) (param_1 + 0x5b0))) | 8;
  *((int *) (param_1 + 0x5e0)) = iVar2 - (iVar1 >> 8);
  return;
}
