// LOAD_AppendQueue @ 0x80032d30  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Appends a LoadQueueSlot (0x18 stride) to the load queue if queueCount < 8:
// flags=0, ptrBigfileCdPos, type, subfileIndex, ptrDestination, size=0, callback,
// then queueCount++. Key match lever: assign the fields via direct array-member
// access D_80096344[qc].field (NOT a cached `slot` pointer) so gcc materializes
// the queue base address before the index (retail ordering). queueCount is
// gp-relative (gp+0x136), loaded lh (signed, compare+index) & lhu (increment).
#include "CTR.h"

struct LoadQueueSlot {
    void *ptrBigfileCdPos;   /* 0x0  */
    u16 flags;               /* 0x4  */
    u16 type;                /* 0x6  */
    int subfileIndex;        /* 0x8  */
    void *ptrDestination;    /* 0xC  */
    int size;                /* 0x10 */
    void *callback;          /* 0x14 */
};

extern struct LoadQueueSlot D_80096344[];   /* load queue array (absolute) */
short D_8008D0A2 = 0;                        /* loadQueueCount (gp+0x136) */

void LOAD_AppendQueue(void *bigfilePtr, int type, int subfileIndex, void *dest, void *callback)
{
    short qc = D_8008D0A2;

    if (qc < 8)
    {
        D_80096344[qc].flags = 0;
        D_80096344[qc].ptrBigfileCdPos = bigfilePtr;
        D_80096344[qc].type = type;
        D_80096344[qc].subfileIndex = subfileIndex;
        D_80096344[qc].ptrDestination = dest;
        D_80096344[qc].size = 0;
        D_80096344[qc].callback = callback;
        D_8008D0A2 = qc + 1;
    }
}
