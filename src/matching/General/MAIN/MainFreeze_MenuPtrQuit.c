// MainFreeze_MenuPtrQuit @ 0x80039908  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 1660, grind floor ~1635 non-natural).
// Residual: branch/delay-slot scheduling (retail keeps menuBox in v1 across the
// phase split). Pause-menu "Quit" proc: phase!=0 (draw) clears drawStyle bit 0x100,
// re-sets it only for >2 players, SafeAdvDestroy; phase 0 (select): row 0 quits to
// main menu (GhostTape_Destroy, Loading.OnBegin AddBits|=MAIN_MENU(0x2000) /
// RemBits|=ADVENTURE_ARENA(0x100000), mainMenuState=0, gameMode1&=~1,
// MainRaceTrack_RequestLoad(0x27)); row +/-1 sets ptrDesiredMenu=GetMenuPtr.
// gGT gp-relative (gp+0x340, per-block reload); Loading.OnBegin gp-rel (gp+0x194/0x198).
#include "CTR.h"

struct RectMenu { char pad0[0x14]; u16 drawStyle; char pad16[0x1A - 0x16]; short rowSelected; char pad1c[0x1E - 0x1C]; short nProcCallPhase; };
struct GameTracker { char pad0[0x1CA8]; unsigned char numPlyrCurrGame; char pad1[4]; int gameMode1; };

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */
int D_8008D100 = 0;   /* sdata->Loading.OnBegin.AddBitsConfig0 (gp+0x194) */
int D_8008D104 = 0;   /* sdata->Loading.OnBegin.RemBitsConfig0 (gp+0x198) */
extern short D_8008D97C;   /* sdata->mainMenuState (absolute) */
extern void *D_8008D924;   /* sdata->ptrDesiredMenu (absolute) */

extern void func_80028410(void);   /* GhostTape_Destroy */
extern void func_8003CFC0(int);    /* MainRaceTrack_RequestLoad */
extern void *func_80039DCC(void);  /* MainFreeze_GetMenuPtr */
extern void func_800399FC(void);   /* MainFreeze_SafeAdvDestroy */

void MainFreeze_MenuPtrQuit(struct RectMenu *menu)
{
    if (menu->nProcCallPhase != 0)
    {
        u16 style = menu->drawStyle & 0xFEFF;
        menu->drawStyle = style;
        if (D_8008D2AC->numPlyrCurrGame > 2)
            menu->drawStyle = style | 0x100;
        func_800399FC();
    }
    else
    {
        short row = menu->rowSelected;
        if (row == 0)
        {
            func_80028410();
            D_8008D100 |= 0x2000;
            D_8008D97C = 0;
            D_8008D104 |= 0x100000;
            D_8008D2AC->gameMode1 &= ~1;
            func_8003CFC0(0x27);
            return;
        }
        if (row == 1 || row == -1)
            D_8008D924 = func_80039DCC();
    }
}
