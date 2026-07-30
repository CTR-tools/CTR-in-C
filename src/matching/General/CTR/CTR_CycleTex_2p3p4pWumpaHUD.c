#include "CTR.h"

// CTR_CycleTex_2p3p4pWumpaHUD @ 0x80021B94 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH.
// Wumpa-HUD frame cycler: copy the current active tex to ptrArray[0], then point ptrActiveTex at the
// last frame slot &ptrArray[numFrames-1], masked to a 24-bit OT link (CtrGpu_PrimToOTLink24). The -4
// is applied to the pointer (ptrArray+numFrames)-4, not the element offset, to match retail codegen.

void CTR_CycleTex_2p3p4pWumpaHUD(u_int *ptrActiveTex, u_int *ptrArray, int numFrames)
{
    ptrArray[0] = ptrActiveTex[0];
    ptrActiveTex[0] = ((u_int)(ptrArray + numFrames) - 4) & 0xFFFFFF;
}
