// LOAD_RunPtrMap @ 0x800326b4  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 10). Residual: the address computation
// `origin + offset` emits `addu v0,a0,v0` where retail canonicalizes the
// commutative add as `addu v0,v0,a0` (offset first) — same result.
// Pointer-relocation pass: for each patchArr[i] (a byte-offset, aligned via
// (>>2)<<2), the int word at origin[offset] is fixed up by adding origin.
// Key: an explicit `end` var avoids an extra move for the loop bound.
#include "CTR.h"

void LOAD_RunPtrMap(char *origin, int *patchArr, int numPtrs)
{
    int *ptrCurrOffset;
    int *end = &patchArr[numPtrs];

    for (ptrCurrOffset = patchArr; ptrCurrOffset < end; ptrCurrOffset++)
    {
        int offset = (*ptrCurrOffset >> 2) << 2;
        *(int *)&origin[offset] = *(int *)&origin[offset] + (int)origin;
    }
}
