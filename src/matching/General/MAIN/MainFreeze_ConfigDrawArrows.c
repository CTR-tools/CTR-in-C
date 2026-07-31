// MainFreeze_ConfigDrawArrows @ 0x80037bc0  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 7790, grind floor ~6895 non-natural).
// Residual: whole-function register allocation + the retail unaligned 16-byte color
// gradient copy idiom (lwl/lwr/swl/swr to the stack arg area) on a call-heavy fn.
// Draws two flanking arrows around a centered string: color flashes red/orange via
// frameCounter & 4 (read u16/lhu), lineWidth = DecalFont_GetLineWidth(str,1)>>1, then
// two DecalHUD_Arrow2D calls (left offsetX-lw-0x14 flag 0x800, right offsetX+lw+0x12
// flag 0). icon = *(gGT->iconGroup4 + 0xF4); primMem = backBuffer+0x74; ot =
// pushBuffer_UI.ptrOT (gGT+0x147C). gGT gp-relative (gp+0x340); color = 4 gradient
// WORDS at data.ptrColor[color] (matches saphi — a prior byte-read bug was fixed).
#include "CTR.h"

extern unsigned short D_8008D970;   /* sdata->frameCounter (absolute; read lhu) */
extern int *D_80081D70[];    /* data.ptrColor[] — each entry points to 4 gradient words */

struct GameTracker {
    char pad_10[0x10];
    char *backBuffer;        /* 0x10 */
    char pad_147C[0x147C - 0x14];
    void *pushBuffer_UI_ptrOT;   /* 0x147C */
    char pad_2124[0x2124 - 0x1480];
    char *iconGroup4;        /* 0x2124 */
};

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */

extern int  func_800224D0(char *str, int mode);   /* DecalFont_GetLineWidth */
extern void func_80023190(void *icon, int x, int y, void *primMem, void *ot,
                          int c0, int c1, int c2, int c3, char zero, int a10, int a11); /* DecalHUD_Arrow2D */

void MainFreeze_ConfigDrawArrows(short offsetX, short offsetY, char *str)
{
    struct GameTracker *gGT;
    int color = 0;
    int lineWidth;
    char *colorWords;
    void *icon;

    if ((D_8008D970 & 4) == 0)
        color = 3;

    lineWidth = func_800224D0(str, 1) >> 1;
    colorWords = D_80081D70[color];

    gGT = D_8008D2AC;
    icon = *(void **)(gGT->iconGroup4 + 0xF4);
    func_80023190(icon, (offsetX - lineWidth) - 0x14, offsetY + 7,
                  gGT->backBuffer + 0x74, gGT->pushBuffer_UI_ptrOT,
                  *(int *)colorWords, *(int *)(colorWords+4), *(int *)(colorWords+8), *(int *)(colorWords+12), 0, 0x1000, 0x800);

    gGT = D_8008D2AC;
    icon = *(void **)(gGT->iconGroup4 + 0xF4);
    func_80023190(icon, (offsetX + lineWidth) + 0x12, offsetY + 7,
                  gGT->backBuffer + 0x74, gGT->pushBuffer_UI_ptrOT,
                  *(int *)colorWords, *(int *)(colorWords+4), *(int *)(colorWords+8), *(int *)(colorWords+12), 0, 0x1000, 0);
}
