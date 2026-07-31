// LOAD_CDRequestCallback @ 0x80032d8c  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// CD-request completion callback: if the slot has a callbackFuncPtr, call it
// with the slot; then set sdata->queueReady = 1 (gp+0x134, byte).
#include "CTR.h"

struct LoadQueueSlot {
    char pad0[0x14];
    void (*callback)(struct LoadQueueSlot *);   /* 0x14 */
};

char D_8008D0A0 = 0;   /* sdata->queueReady (gp+0x134) */

void LOAD_CDRequestCallback(struct LoadQueueSlot *lqs)
{
    if (lqs->callback != 0)
        lqs->callback(lqs);
    D_8008D0A0 = 1;
}
