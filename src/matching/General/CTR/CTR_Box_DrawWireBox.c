#include "CTR.h"

// CTR_Box_DrawWireBox @ 0x80021594 (SCUS_944.26.exe, 926/NTSC-U).
// BEHAVIOR-EXACT best match (Branch/Stack 0; ~1725 residual = gcc-vs-retail callee-saved regalloc
// of the shared constants 5/0x48/0x55555555). Two LINE_F3 polylines tracing a RECT outline:
// prim1 = (topX,topY)-(botX,topY)-(botX,botY); prim2 = (topX,topY)-(topX,botY)-(botX,botY).
// setlen(5)+setcode(0x48), end@0x14=0x55555555, color r/g/b, 0x18-byte alloc from primMem. AddPrim=func_80075310.


struct RECT { u_short x, y, w, h; };

extern void func_80075310(void *ot, void *p);   // AddPrim (libgpu)

void CTR_Box_DrawWireBox(struct RECT *r, u_char *color, void *ot, struct PrimMem *primMem)
{
    void *p;
    void *alloc;

    alloc = primMem->curr;
    p = 0;
    if (alloc <= primMem->endMin100) { primMem->curr = (void *)((int)alloc + 0x18); p = alloc; }
    if (p != 0)
    {
        *(char *)((int)p + 3) = 5;          /* setlen(5) */
        *(char *)((int)p + 7) = 0x48;       /* LINE_F3 (polyline) */
        *(int *)((int)p + 0x14) = 0x55555555;
        *(u_char *)((int)p + 4) = color[0];
        *(u_char *)((int)p + 5) = color[1];
        *(u_char *)((int)p + 6) = color[2];
        *(u_short *)((int)p + 8) = r->x;
        *(u_short *)((int)p + 0xa) = r->y;
        *(u_short *)((int)p + 0xc) = r->x + r->w;
        *(u_short *)((int)p + 0xe) = r->y;
        *(u_short *)((int)p + 0x10) = r->x + r->w;
        *(u_short *)((int)p + 0x12) = r->y + r->h;
        func_80075310(ot, p);
    }

    alloc = primMem->curr;
    p = 0;
    if (alloc <= primMem->endMin100) { primMem->curr = (void *)((int)alloc + 0x18); p = alloc; }
    if (p != 0)
    {
        *(char *)((int)p + 3) = 5;
        *(char *)((int)p + 7) = 0x48;
        *(int *)((int)p + 0x14) = 0x55555555;
        *(u_char *)((int)p + 4) = color[0];
        *(u_char *)((int)p + 5) = color[1];
        *(u_char *)((int)p + 6) = color[2];
        *(u_short *)((int)p + 8) = r->x;
        *(u_short *)((int)p + 0xa) = r->y;
        *(u_short *)((int)p + 0xc) = r->x;
        *(u_short *)((int)p + 0xe) = r->y + r->h;
        *(u_short *)((int)p + 0x10) = r->x + r->w;
        *(u_short *)((int)p + 0x12) = r->y + r->h;
        func_80075310(ot, p);
    }
}
