// MainFrame_InitVideoSTR @ 0x80035d30  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 920, Register 4 — CORRECT offsets/behavior).
// Residual: retail emits a struct-copy (interleaved load-nop-store per rect field),
// but the videoSTR_src_vramRect is gp-relative and patch_gprel keys on the symbol
// name (overwrites the full imm, discarding struct-field addends), so the rect must
// use SEPARATE gp-relative globals (x/y/w/h) — which gcc then batch-loads instead of
// pairing. Behavior identical (correct 0x914/916/918/91A stores).
// Arms the fullscreen .STR video: copies the source VRAM rect (*r) to gp+0x914,
// enable flag to gp+0x35C, dst X/Y to gp+0x930/0x934.
#include "CTR.h"

typedef struct { short x, y, w, h; } RECT;

short D_8008D880 = 0;   /* rect.x (gp+0x914) */
short D_8008D882 = 0;   /* rect.y (gp+0x916) */
short D_8008D884 = 0;   /* rect.w (gp+0x918) */
short D_8008D886 = 0;   /* rect.h (gp+0x91A) */
int   D_8008D2C8 = 0;   /* boolPlayVideoSTR (gp+0x35C) */
short D_8008D89C = 0;   /* videoSTR_dst_vramX (gp+0x930) */
short D_8008D8A0 = 0;   /* videoSTR_dst_vramY (gp+0x934) */

void MainFrame_InitVideoSTR(u32 boolPlayVideoStr, RECT *r, short posX, short posY)
{
    D_8008D880 = r->x;
    D_8008D882 = r->y;
    D_8008D884 = r->w;
    D_8008D886 = r->h;
    D_8008D2C8 = boolPlayVideoStr;
    D_8008D89C = posX;
    D_8008D8A0 = posY;
}
