// MainInit_VRAMDisplay @ 0x8003c310  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 2865, Reg 13; grind floor ~2355 non-natural).
// Residual: nested-loop register allocation / array-indexing scheme (retail holds an
// x-base reg + recomputes &x[i] per outer iter, reading x[i] twice; 67 vs 56 insns).
// Clears/refreshes both VRAM display buffers: for each of x={0,0x100} y={0,0x128},
// build RECT {x+0x200, 0x10c, 0x100, 0xd8}, SetDrawMove(&move,&r,x,y), tag|=0xffffff,
// DrawOTag, DrawSync(0). SetDrawMove=func_800778DC, DrawOTag=func_80073398, DrawSync=func_80072FF0.
#include "CTR.h"

typedef struct { short x, y, w, h; } VRect;
struct DrMove { unsigned int tag; unsigned int rest[5]; };   /* DR_MOVE, tag@0 */

extern void func_800778DC(struct DrMove *move, VRect *r, int x, int y);   /* SetDrawMove */
extern void func_80073398(unsigned int *ot);                             /* DrawOTag */
extern void func_80072FF0(int mode);                                     /* DrawSync */

void MainInit_VRAMDisplay(void)
{
    VRect r;
    struct DrMove move;
    short x[2];
    short y[2];
    int i, j;

    x[0] = 0;
    x[1] = 0x100;
    y[0] = 0;
    y[1] = 0x128;

    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {
            r.x = x[i] + 0x200;
            r.y = 0x10c;
            r.w = 0x100;
            r.h = 0xd8;
            func_800778DC(&move, &r, x[i], y[j]);
            move.tag |= 0xffffff;
            func_80073398((unsigned int *)&move);
            func_80072FF0(0);
        }
    }
}
