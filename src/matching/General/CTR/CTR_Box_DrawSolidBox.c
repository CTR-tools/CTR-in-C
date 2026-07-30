#include "CTR.h"

// CTR_Box_DrawSolidBox @ 0x80021894 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH.
// Leaf: one 0x18-byte POLY_F4 (code 0x28) filling the RECT, with the AddPrim INLINED and the DMA
// tag length (5) baked into the tag word as |0x05000000 when linking into ot. colorCode = (rgb & 0xFFFFFF)
// | 0x28000000. Verts packed x|(y<<16). No stack frame / no AddPrim call.

struct RECT { short x, y, w, h; };

void CTR_Box_DrawSolidBox(struct RECT *r, u_int *color, u_int *ot, struct PrimMem *primMem)
{
    void *p;
    void *alloc;

    alloc = primMem->curr;
    p = 0;
    if (alloc <= primMem->endMin100) { p = alloc; primMem->curr = (void *)((int)p + 0x18); }
    if (p != 0)
    {
        *(u_int *)((int)p + 4) = (*color & 0xFFFFFF) | 0x28000000;   /* POLY_F4 code + rgb */
        *(u_int *)((int)p + 8) = (u_short)r->x | (r->y << 16);
        *(u_int *)((int)p + 0xc) = ((r->x + r->w) & 0xFFFF) | (r->y << 16);
        *(u_int *)((int)p + 0x10) = (u_short)r->x | ((r->y + r->h) << 16);
        *(u_int *)((int)p + 0x14) = ((r->x + r->w) & 0xFFFF) | ((r->y + r->h) << 16);
        /* inline AddPrim: link into ot, tag.len = 5 baked into the tag word */
        *(u_int *)((int)p + 0) = (*ot & 0xFFFFFF) | 0x05000000;
        *ot = (u_int)p & 0xFFFFFF;
    }
}
