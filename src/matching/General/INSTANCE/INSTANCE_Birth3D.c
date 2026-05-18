// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef unsigned int undefined4;

extern char *D_8008D2AC;
extern int func_800310D4(void *pool);
extern void func_80030778(int inst, undefined4 model, undefined4 name, undefined4 thread, undefined4 flags);

int func_8003086C(undefined4 param_1, undefined4 param_2, undefined4 param_3)
{
    int iVar1;

    iVar1 = func_800310D4(D_8008D2AC + 0x18F8);
    if (iVar1 != 0)
    {
        func_80030778(iVar1, param_1, param_2, param_3, 0xF);
    }
    return iVar1;
}
