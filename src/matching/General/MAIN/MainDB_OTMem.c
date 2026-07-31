// MainDB_OTMem @ 0x80034a28  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Inits an OT (ordering-table) allocator: MEMPACK_AllocMem(size),
// capacityBytes=size, cursor/start = alloc, end = alloc + (size aligned down to 4).
#include "CTR.h"

struct DBOTMem {
    u32   capacityBytes;   /* 0x0 */
    void *start;           /* 0x4 */
    void *end;             /* 0x8 */
    void *cursor;          /* 0xC */
};

extern void *func_8003E874(u32 size, char *name);   /* MEMPACK_AllocMem */
extern char D_8008D2C0[];                           /* "ot mem" tag */

void MainDB_OTMem(struct DBOTMem *otMem, u32 size)
{
    void *p = func_8003E874(size, D_8008D2C0);
    u32 alignedSize;

    otMem->capacityBytes = size;
    alignedSize = (size >> 2) << 2;
    otMem->cursor = p;
    otMem->start = p;
    otMem->end = (void *)((int)p + alignedSize);
}
