// LOAD_OvrEndRace @ 0x800334f4  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 60 = single `lui a3` dest-addr schedule
// reorder, identical to LOAD_OvrLOD; not permuter-fixable). Behavior identical.
// Loads the EndOfRace overlay (221-225) for the requested slot if not current:
// mark load_inProgress, queue BI_OVERLAYSECT1 + param_1 into OVR_Region1 with
// LOAD_Callback_Overlay_Generic, record overlayIndex_EndOfRace, invalidate
// overlayIndex_LOD (=0xff).
#include "CTR.h"

struct GameTracker {
    char pad0[0x2540];
    unsigned char overlayIndex_LOD;        /* 0x2540 */
    unsigned char pad1[2];                 /* 0x2541-0x2542 */
    unsigned char overlayIndex_EndOfRace;  /* 0x2543 */
};

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
int   D_8008D0A4 = 0;   /* sdata->load_inProgress   (gp+0x138) */
void *D_8008D09C = 0;   /* sdata->ptrBigfileCdPos_2 (gp+0x130) */
extern char D_8009F6FC[];   /* OVR_Region1 (absolute) */

extern void func_80032D30(void *bigfile, int type, int index, void *dest, void *cb);  /* LOAD_AppendQueue */
extern void func_800319E8(void);   /* LOAD_Callback_Overlay_Generic */

void LOAD_OvrEndRace(u32 param_1)
{
    if ((u32)D_8008D2AC->overlayIndex_EndOfRace != param_1)
    {
        D_8008D0A4 = 1;
        func_80032D30(D_8008D09C, 1, param_1 + 0xDD, D_8009F6FC, func_800319E8);
        D_8008D2AC->overlayIndex_EndOfRace = param_1;
        D_8008D2AC->overlayIndex_LOD = 0xff;
    }
}
