// LOAD_Hub_ReadFile @ 0x80032ffc  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Loads an adventure-hub level into the OTHER mempack (double-buffered hubs):
// if that pack already holds the level, return; else modelMaskHints3D=0, swap to
// the other pack, clear low mem, mark load_inProgress, level2=0, record the levID,
// then queue VRAM + LEV(GETADDR,LOAD_Callback_LEV) + PTR(SETADDR,PatchMem_Ptr,
// LOAD_HubCallback) via LOAD_GetBigfileIndex(levID,1,LVI_*).
// Match levers: access gGT via D_8008D2AC directly (retail reloads it per block,
// not a cached saved reg) and init PatchMem_Ptr `= 0` so it is gp-relative.
#include "CTR.h"

struct GameTracker {
    char pad0[0x164];
    int level2;                        /* 0x164 */
    char pad1[0x254C - 0x168];
    short levID_in_each_mempack[8];    /* 0x254C */
};

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
int   D_8008D0F0 = 0;   /* sdata->modelMaskHints3D (gp+0x184) */
int   D_8008D0A4 = 0;   /* sdata->load_inProgress  (gp+0x138) */
void *D_8008D090 = 0;   /* sdata->PatchMem_Ptr      (gp+0x124) */

extern void func_8003E80C(int packID);                                       /* MEMPACK_SwapPacks   */
extern void func_8003E9B8(void);                                             /* MEMPACK_ClearLowMem */
extern int  func_80032C24(int levID, int lod, int fileIndexInGroup);         /* LOAD_GetBigfileIndex */
extern void func_80032D30(void *bigfile, int type, int index, void *dest, void *cb); /* LOAD_AppendQueue */
extern void func_80031A78(void);   /* LOAD_Callback_LEV  */
extern void func_80031B14(void);   /* LOAD_HubCallback   */

void LOAD_Hub_ReadFile(void *bigfile, int levID, int packID)
{
    if (D_8008D2AC->levID_in_each_mempack[packID] == levID)
        return;

    D_8008D0F0 = 0;
    func_8003E80C(packID);
    func_8003E9B8();
    D_8008D0A4 = 1;
    D_8008D2AC->level2 = 0;
    D_8008D2AC->levID_in_each_mempack[packID] = levID;

    func_80032D30(bigfile, 3, func_80032C24(levID, 1, 0), 0, 0);
    func_80032D30(bigfile, 2, func_80032C24(levID, 1, 1), 0, func_80031A78);
    func_80032D30(bigfile, 1, func_80032C24(levID, 1, 2), D_8008D090, func_80031B14);
}
