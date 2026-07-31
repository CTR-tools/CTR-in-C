// LOAD_OvrThreads @ 0x80033570  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 60 = same `lui a3` dest-addr schedule
// reorder as LOAD_OvrLOD/OvrEndRace; not permuter-fixable). Behavior identical.
// Loads the Threads overlay (230-233) if not current: mark load_inProgress,
// invalidate overlayIndex_Threads (=0xff), queue BI_OVERLAYSECT3 + param_1 into
// OVR_Region3 with the per-overlay callback overlayCallbackFuncs[param_1].
#include "CTR.h"

struct GameTracker {
    char pad0[0x2541];
    unsigned char overlayIndex_Threads;   /* 0x2541 */
};

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
int   D_8008D0A4 = 0;   /* sdata->load_inProgress   (gp+0x138) */
void *D_8008D09C = 0;   /* sdata->ptrBigfileCdPos_2 (gp+0x130) */
extern char  D_800AB9F0[];   /* OVR_Region3 (absolute) */
extern void *D_80083A70[];   /* data.overlayCallbackFuncs (absolute) */

extern void func_80032D30(void *bigfile, int type, int index, void *dest, void *cb);  /* LOAD_AppendQueue */

void LOAD_OvrThreads(u32 param_1)
{
    if ((u32)D_8008D2AC->overlayIndex_Threads != param_1)
    {
        D_8008D0A4 = 1;
        D_8008D2AC->overlayIndex_Threads = 0xff;
        func_80032D30(D_8008D09C, 1, param_1 + 0xE6, D_800AB9F0, D_80083A70[param_1]);
    }
}
