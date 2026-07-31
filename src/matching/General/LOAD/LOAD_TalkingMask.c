// LOAD_TalkingMask @ 0x800347d0  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 105; grind plateau 10 non-natural temp).
// Residual: gcc-2.8.1 folds the per-call constant (0x1B6/0x1B7) into maskID*4
// while retail folds it into (packID-1)*2 (constant-reassociation choice) +
// a reg-alloc detail. Behavior identical.
// Loads a talking-mask (AdvHub NPC): wipe modelMaskHints3D, invalidate the target
// packs levID, swap+clear that pack, mark load_inProgress, then queue the mask VRAM
// (LT_VRAM) and its PTR (LT_GETADDR, LOAD_Callback_MaskHints3D) at
// BI_UKAHEAD(0x1B6) + maskID*4 + (packID-1)*2 (+1 for the PTR).
#include "CTR.h"

struct GameTracker { char pad0[0x254C]; short levID_in_each_mempack[8]; };   /* @0x254C */

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
int   D_8008D0F0 = 0;   /* sdata->modelMaskHints3D  (gp+0x184) */
int   D_8008D0A4 = 0;   /* sdata->load_inProgress   (gp+0x138) */
void *D_8008D09C = 0;   /* sdata->ptrBigfileCdPos_2 (gp+0x130) */

extern void func_8003E80C(int packID);   /* MEMPACK_SwapPacks   */
extern void func_8003E9B8(void);         /* MEMPACK_ClearLowMem */
extern void func_80032D30(void *bigfile, int type, int index, void *dest, void *cb);  /* LOAD_AppendQueue */
extern void func_80031A50(void);         /* LOAD_Callback_MaskHints3D */

void LOAD_TalkingMask(int packID, int maskID)
{
    D_8008D0F0 = 0;
    D_8008D2AC->levID_in_each_mempack[packID] = -1;
    func_8003E80C(packID);
    func_8003E9B8();
    D_8008D0A4 = 1;

    func_80032D30(D_8008D09C, 3, maskID * 4 + ((packID - 1) * 2 + 0x1B6), 0, 0);
    func_80032D30(D_8008D09C, 2, maskID * 4 + ((packID - 1) * 2 + 0x1B7), 0, func_80031A50);
}
