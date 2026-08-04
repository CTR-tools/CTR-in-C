/*
 * UI_DrawLimitClock @ 0x8004F894 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 / Register 0 (residual = 4-insn flags-block scheduling, score 860).
 * Real types (gametracker.h). Countdown time-limit clock (Battle / Crystal Challenge):
 * timeRemaining = gGT->originalEventTime - gGT->elapsedEventTime; RECTMENU_DrawTime(max(tr,0)); on
 * expiry (unless END_OF_RACE) sets actionsFlagSet bit 0x2000000 on every driver + MainGameEnd_Initialize.
 * Color DARK_RED(0x1c), flashing WHITE(4)/DARK_RED under 15s (tr<0x3840) by gGT->timer&1.
 * Phase B (saphi): see note below.
 */
#include "../uitypes.h"

#define END_OF_RACE 0x200000

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
extern char *func_80044FF8(int ms);      /* RECTMENU_DrawTime */
extern void func_8003A3FC(void);         /* MainGameEnd_Initialize */
extern void func_80022878(char *s, int x, int y, int font, short color);  /* DecalFont_DrawLine */

void UI_DrawLimitClock(short posX, short posY, char fontType)
{
    char *str;
    int timeRemaining;
    unsigned short flags;
    int i;

    timeRemaining = D_8008D2AC->originalEventTime - D_8008D2AC->elapsedEventTime;
    if (timeRemaining < 0) {
        str = func_80044FF8(0);
        if ((D_8008D2AC->gameMode1 & END_OF_RACE) == 0) {
            for (i = 0; i < D_8008D2AC->numPlyrCurrGame; i++) {
                D_8008D2AC->drivers[i]->actionsFlagSet |= 0x2000000;
            }
            func_8003A3FC();
        }
    } else {
        str = func_80044FF8(timeRemaining);
    }
    flags = 0x1c;
    if ((timeRemaining < 0x3840) && (flags = 4, (D_8008D2AC->timer & 1) != 0)) {
        flags = 0x1c;
    }
    func_80022878(str, posX, posY, fontType, flags);
}
