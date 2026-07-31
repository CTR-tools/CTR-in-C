// LOAD_VramFile @ 0x80031fdc  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 70, grind floor). Residual: one
// lqs.callback=0 scheduling reorder + the maspsx absolute-store at-macro on the
// -2 branch (D_80083A48 = loadedFile; same wall as LOAD_Callback_Podiums).
// LOAD_ReadFile wrapper keyed on callbackOrFlags (LT_VRAM=3), wraps in
// MEMPACK_Push/PopState when ptrDestination==NULL: -1 = sync read then inline
// VramFileCallback via a stack lqs + VSync(2) + frameFinishedVRAM=0; -2 = async,
// store result to data.currSlot(D_80083A48) & *ptrDest; else = async w/ callback.
// Key match: return lqs.ptrDestination (gcc reloads the stack slot). Score with GP_VALUE=0x8008CF6C.
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

extern void func_8003E978(void);                                      // MEMPACK_PushState
extern void func_8003E9D0(void);                                      // MEMPACK_PopState
extern void *func_800321B4(void *, int, int, void *, int *, void *);  // LOAD_ReadFile
extern void func_80031EE4(struct LoadQueueSlot *);                    // LOAD_VramFileCallback
extern void func_80075350(int);                                       // VSync

extern void *D_80083A48;   // data.currSlot.ptrDestination (absolute)
int D_8008D0A8 = 0;        // sdata->frameFinishedVRAM (gp+0x13C)

void *LOAD_VramFile(void *bigfilePtr, int subfileIndex, void *ptrDestination, int *sizePtr, int callbackOrFlags)
{
    struct LoadQueueSlot lqs;
    void *loadedFile;

    if (ptrDestination == 0)
        func_8003E978();

    if (callbackOrFlags == -1)
    {
        lqs.ptrBigfileCdPos = bigfilePtr;
        lqs.flags = 0;
        lqs.type = 3;
        lqs.subfileIndex = subfileIndex;
        loadedFile = func_800321B4(bigfilePtr, 3, subfileIndex, ptrDestination, sizePtr, 0);
        lqs.ptrDestination = loadedFile;
        lqs.callback = 0;
        lqs.size = *sizePtr;
        func_80031EE4(&lqs);
        func_80075350(2);
        D_8008D0A8 = 0;
        if (ptrDestination == 0)
            func_8003E9D0();
        return lqs.ptrDestination;
    }

    if (callbackOrFlags == -2)
    {
        loadedFile = func_800321B4(bigfilePtr, 3, subfileIndex, 0, sizePtr, func_80031EE4);
        D_80083A48 = loadedFile;
        *(void **)ptrDestination = loadedFile;
        return loadedFile;
    }

    return func_800321B4(bigfilePtr, 3, subfileIndex, ptrDestination, sizePtr, func_80031EE4);
}
