// MainLoadVLC @ 0x8003c518  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Allocs + queues the VLC (video decode) table: clears bool_IsLoaded_VlcTable
// (gp+0x494), reads entry[BI_VLCTABLE=0x1E0].size (bigfile+0xF0C), rounds up to a
// 0x800 multiple via (size+0x7ff)/0x800*0x800 (retail signed-div form, not & mask),
// MEMPACK_AllocMem it into ptrVlcTable (gp+0x490), then LOAD_AppendQueue(bigfile,
// LT_SETADDR, BI_VLCTABLE, ptrVlcTable, MainLoadVLC_Callback). ptrBigfileCdPos_2
// (D_8008D09C) accessed absolute + reloaded for the queue call. Match lever: load
// size BEFORE bool=0 so the store fills the load-delay slot (no nop).
#include "CTR.h"

#define LT_SETADDR  1
#define BI_VLCTABLE 0x1E0

extern void *D_8008D09C;    /* sdata->ptrBigfileCdPos_2 (absolute) */
extern char  D_8001146C[];  /* MEMPACK_AllocMem tag ("vlc table") */
void *D_8008D3FC = 0;       /* sdata->ptrVlcTable (gp+0x490) */
int   D_8008D400 = 0;       /* sdata->bool_IsLoaded_VlcTable (gp+0x494) */

extern void *func_8003E874(int size, char *name);   /* MEMPACK_AllocMem */
extern void  func_80032D30(void *bigfile, int type, int index, void *dest, void *cb);  /* LOAD_AppendQueue */
extern void  func_8003C508(void);   /* MainLoadVLC_Callback */

void MainLoadVLC(void)
{
    int size = *(int *)((char *)D_8008D09C + 0xF0C);

    D_8008D400 = 0;
    size = (size + 0x7ff) / 0x800 * 0x800;
    D_8008D3FC = func_8003E874(size, D_8001146C);
    func_80032D30(D_8008D09C, LT_SETADDR, BI_VLCTABLE, D_8008D3FC, func_8003C508);
}
