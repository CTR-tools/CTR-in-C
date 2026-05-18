// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef unsigned int uint;
typedef unsigned short ushort;

uint func_80030F58(int param_1, int param_2)
{
    int iVar1;
    int *animArr;
    int anim;

    iVar1 = *(int *)(param_1 + 0x18);
    if (iVar1 == 0) return 0;
    if (*(short *)(iVar1 + 0x12) <= 0) return 0;
    iVar1 = *(int *)(iVar1 + 0x14);
    if (iVar1 == 0) return 0;
    if (param_2 >= *(int *)(iVar1 + 0x34)) return 0;
    animArr = *(int **)(iVar1 + 0x38);
    if (animArr == 0) return 0;
    anim = animArr[param_2];
    if (anim == 0) return 0;
    return (uint)*(ushort *)(anim + 0x10) & 0x7FFF;
}
