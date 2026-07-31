// MainDB_PrimMem @ 0x800349c4  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Inits a PrimMem allocator: MEMPACK_AllocMem(size), capacityBytes=size,
// allocationStart/cursor/start = alloc, end = alloc + (size aligned down to 4),
// guardEnd = end - 0x100. (local struct renamed DBPrimMem to avoid CTR.h clash.)
#include "CTR.h"

struct DBPrimMem {
    u32   capacityBytes;    /* 0x0  */
    void *allocationStart;  /* 0x4  */
    void *end;              /* 0x8  */
    void *cursor;           /* 0xC  */
    void *guardEnd;         /* 0x10 */
    char  pad_18[0x18 - 0x14];
    void *start;            /* 0x18 */
};

extern void *func_8003E874(u32 size, char *name);   /* MEMPACK_AllocMem */
extern char D_8008D2B8[];                           /* "prim mem" tag */

void MainDB_PrimMem(struct DBPrimMem *primMem, u32 size)
{
    void *p = func_8003E874(size, D_8008D2B8);
    u32 alignedSize;

    primMem->capacityBytes = size;
    alignedSize = (size >> 2) << 2;
    primMem->start = p;
    primMem->cursor = p;
    primMem->allocationStart = p;
    primMem->end = (void *)((int)p + alignedSize);
    primMem->guardEnd = (void *)((int)p + alignedSize - 0x100);
}
