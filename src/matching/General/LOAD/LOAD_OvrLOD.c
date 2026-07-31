// LOAD_OvrLOD @ 0x80033474  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 60 = single instruction-schedule reorder
// of the dest address `lui a3` placement; not permuter-fixable). Behavior identical.
// Loads the LOD-overlay (226-229) for the requested LOD if not already current:
// mark load_inProgress, queue BI_OVERLAYSECT2 + (lod-1) into OVR_Region2 with
// LOAD_Callback_Overlay_Generic, then record overlayIndex_LOD and invalidate
// overlayIndex_Threads (=0xff). Match lever: keep the ORIGINAL param for the index
// (param_1 + 0xE1) while a separate newLOD=param_1-1 drives the check/store.
#include "CTR.h"

struct GameTracker {
    char pad0[0x2540];
    unsigned char overlayIndex_LOD;      /* 0x2540 */
    unsigned char overlayIndex_Threads;  /* 0x2541 */
};

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
int   D_8008D0A4 = 0;   /* sdata->load_inProgress   (gp+0x138) */
void *D_8008D09C = 0;   /* sdata->ptrBigfileCdPos_2 (gp+0x130) */
extern char D_800A0CB8[];   /* OVR_Region2 (absolute) */

extern void func_80032D30(void *bigfile, int type, int index, void *dest, void *cb);  /* LOAD_AppendQueue */
extern void func_800319E8(void);   /* LOAD_Callback_Overlay_Generic */

void LOAD_OvrLOD(u32 param_1)
{
    u32 newLOD = param_1 - 1;   /* {1-4} -> {0-3} */

    if ((u32)D_8008D2AC->overlayIndex_LOD != newLOD)
    {
        D_8008D0A4 = 1;
        /* BI_OVERLAYSECT2(0xE2) + newLOD == param_1 + 0xE1 (retail keeps original param) */
        func_80032D30(D_8008D09C, 1, param_1 + 0xE1, D_800A0CB8, func_800319E8);
        D_8008D2AC->overlayIndex_LOD = newLOD;
        D_8008D2AC->overlayIndex_Threads = 0xff;
    }
}
