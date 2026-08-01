/*
 * UI_DrawBattleScores @ 0x80050654 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 3125, Branch Differences 0, Stack Differences 0,
 * Register Differences 45 — a genuine gcc-2.8.1 register-allocation wall (a 200-iter
 * -j4 grind never got below ~3050; random permutation can't recolor the s0/s1/s2-heavy
 * two-branch body). From aibox staging; base was already behaviorally correct (no
 * false-friend, no hacks). Names kept as raw offsets (pure-rename delivery) with the
 * saphi field meanings in comments so the match stays byte-for-byte.
 *
 * Draws one battle-mode score/lives readout for a driver:
 *   - POINT_LIMIT (gameMode1 & 0x4000): value = gGT->battleSetup.pointsPerTeam[driver->BattleHUD.teamID]
 *                                       (gGT[+0x1D90 + teamID*4]); icon = ptrIcons[0x85] (gGT+0x2100)
 *   - LIFE_LIMIT  (gameMode1 & 0x8000): value = driver->BattleHUD.numLives (driver+0x4E4);
 *                                       icon = ptrIcons[0x84] (gGT+0x20FC)
 *   - neither flag set: draw nothing.
 * Then sprintf(value) and DrawLine at (posX+37, posY+4) FONT_SMALL, color
 * data.battleScoreColor[numPlyrCurrGame-1][driver->driverID] (D_8008644C, stride 8 per
 * player-count row, 2 per driver), and DrawPolyFT4(icon, posX, posY, &backBuffer->primMem,
 * pushBuffer_UI.ptrOT, 1, 0x1000).
 *
 * saphi UI_DrawBattleScores (game/UI/UI_RaceHud.c:540) audited CLEAN vs this asm + Ghidra.
 */
typedef unsigned int uint;
typedef unsigned char byte;
typedef unsigned int undefined4;

extern char *D_8008D2AC;                 /* sdata->gGT */
extern char D_8008644C;                  /* data.battleScoreColor[][] (s16) */
extern char D_8008D520;                  /* "%d" format (s_longInt) */
extern int  func_80078348(char *buf, const char *fmt, ...);                        /* sprintf */
extern void func_80022878(char *str, int x, int y, int font, int color);           /* DecalFont_DrawLine */
extern void func_80022DB0(undefined4 icon, int x, int y, int prim, undefined4 ot, int p6, int p7); /* DecalHUD_DrawPolyFT4 */

void UI_DrawBattleScores(int posX, int posY, int driver)
{
    short scoreColor;
    undefined4 ptrOT;
    undefined4 iconTex;
    int backBuffer;
    char string[32];

    /* data.battleScoreColor[numPlyrCurrGame-1][driver->driverID] */
    scoreColor = *((short *)(((&D_8008644C) + (((uint)(*((byte *)(driver + 0x4A)))) * 2))
                             + ((((uint)((byte)D_8008D2AC[0x1CA8])) - 1) * 8)));

    if (((*((uint *)D_8008D2AC)) & 0x4000) != 0)   /* POINT_LIMIT */
    {
        func_80078348(string, &D_8008D520,
                      *((undefined4 *)((D_8008D2AC + ((*((int *)(driver + 0x4E8))) * 4)) + 0x1D90)));
        func_80022878(string, ((posX + 0x25) * 0x10000) >> 0x10, ((posY + 4) * 0x10000) >> 0x10, 2, (int)scoreColor);
        backBuffer = *((int *)(D_8008D2AC + 0x10));
        ptrOT = *((undefined4 *)(D_8008D2AC + 0x147C));
        iconTex = *((undefined4 *)(D_8008D2AC + 0x2100));
    }
    else
    {
        if (((*((uint *)D_8008D2AC)) & 0x8000) == 0)   /* LIFE_LIMIT */
        {
            return;
        }
        func_80078348(string, &D_8008D520, *((undefined4 *)(driver + 0x4E4)));
        func_80022878(string, ((posX + 0x25) * 0x10000) >> 0x10, ((posY + 4) * 0x10000) >> 0x10, 2, (int)scoreColor);
        backBuffer = *((int *)(D_8008D2AC + 0x10));
        ptrOT = *((undefined4 *)(D_8008D2AC + 0x147C));
        iconTex = *((undefined4 *)(D_8008D2AC + 0x20FC));
    }
    func_80022DB0(iconTex, (int)((short)posX), (int)((short)posY), backBuffer + 0x74, ptrOT, 1, 0x1000);
}
