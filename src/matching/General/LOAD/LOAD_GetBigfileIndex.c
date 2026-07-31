// LOAD_GetBigfileIndex @ 0x80032c24  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 2150, grind plateau ~1235 non-natural).
// Residual: gcc-2.8.1 vs retail branch-delay-slot filling + tail cross-jumping —
// retail hoists each next-block `levelID - X` subtraction into the current
// branch delay slot and folds the BI_* constant into each `jr` delay slot, while
// gcc-2.8.1 cross-jumps the shared `+levBigLodIndex+fileIndex` return tails.
// Behavior identical (leaf return-dispatcher; no calls/stack).
// Maps (levelID, lod, fileIndexInGroup) to a bigfile index across track/battle/
// cutscene/credits/menu/hub ranges. Note: levBigLodIndex[lod-1] is folded to
// D_8008D0B3[lod] (base = array - 1), matching the retail 0(D_8008D0B3) reloc.
#include "CTR.h"

#define NITRO_COURT        0x12
#define INTRO_RACE_TODAY   0x1E
#define OXIDE_ENDING       0x2A
#define ADVENTURE_GARAGE   0x28
#define NAUGHTY_DOG_CRATE  0x29
#define CREDITS_CRASH      0x2C
#define MAIN_MENU_LEVEL    0x27
#define SCRAPBOOK          0x40
#define GEM_STONE_VALLEY   0x19

#define BI_ARCADETRACKS    0
#define BI_BATTLETRACKS    0x90
#define BI_ADVENTUREHUB    0xC8
#define BI_MAINMENUFILE    0xD7
#define BI_NDBOX           0x201
#define BI_CUTSCENES_INTRO 0x203
#define BI_CUTSCENES_OUTRO 0x21E
#define BI_CREDITS         0x222
#define BI_SCRAPBOOK       0x25E

extern u8 D_8008D0B3[];   /* sdata->levBigLodIndex - 1 (indexed by lod) */

int LOAD_GetBigfileIndex(u32 levelID, int lod, int fileIndexInGroup)
{
    if (levelID < NITRO_COURT)
        return BI_ARCADETRACKS + levelID * 8 + D_8008D0B3[lod] + fileIndexInGroup;

    if ((u32)(levelID - NITRO_COURT) < 7)
        return BI_BATTLETRACKS + (levelID - NITRO_COURT) * 8 + D_8008D0B3[lod] + fileIndexInGroup;

    if ((u32)(levelID - INTRO_RACE_TODAY) < 9)
        return BI_CUTSCENES_INTRO + (levelID - INTRO_RACE_TODAY) * 3 + fileIndexInGroup;

    if ((u32)(levelID - OXIDE_ENDING) < 2)
        return BI_CUTSCENES_OUTRO + (levelID - OXIDE_ENDING) * 2 + fileIndexInGroup;

    if (levelID == ADVENTURE_GARAGE)
        return BI_MAINMENUFILE + 2 + fileIndexInGroup;

    if (levelID == NAUGHTY_DOG_CRATE)
        return BI_NDBOX + fileIndexInGroup;

    if ((u32)(levelID - CREDITS_CRASH) < 20)
        return BI_CREDITS + (levelID - CREDITS_CRASH) * 3 + fileIndexInGroup;

    if (levelID == MAIN_MENU_LEVEL)
        return BI_MAINMENUFILE + fileIndexInGroup;

    if (levelID == SCRAPBOOK)
        return BI_SCRAPBOOK + fileIndexInGroup;

    return BI_ADVENTUREHUB + (levelID - GEM_STONE_VALLEY) * 3 + fileIndexInGroup;
}
