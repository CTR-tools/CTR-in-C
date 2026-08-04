/*
 * UI_DrawPosSuffix @ 0x8005045C (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg-coloring residual). Uses real types (uitypes.h).
 * Draws the ordinal suffix ("st"/"nd"/"rd"/"th") for a racer's position:
 * rank = driver->driverRank, or (Battle mode) gGT->finishedRankOfEachTeam[BattleHUD.teamID];
 * draws lngStrings[stringIndexSuffix[rank]] at (posX,posY); pokes instBigNum->matrix.t[2].
 * Phase B (saphi): pending.
 */
#include "../uitypes.h"

#define BATTLE_MODE 0x20
#define FONT_BIG 1

extern void func_80022878(char *s, int x, int y, int font, short flags);  /* DecalFont_DrawLine */
extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
extern char **D_8008D878;                /* sdata->lngStrings */
extern short D_8008643C[];               /* data.stringIndexSuffix[rank] */

void UI_DrawPosSuffix(short posX, short posY, struct Driver *driver, unsigned short flags)
{
    int rank;

    if ((D_8008D2AC->gameMode1 & BATTLE_MODE) == 0) {
        rank = (int)driver->driverRank;
    } else {
        rank = D_8008D2AC->finishedRankOfEachTeam[driver->BattleHUD.teamID];
    }
    func_80022878(D_8008D878[D_8008643C[rank]], posX, posY, FONT_BIG, flags);
    if (driver->instBigNum != 0) {
        driver->instBigNum->matrix.t[2] = driver->driverRank + 0x100;
    }
}
