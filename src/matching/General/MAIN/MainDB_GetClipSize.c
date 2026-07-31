// MainDB_GetClipSize @ 0x80034960  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 20, grind floor 4 reg diffs).
// switch(levelID) OT/clip-entry count: SEWER_SPEEDWAY(8)=6000, MYSTERY_CAVES(9)=2500,
// MAIN_MENU_LEVEL(0x27)=16, ADVENTURE_GARAGE(0x28)=24000, PAPU_PYRAMID(5)/POLAR_PASS(0xc)
// =2500 in 3P/4P else 3000, default 3000. Case order (PAPU/POLAR last) matches the
// retail jump-table block layout + shared default-3000 tail.
#include "CTR.h"

int MainDB_GetClipSize(u32 levelID, int numPlyrCurrGame)
{
    switch (levelID)
    {
    case 8:    /* SEWER_SPEEDWAY */
        return 6000;
    case 9:    /* MYSTERY_CAVES */
        return 0x9c4;
    case 0x27: /* MAIN_MENU_LEVEL */
        return 0x10;
    case 0x28: /* ADVENTURE_GARAGE */
        return 24000;
    case 5:    /* PAPU_PYRAMID */
    case 0xc:  /* POLAR_PASS   */
        if (numPlyrCurrGame > 2)
            return 0x9c4;
        break;
    }
    return 3000;
}
