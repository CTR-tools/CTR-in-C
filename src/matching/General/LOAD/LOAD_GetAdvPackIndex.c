// LOAD_GetAdvPackIndex @ 0x800335dc  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Returns the adventure mempack index for the current hub level: 2 for the two
// hub levels (GEM_STONE_VALLEY 0x19 / GLACIER_PARK 0x1C), else 1. Callers use
// 3 - this to load the OTHER pack. Match lever: `if (== 0x19 || == 0x1C) return 2`
// (return-2 first) matches the retail beq->ret2 / bne->ret1 layout.
#include "CTR.h"

struct GameTracker { char pad0[0x1A10]; int levelID; };   /* levelID @0x1A10 */

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */

int LOAD_GetAdvPackIndex(void)
{
    int levelID = D_8008D2AC->levelID;

    if (levelID == 0x19 || levelID == 0x1C)
        return 2;

    return 1;
}
