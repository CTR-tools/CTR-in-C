// MainFreeze_ConfigDrawNPC105 @ 0x800379f4  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 6745, grind floor ~5295 non-natural).
// Residual: whole-function register allocation — 8 params held in saved regs across
// the loop (high pressure); gcc-2.8.1 assigns s-regs differently than retail.
// Draws a triangle-fan circle (NPC105 reward burst): per step, compute an edge point
// from the packed sin/cos quarter-table D_800845A0 with quadrant sign-flips (INLINED,
// as retail does — NOT via MATH_Cos/MATH_Sin calls), X via (radius*8/5)*cos, Y via
// radius*sin (>>12), and RECTMENU_DrawRwdTriangle each segment. color = 3x word-copy.
// Match levers: (radius<<3)/5 = the 0x66666667 div-by-5 magic; inline quadrant logic
// mirroring the asm (a&0x400 / a&0x800 sign flips); loop until (short)step > 0x1000.
#include "CTR.h"
extern int D_800845A0[];
extern void func_800453E8(void *pos, void *color, void *otMem, void *primMem);
void MainFreeze_ConfigDrawNPC105(short startX, short startY, short radius, int angleStep, short startAngle, char *color, void *otMem, void *primMem){
    short pos[6]; int colorBuf[3]; int scaledRadiusX = (radius << 3) / 5; unsigned int step = 0;
    colorBuf[0] = *(int *)color; colorBuf[1] = colorBuf[0]; colorBuf[2] = colorBuf[0];
    pos[0] = startX; pos[1] = startY;
    do {
        unsigned int a = (short)step + startAngle;
        short lo = (short)D_800845A0[a & 0x3ff];
        int hi = D_800845A0[a & 0x3ff] >> 0x10;
        int cx, sy;
        if ((a & 0x400) == 0) { sy = lo; if ((a & 0x800) != 0) { cx = -hi; sy = -sy; } else { cx = hi; } }
        else { if ((a & 0x800) == 0) { cx = -lo; sy = hi; } else { cx = lo; sy = -hi; } }
        pos[4] = startX + (short)((scaledRadiusX * cx) >> 0xc);
        pos[5] = startY + (short)((radius * sy) >> 0xc);
        if ((step & 0xffff) != 0) func_800453E8(pos, colorBuf, otMem, primMem);
        step = step + angleStep;
        pos[2] = pos[4]; pos[3] = pos[5];
    } while ((int)(step << 16) >> 16 <= 0x1000);
}
