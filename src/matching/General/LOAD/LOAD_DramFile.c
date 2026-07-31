// LOAD_DramFile @ 0x80031e00  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 985 after field-write reorder, grind→565
// via non-natural mutations). Residual: gcc-2.8.1 vs retail register allocation
// on the 3-way dispatcher (retail keeps subfileIndex in a temp t0 and bigfilePtr
// in a0; this gcc promotes both to callee-saved regs). LOAD_ReadFile wrapper
// keyed on callbackOrFlags: -1 = sync read then inline DramFileCallback via a
// stack lqs; -2 = async, store result to data.currSlot(D_80083A48) & *ptrDest;
// else = async with DramFileCallback. All use LT_GETADDR(2).
#include "CTR.h"

struct LoadQueueSlot {
    void *ptrBigfileCdPos;                       // 0x0
    u16 flags;                                   // 0x4
    u16 type;                                    // 0x6
    int subfileIndex;                            // 0x8
    void *ptrDestination;                        // 0xC
    int size;                                    // 0x10
    void (*callback)(struct LoadQueueSlot *);    // 0x14
};

extern void *func_800321B4(void *, int, int, void *, int *, void *);   // LOAD_ReadFile
extern void func_80031D30(struct LoadQueueSlot *);                     // LOAD_DramFileCallback

extern void *D_80083A48;   // data.currSlot.ptrDestination (absolute)

void *LOAD_DramFile(void *bigfilePtr, int subfileIndex, void *ptrDestination, int *sizePtr, int callbackOrFlags)
{
    struct LoadQueueSlot lqs;
    void *loadedFile;

    if (callbackOrFlags == -1)
    {
        lqs.ptrBigfileCdPos = bigfilePtr;
        lqs.flags = 0;
        lqs.type = 2;
        lqs.subfileIndex = subfileIndex;
        loadedFile = func_800321B4(bigfilePtr, 2, subfileIndex, ptrDestination, sizePtr, 0);
        lqs.ptrDestination = loadedFile;
        lqs.callback = 0;
        lqs.size = *sizePtr;
        func_80031D30(&lqs);
        return loadedFile;
    }

    if (callbackOrFlags == -2)
    {
        loadedFile = func_800321B4(bigfilePtr, 2, subfileIndex, 0, sizePtr, func_80031D30);
        D_80083A48 = loadedFile;
        *(void **)ptrDestination = loadedFile;
        return loadedFile;
    }

    return func_800321B4(bigfilePtr, 2, subfileIndex, ptrDestination, sizePtr, func_80031D30);
}
