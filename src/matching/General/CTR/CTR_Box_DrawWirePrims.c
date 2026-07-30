#include "CTR.h"

// CTR_Box_DrawWirePrims @ 0x80021500 (SCUS_944.26.exe, 926/NTSC-U).
// BEHAVIOR-EXACT best match -- Branch/Stack diffs = 0; residual is a gcc-2.8.1-vs-retail
// register-allocation/load-scheduling difference (primMem in t0 vs v1; r/g/b stack-arg loads
// scheduled slightly differently), unforceable from C (permuter floor ~570). Logic verified byte-
// for-byte vs the Ghidra decompile + disassembly.
//
// Builds one PsyQ LINE_F2 (setLineF2 = setlen(3)+setcode(0x40)) from (x0,y0)-(x1,y1) in color
// (r,g,b), bump-allocated from primMem (0x10 bytes; curr@0xC, endMin100@0x10), then AddPrim into ot.
// No-op if primMem is full. Colors are passed as full stack words (lw), hence int params.
// AddPrim = func_80075310 (libgpu). Prim layout: tag.len@3, code@7, r@4/g@5/b@6, x0@8/y0@A/x1@C/y1@E.

extern void func_80075310(void *ot, void *p);   // AddPrim (libgpu)

void CTR_Box_DrawWirePrims(u_short x0, u_short y0, u_short x1, u_short y1,
                           int red, int green, int blue,
                           void *ot, struct PrimMem *primMem)
{
    void *p;
    void *pPrimAlloc;

    pPrimAlloc = primMem->curr;
    p = 0;
    if (pPrimAlloc <= primMem->endMin100)
    {
        primMem->curr = (void *)((int)pPrimAlloc + 0x10);
        p = pPrimAlloc;
    }
    if (p != 0)
    {
        *(char *)((int)p + 3) = 3;          /* setlen(p, 3)  */
        *(char *)((int)p + 7) = 0x40;       /* setcode(p, 0x40) = LINE_F2 */
        *(u_char *)((int)p + 4) = red;
        *(u_char *)((int)p + 5) = green;
        *(u_short *)((int)p + 8) = x0;
        *(u_short *)((int)p + 10) = y0;
        *(u_short *)((int)p + 0xc) = x1;
        *(u_short *)((int)p + 0xe) = y1;
        *(u_char *)((int)p + 6) = blue;
        func_80075310(ot, p);
    }
}
