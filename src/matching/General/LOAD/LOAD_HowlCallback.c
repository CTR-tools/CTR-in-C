// LOAD_HowlCallback @ 0x8003254c  (vs SCUS_944.26.exe)
// SCORE 0. HOWL sector-chain CD read-done ISR cb: CdReadCallback(0); set
// sdata->howlChainState (gp+0x8D4) = 0 if (result & 0xff)==CdlComplete else -1.
// Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"

extern void func_800771B0(void *);   /* CdReadCallback */

int D_8008D840 = 0;   /* sdata->howlChainState (gp+0x8D4) */

void LOAD_HowlCallback(u8 result, u8 *unk)
{
    func_800771B0(0);

    if ((result & 0xff) == 2)
        D_8008D840 = 0;
    else
        D_8008D840 = -1;
}
