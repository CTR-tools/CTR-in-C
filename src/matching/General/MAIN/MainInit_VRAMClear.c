// MainInit_VRAMClear @ 0x8003c248  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 1720, grind floor ~1465 non-natural).
// Residual: retail builds the fill-prim tag word `a` via a read-modify-write of
// uninitialized stack (lw;or 0xffffff;sb 3) and b2 byte-wise; a clean C
// `commands.a = 0x3ffffff` compiles to lui/ori/sw instead (behaviorally identical,
// reproducing the RMW would need UB/hacks). 
// Clears all VRAM to black at boot: SetDefDrawEnv (0,0,0x400,0x200), dfe=1,
// PutDrawEnv, then two GP0 fill rects via DrawOTag (tag 0x3ffffff, RGB 0): the full
// 0x3ff x 0x1ff region, then a 1px strip at y=0x1ff.
#include "CTR.h"

typedef struct { char pad0[0x17]; char dfe; char pad1[0x60 - 0x18]; } DRAWENV;

extern void func_80077D38(DRAWENV *env, int x, int y, int w, int h);  /* SetDefDrawEnv */
extern void func_80073408(DRAWENV *env);                              /* PutDrawEnv */
extern void func_80073398(unsigned int *p);                          /* DrawOTag */

void MainInit_VRAMClear(void)
{
    DRAWENV drawEnv;
    struct { int a; short b1, b2, c, d, e, f; } commands;

    func_80077D38(&drawEnv, 0, 0, 0x400, 0x200);
    drawEnv.dfe = 1;
    func_80073408(&drawEnv);

    commands.a = 0x3ffffff;
    commands.b1 = 0;
    commands.b2 = 0x200;
    commands.c = 0;
    commands.d = 0;
    commands.e = 0x3ff;
    commands.f = 0x1ff;
    func_80073398((unsigned int *)&commands);

    commands.d = 0x1ff;
    commands.f = 1;
    func_80073398((unsigned int *)&commands);
}
