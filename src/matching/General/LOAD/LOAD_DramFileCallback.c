// LOAD_DramFileCallback @ 0x80031d30  (vs SCUS_944.26.exe)
// SCORE 0. Async DRAM-file load-done cb. If ptrDestination!=0: read the
// pointer-map offset header, LOAD_RunPtrMap the fixups; if (flags & LT_SETADDR
// =0x1) ReallocMem(offset+4); else flags |= LT_GETADDR(0x2); ptrDestination+=4.
// Then invoke lqs->callback only if it is a valid 0x80xxxxxx pointer; set
// sdata->queueReady=1 (gp+0x134). NOTE: in-repo decomp is heavily incomplete
// here (missing the fileBuf guard, the <0 branch, the flag gate, ptr bump,
// callback dispatch, and queueReady). Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"

struct LoadQueueSlot {
    char pad0[0x4];
    u16 flags;                                    // 0x4
    char pad1[0x6];
    char *ptrDestination;                         // 0xC
    char pad2[0x4];
    void (*callback)(struct LoadQueueSlot *);     // 0x14
};

struct DramPointerMap { int numBytes; int offsets[1]; };

extern void func_800326B4(char *, int *, int);    // LOAD_RunPtrMap
extern void func_8003E94C(int);                   // MEMPACK_ReallocMem

char D_8008D0A0 = 0;   // sdata->queueReady (gp+0x134)

void LOAD_DramFileCallback(struct LoadQueueSlot *lqs)
{
    char *fileBuf = lqs->ptrDestination;

    if (fileBuf != 0)
    {
        int ptrMapOffset = *(int *)fileBuf;
        char *realFileBuf = fileBuf + 4;

        if (ptrMapOffset >= 0)
        {
            struct DramPointerMap *dpm = (struct DramPointerMap *)(realFileBuf + ptrMapOffset);
            func_800326B4(realFileBuf, dpm->offsets, dpm->numBytes >> 2);
            if (lqs->flags & 1)
                func_8003E94C(ptrMapOffset + 4);
        }
        else
        {
            lqs->flags |= 0x2;
        }
        lqs->ptrDestination += 4;
    }

    if (lqs->callback != 0 && ((unsigned int)lqs->callback & 0xff000000) == 0x80000000)
        lqs->callback(lqs);

    D_8008D0A0 = 1;   // sdata->queueReady = 1
}
