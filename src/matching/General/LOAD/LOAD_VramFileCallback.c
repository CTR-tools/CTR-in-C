// LOAD_VramFileCallback @ 0x80031ee4  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 1365). Residual: gcc block-ordering of
// the two mutually-exclusive single-TIM/multi-TIM if-branches (retail lays out
// multi as fall-through, single at the branch target; this gcc does the reverse).
// Uploads TIM image(s) to VRAM via LoadImage: single TIM if vramBuf[0]!=0x20,
// else a packed list (0x20 marker) walked by int-ptr stride (size>>2); rect is
// copied field-by-field to a local RECT (retail lhu/sh). Ends by latching
// sdata->frameFinishedVRAM = gGT->frameTimer_VsyncCallback (gp+0x13C = gGT+0x1CE4).
// NOTE: in-repo decomp end is WRONG (frameFinishedVRAM=2; queueReady=0). Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"

typedef struct { short x, y, w, h; } RECT;
struct VramHeader { int pad0[3]; RECT rect; };   // rect@0xC (aligned), pixels@0x14

struct LoadQueueSlot { char pad0[0xC]; int *ptrDestination; };

extern char *D_8008D2AC;                     // sdata->gGT
extern void func_80073174(RECT *, void *);   // LoadImage

int D_8008D0A8 = 0;   // sdata->frameFinishedVRAM (gp+0x13C)

void LOAD_VramFileCallback(struct LoadQueueSlot *lqs)
{
    int *vramBuf = lqs->ptrDestination;
    struct VramHeader *vh = (struct VramHeader *)vramBuf;
    RECT rect;

    if (vramBuf != 0 && vramBuf[0] != 0x20)
    {
        rect.x = vh->rect.x;
        rect.y = vh->rect.y;
        rect.w = vh->rect.w;
        rect.h = vh->rect.h;
        func_80073174(&rect, (char *)vh + 0x14);
    }

    if (vramBuf != 0 && vramBuf[0] == 0x20)
    {
        int size;
        vramBuf++;
        size = vramBuf[0];
        vh = (struct VramHeader *)&vramBuf[1];
        while (size != 0)
        {
            rect.x = vh->rect.x;
            rect.y = vh->rect.y;
            rect.w = vh->rect.w;
            rect.h = vh->rect.h;
            func_80073174(&rect, (char *)vh + 0x14);
            vramBuf = (int *)vh + (size >> 2);
            size = vramBuf[0];
            vh = (struct VramHeader *)&vramBuf[1];
        }
    }

    D_8008D0A8 = *(int *)(D_8008D2AC + 0x1ce4);
}
