// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef unsigned int undefined4;

extern void *func_800309A4(int p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7);

void *func_80030A50(undefined4 *param_1)
{
    return func_800309A4(*param_1, param_1[1], param_1[2], param_1[3], param_1[4], param_1[5], param_1[6]);
}
