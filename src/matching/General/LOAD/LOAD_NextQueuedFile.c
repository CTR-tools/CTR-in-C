// LOAD_NextQueuedFile @ 0x80032dc0  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 705, grind found nothing lower).
// Residual: gcc-2.8.1 reuses the queueSlots base loaded for the currSlot copy as
// the shift-loop induction base (index addressing), while retail loads a fresh
// D_8009632C = queueSlots-1elem base with pure pointer induction; plus a minor
// currSlot-copy address schedule. Behavior identical.
// Pumps the load queue: guarded by queueReady && !XA_State && queueLength, either
// clears queueRetry or copies queueSlots[0] to currSlot and shifts the queue down,
// then dispatches on type (RAW=ReadFile_ex / DRAM=DramFile / VRAM=VramFile) and
// decrements queueLength; finally the frameFinishedVRAM completion (>=3 frames):
// fire callback, PopState if flags&LT_SETADDR, re-arm queueReady.
// Match levers: access currSlot via D_80083A3C directly (retail re-materializes
// the address per block) and invert the queueRetry if/else (copy path first).
#include "CTR.h"

struct LoadQueueSlot {
    void *ptrBigfileCdPos;   /* 0x0  */
    u16 flags;               /* 0x4  */
    u16 type;                /* 0x6  */
    int subfileIndex;        /* 0x8  */
    void *ptrDestination;    /* 0xC  */
    int size;                /* 0x10 */
    void (*callback)(struct LoadQueueSlot *);   /* 0x14 */
};

struct GameTracker { char pad0[0x1CE4]; int frameTimer; };   /* frameTimer_VsyncCallback @0x1CE4 */

extern struct LoadQueueSlot D_80083A3C;      /* data.currSlot (absolute) */
extern struct LoadQueueSlot D_80096344[];    /* load queue slots (absolute) */
extern int D_8008D708;                       /* sdata->XA_State (absolute) */
extern struct GameTracker *D_8008D2AC;       /* sdata->gGT (absolute) */

char  D_8008D0A0 = 0;   /* sdata->queueReady       (gp+0x134) */
char  D_8008D0A1 = 0;   /* sdata->queueRetry       (gp+0x135) */
short D_8008D0A2 = 0;   /* sdata->queueLength      (gp+0x136) */
int   D_8008D0A8 = 0;   /* sdata->frameFinishedVRAM (gp+0x13C) */

extern void *func_800321B4(void *bigfile, int type, int index, void *dest, int *sizePtr, void *cb); /* LOAD_ReadFile_ex */
extern void *func_80031E00(void *bigfile, int index, void *dest, int *sizePtr, int cbOrFlags);      /* LOAD_DramFile   */
extern void *func_80031FDC(void *bigfile, int index, void *dest, int *sizePtr, int cbOrFlags);      /* LOAD_VramFile   */
extern void  func_80032D8C(struct LoadQueueSlot *);   /* LOAD_CDRequestCallback */
extern void  func_8003E9D0(void);                     /* MEMPACK_PopState */

void LOAD_NextQueuedFile(void)
{
    if (D_8008D0A0 != 0 && D_8008D708 == 0 && D_8008D0A2 != 0)
    {
        int i;

        D_8008D0A0 = 0;

        if (D_8008D0A1 == 0)
        {
            D_80083A3C = D_80096344[0];
            for (i = 1; i < D_8008D0A2; i++)
                D_80096344[i - 1] = D_80096344[i];
        }
        else
        {
            D_8008D0A1 = 0;
        }

        switch (D_80083A3C.type)
        {
        case 1:
            D_80083A3C.ptrDestination = func_800321B4(D_80083A3C.ptrBigfileCdPos, 1, D_80083A3C.subfileIndex, D_80083A3C.ptrDestination, &D_80083A3C.size, func_80032D8C);
            break;
        case 2:
            D_80083A3C.ptrDestination = func_80031E00(D_80083A3C.ptrBigfileCdPos, D_80083A3C.subfileIndex, D_80083A3C.ptrDestination, &D_80083A3C.size, (int)D_80083A3C.callback);
            break;
        case 3:
            D_80083A3C.ptrDestination = func_80031FDC(D_80083A3C.ptrBigfileCdPos, D_80083A3C.subfileIndex, D_80083A3C.ptrDestination, &D_80083A3C.size, (int)D_80083A3C.callback);
            break;
        }

        D_8008D0A2--;
    }

    if (D_8008D0A8 != 0)
    {
        if ((u32)(D_8008D2AC->frameTimer - D_8008D0A8) >= 3)
        {
            if (D_80083A3C.callback != 0)
                D_80083A3C.callback(&D_80083A3C);

            D_8008D0A8 = 0;

            if (D_80083A3C.flags & 0x1)
                func_8003E9D0();

            D_8008D0A0 = 1;
        }
    }
}
