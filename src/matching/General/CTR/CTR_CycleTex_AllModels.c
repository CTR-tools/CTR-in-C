#include "CTR.h"

// CTR_CycleTex_AllModels @ 0x80021AC0 (SCUS_944.26.exe, 926/NTSC-U).
// BEHAVIOR-EXACT best match (Branch/Stack 0; ~80 residual = gcc uses 5 callee-saved regs vs
// retail 6, shifting the param->s-reg naming; permuter cannot add a register). Logic byte-verified.
// For each of numModels models in pModelArray (stop at a NULL model), walk its headers[] (stride
// 0x40, count @0x12, array @0x14); for each header with animtex@0x3C != 0 and (flags@0x16 & 2)==0,
// call CTR_CycleTex_Model(animtex, timer) (func_80021A20). Inner loop is a pointer walk (header<end).

struct ModelHeader { char pad0[0x16]; u16 flags; char pad1[0x24]; void *animtex; };
struct Model { char pad0[0x12]; s16 numHeaders; struct ModelHeader *headers; };

extern void func_80021A20(void *animtex, int timer);   // CTR_CycleTex_Model

void CTR_CycleTex_AllModels(u_int numModels, struct Model **pModelArray, int timer)
{
    struct Model *pModel;
    struct ModelHeader *pHeader;
    struct ModelHeader *pEnd;

    if (pModelArray == 0) return;
    if (numModels == 0) return;

    while (1)
    {
        pModel = *pModelArray;
        if (pModel == 0) return;

        pHeader = pModel->headers;
        pEnd = pHeader + pModel->numHeaders;
        while (pHeader < pEnd)
        {
            if ((pHeader->animtex != 0) && ((pHeader->flags & 2) == 0))
                func_80021A20(pHeader->animtex, timer);
            pHeader++;
        }

        numModels--;
        if (numModels == 0) return;
        pModelArray++;
    }
}
