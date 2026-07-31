// MainFreeze_GetMenuPtr @ 0x80039dcc  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 3000, grind floor ~2335 non-natural).
// Residual: gGT-pointer vs gameMode1-value register assignment (retail a0/v1, gcc
// picks the reverse) — whole-function reg-alloc. Behavior identical.
// (Ghidra: MainFreeze_GetMenuBox.) Returns the pause-menu box for the current mode:
// ADVENTURE_ARENA → set rowsAdvHub[1].stringIndex = AKU(0xb) if driver[0] is a good
// guy else UKA(0xc), return menuAdvHub; ADVENTURE_MODE → advCup/advRace by
// ADVENTURE_CUP; BATTLE_MODE → battle; else gameMode2 & CUP_ANY_KIND → arcadeCup / arcadeRace.
// gGT gp-relative (gp+0x340); MaskBoolGoodGuy returns short (sll16 test).
#include "CTR.h"

#define ADVENTURE_ARENA 0x100000
#define ADVENTURE_MODE  0x80000
#define ADVENTURE_CUP   0x10000000
#define BATTLE_MODE     0x20
#define CUP_ANY_KIND    0x10

struct GameTracker {
    int gameMode1;                 /* 0x0  */
    char pad4[0x8 - 0x4];
    int gameMode2;                 /* 0x8  */
    char pad_24EC[0x24EC - 0xC];
    void *drivers[8];              /* 0x24EC */
};

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */

extern short D_80084368[];   /* data.rowsAdvHub — [1].stringIndex @ +6 (short idx 3) */
extern int D_80084388, D_800843D4, D_80084418, D_80084474, D_800844B8, D_80084510;  /* menu boxes */
extern short func_80064BE4(void *driver);   /* VehPickupItem_MaskBoolGoodGuy */

void *MainFreeze_GetMenuPtr(void)
{
    struct GameTracker *gGT = D_8008D2AC;
    int gm = gGT->gameMode1;

    if (gm & ADVENTURE_ARENA)
    {
        short hint = 0xc;
        if (func_80064BE4(gGT->drivers[0]) != 0)
            hint = 0xb;
        D_80084368[3] = hint;
        return &D_80084388;
    }
    if (gm & ADVENTURE_MODE)
    {
        if (gm & ADVENTURE_CUP)
            return &D_80084418;
        return &D_800843D4;
    }
    if (gm & BATTLE_MODE)
        return &D_80084474;
    if (gGT->gameMode2 & CUP_ANY_KIND)
        return &D_800844B8;
    return &D_80084510;
}
