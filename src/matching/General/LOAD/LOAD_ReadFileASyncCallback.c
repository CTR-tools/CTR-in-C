// LOAD_ReadFileASyncCallback @ 0x80032110  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 10). Residual: the error-branch flags
// read resolves via reloc D_80083A3C+4 vs the retail`s D_80083A40 (same address,
// different symbol; forcing the exact symbol re-introduces a scheduling reorder).
// CD async read-done ISR cb: CdReadCallback(0); on CdlComplete: if(currSlot.flags
// & LT_SETADDR) ReallocMem(size), then call sdata->callbackCdReadSuccess(currSlot);
// on error: if(flags & LT_SETADDR) MEMPACK_PopState(), set sdata->queueRetry=1.
// NOTE: in-repo decomp is stale/wrong (ReallocMem(0), queueReady/queueLength).
// Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"

struct LoadQueueSlot { char pad0[0x4]; u16 flags; char pad1[0xA]; int size; };   // flags@0x4, size@0x10

extern void func_800771B0(int);   // CdReadCallback
extern void func_8003E94C(int);   // MEMPACK_ReallocMem
extern void func_8003E9D0(void);  // MEMPACK_PopState

extern struct LoadQueueSlot D_80083A3C;   // data.currSlot (absolute)

void (*D_8008D85C)(struct LoadQueueSlot *) = 0;   // sdata->callbackCdReadSuccess (gp+0x8F0)
char D_8008D0A1 = 0;   // sdata->queueRetry (gp+0x135)

void LOAD_ReadFileASyncCallback(u8 result, u8 *unk)
{
    func_800771B0(0);
    result &= 0xff;

    if (result == 2)
    {
        struct LoadQueueSlot *lqs = &D_80083A3C;
        if (lqs->flags & 0x1)
            func_8003E94C(lqs->size);
        if (D_8008D85C != 0)
            D_8008D85C(lqs);
    }
    else
    {
        if (D_80083A3C.flags & 0x1)
            func_8003E9D0();
        D_8008D0A1 = 1;
    }
}
