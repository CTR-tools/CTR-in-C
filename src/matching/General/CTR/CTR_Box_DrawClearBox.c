#include "CTR.h"

// CTR_Box_DrawClearBox @ 0x8002177C (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH.
// A semi-transparent full-box clear: one 0x20-byte DR_TPAGE(E1)+POLY_F4 packet. Outer tag len=7
// (8 words), texpage = (transparency<<5)|0xE1000A00 (E1 draw-mode: semitrans + dither 0x200 + bit11
// 0x800), inner PolyF4 tag.self@8=0, code 0x2A@0xF, r/g/b@0xC-E, 4 verts covering the RECT. AddPrim=func_80075310.

struct RECT { u_short x, y, w, h; };

extern void func_80075310(void *ot, void *p);   // AddPrim (libgpu)

void CTR_Box_DrawClearBox(struct RECT *r, u_char *color, int transparency,
                          void *ot, struct PrimMem *primMem)
{
    void *p;
    void *alloc;

    alloc = primMem->curr;
    p = 0;
    if (alloc <= primMem->endMin100) { p = alloc; primMem->curr = (void *)((int)p + 0x20); }
    if (p != 0)
    {
        *(char *)((int)p + 3) = 7;              /* outer DMA tag len (TPage+PolyF4 = 8 words) */
        *(char *)((int)p + 0xf) = 0x2A;         /* POLY_F4 semi-transparent */
        *(int *)((int)p + 4) = (transparency << 5) | 0xE1000A00;   /* DR_TPAGE (E1) draw-mode word */
        *(int *)((int)p + 8) = 0;
        *(u_char *)((int)p + 0xc) = color[0];
        *(u_char *)((int)p + 0xd) = color[1];
        *(u_char *)((int)p + 0xe) = color[2];
        *(u_short *)((int)p + 0x10) = r->x;
        *(u_short *)((int)p + 0x12) = r->y;
        *(u_short *)((int)p + 0x14) = r->x + r->w;
        *(u_short *)((int)p + 0x16) = r->y;
        *(u_short *)((int)p + 0x18) = r->x;
        *(u_short *)((int)p + 0x1a) = r->y + r->h;
        *(u_short *)((int)p + 0x1c) = r->x + r->w;
        *(u_short *)((int)p + 0x1e) = r->y + r->h;
        func_80075310(ot, p);
    }
}
