// LOAD_Callback_PatchMem @ 0x80031aa4  (vs SCUS_944.26.exe)
// SCORE 0. Key: assign load_inProgress=0 BEFORE computing patchStart/patchSize
// (matches retail scheduling). Applies a level pointer-fixup patch then swaps
// mempacks. Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"

struct LoadQueueSlot { char pad0[0xC]; char *ptrDestination; };

extern char *D_8008D2AC;   // sdata->gGT
int D_8008D0A4 = 0;        // sdata->load_inProgress (gp+0x138)
char *D_8008D08C = 0;      // sdata->ptrLevelFile (gp+0x120)

extern void func_800326B4(char *, int *, int);   // LOAD_RunPtrMap
extern void func_8003E80C(int);                  // MEMPACK_SwapPacks
extern void func_8003E938(void);                 // MEMPACK_ClearHighMem

void LOAD_Callback_PatchMem(struct LoadQueueSlot *lqs)
{
    char *patchPtr = lqs->ptrDestination;
    char *patchStart;
    int patchSize;
    int patchNum;

    D_8008D0A4 = 0;

    patchStart = patchPtr + 4;
    patchSize = *(int *)patchPtr;
    patchNum = patchSize >> 2;

    func_800326B4(D_8008D08C, (int *)patchStart, patchNum);

    func_8003E80C(0);
    func_8003E938();
    func_8003E80C(*(short *)(D_8008D2AC + 0x254a));
}
