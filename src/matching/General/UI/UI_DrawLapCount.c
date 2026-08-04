/*
 * UI_DrawLapCount @ 0x80050528 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg-coloring residual). Real types (uitypes.h).
 * currLap = min(driver->lapIndex+1, gGT->numLaps). 1P/2P: "LAP" label + sprintf("%d/%d") FONT_BIG.
 * 3P/4P: pokes digits into g_szLapDividing template, FONT_SMALL. NOTE: 926 splits sprintf-vs-manual
 * by player count (not the decomp's #ifdef USE_ONLINE) — followed Ghidra/asm.
 * Phase B (saphi): pending.
 */
#include "../uitypes.h"

#define FONT_BIG   1
#define FONT_SMALL 2

extern void func_80022878(char *s, int x, int y, int font, short flags);  /* DecalFont_DrawLine */
extern int  func_80078348(char *buf, char *fmt, ...);                     /* sprintf */
extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
extern char **D_8008D878;                /* sdata->lngStrings */
extern char D_8008D514[];                /* g_szFormatNumSlashNum "%d/%d" */
extern char D_8008D51C[];                /* g_szLapDividing "N/M" template */

void UI_DrawLapCount(short posX, short posY, int unused, struct Driver *driver)
{
    char *str;
    int fontType;
    int currLap;
    unsigned short flags;
    char buf[24];

    currLap = driver->lapIndex + 1;
    if ((int)D_8008D2AC->numLaps < (int)(driver->lapIndex + 1)) {
        currLap = (int)D_8008D2AC->numLaps;
    }
    if ((unsigned char)D_8008D2AC->numPlyrCurrGame < 3) {
        func_80022878(D_8008D878[0x18], posX, posY, FONT_SMALL, 0x4001);
        func_80078348(buf, D_8008D514, (int)(short)currLap, (int)D_8008D2AC->numLaps);
        str = buf;
        fontType = FONT_BIG;
        flags = 0x4001;
    } else {
        str = D_8008D51C;
        fontType = FONT_SMALL;
        D_8008D51C[0] = (char)currLap + '0';
        flags = 1;
        D_8008D51C[2] = D_8008D2AC->numLaps + '0';
    }
    func_80022878(str, posX, posY + 8, fontType, flags);
}
