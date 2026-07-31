// MainFrame_ResetDB @ 0x80034bbc  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 400, grind found nothing lower).
// Residual: gcc-2.8.1 vs retail register allocation in the per-player ptrOT loop
// (slice temp v0 vs v1) + loop-guard branch scheduling. Behavior identical.
// Per-frame double-buffer flip + OT reset: pump the hub loader, toggle
// swapchainIndex (0<->1), repoint backBuffer at db[newIdx] (stride 0xA4), bump
// frameTimer, reset backBuffer primMem/otMem cursors, ClearOTagR the UI OT
// (numPlyr<<10|6), point each active players tileView ptrOT at its 0x1000-word OT
// slice (reverse order), the unused views + UI view at the +0x3018 tail slice,
// then set pushBuffer_UI/otMem.uiOT to OT base+4. gGT + ptrBigfile1 gp-relative.
// Offsets decoded from asm: db[]@0x18 stride 0xA4, tileView ptrOT@0x25C stride 0x110,
// otSwapchainDB[]@0x18C8, numPlyr@0x1CA8, frameTimer@0x1CF8, pushBuffer_UI@0x147C.
#include "CTR.h"

struct DB {
    char pad_70[0x70];
    unsigned char blurCameraMask;   /* 0x70 */
    char pad_78[0x78 - 0x71];
    void *primStart;                /* 0x78 */
    char pad_80[0x80 - 0x7C];
    void *primCursor;               /* 0x80 */
    char pad_88[0x88 - 0x84];
    int primitiveCount;             /* 0x88 */
    char pad_94[0x94 - 0x8C];
    void *otStart;                  /* 0x94 */
    char pad_9C[0x9C - 0x98];
    void *otCursor;                 /* 0x9C */
    void *otUiOT;                   /* 0xA0 */
};

struct GameTracker {
    char pad_C[0xC];
    int swapchainIndex;             /* 0xC  */
    struct DB *backBuffer;          /* 0x10 */
    char pad_18[0x18 - 0x14];
    struct DB db[2];                /* 0x18 (stride 0xA4) */
    char pad_147C[0x147C - (0x18 + 2 * 0xA4)];
    void *pushBuffer_UI_ptrOT;      /* 0x147C */
    char pad_18C8[0x18C8 - 0x1480];
    void *otSwapchainDB[2];         /* 0x18C8 */
    char pad_1CA8[0x1CA8 - 0x18D0];
    unsigned char numPlyrCurrGame;  /* 0x1CA8 */
    char pad_1CF8[0x1CF8 - 0x1CA9];
    int frameTimer_MainFrame_ResetDB; /* 0x1CF8 */
};

/* pushBuffer[i].ptrOT lives at gGT + i*0x110 + 0x25C */
#define PUSHBUF_PTROT(gt, i) (*(void **)((char *)(gt) + (i) * 0x110 + 0x25C))

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */
void *D_8008D86C = 0;                 /* sdata->ptrBigfile1 (gp+0x900) */

extern void func_80033318(void *bigfile);   /* LOAD_Hub_Main */
extern void func_80021C8C(void);            /* CTR_EmptyFunc_MainFrame_ResetDB */
extern void func_80022B94(struct GameTracker *gGT); /* DecalGlobal_EmptyFunc_MainFrame_ResetDB */
extern void func_800732EC(void *ot, int blocks); /* ClearOTagR */

void MainFrame_ResetDB(struct GameTracker *gGT)
{
    int newIndex;
    int i;
    struct DB *db;
    void *ot;
    void *uiOt;

    func_80033318(D_8008D86C);

    newIndex = 1 - gGT->swapchainIndex;
    gGT->backBuffer = (struct DB *)((char *)gGT + newIndex * 0xA4 + 0x18);
    gGT->swapchainIndex = newIndex;
    gGT->frameTimer_MainFrame_ResetDB++;

    gGT->backBuffer->blurCameraMask = 0;
    gGT->backBuffer->primCursor = gGT->backBuffer->primStart;
    gGT->backBuffer->primitiveCount = 0;
    gGT->backBuffer->otCursor = gGT->backBuffer->otStart;

    func_80021C8C();
    func_80022B94(gGT);

    func_800732EC(gGT->otSwapchainDB[gGT->swapchainIndex], D_8008D2AC->numPlyrCurrGame << 10 | 6);

    for (i = 0; i < (int)D_8008D2AC->numPlyrCurrGame; i++)
        PUSHBUF_PTROT(gGT, i) = (void *)((char *)gGT->otSwapchainDB[gGT->swapchainIndex] + (D_8008D2AC->numPlyrCurrGame - i - 1) * 0x1000 + 0x18);

    for (; i < 4; i++)
        PUSHBUF_PTROT(gGT, i) = (void *)((char *)gGT->otSwapchainDB[gGT->swapchainIndex] + 3 * 0x1000 + 0x18);

    db = gGT->backBuffer;
    uiOt = (void *)((char *)gGT->otSwapchainDB[gGT->swapchainIndex] + 4);
    gGT->pushBuffer_UI_ptrOT = uiOt;
    db->otUiOT = uiOt;
}
