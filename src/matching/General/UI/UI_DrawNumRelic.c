/*
 * UI_DrawNumRelic @ 0x80050F18 (func_80050F18, SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact: base score 95, Branch 0 / Stack 0, Register 7 residual.
 * vs SCUS_944.26.exe; finished from the "near completed" prior grind set.
 *
 * The residual is a gcc-2.8.1-vs-retail register-allocation wall: gcc reuses $s0
 * (posX) in place for the text-X (addiu s0,s0,0xD) while retail keeps posX and
 * targets $a1 (addiu a1,s0,0xD). Control flow, stack, and behavior are identical.
 *
 * NOTE: the prior grind snapshot for this function was BEHAVIORALLY WRONG — the
 * permuter had fused the two constants 0xD (text-X offset) and 8 (gameMode2 field
 * offset) into one variable = 8, so it drew the number at posX+8 instead of the
 * retail posX+0xD. Verified against the asm (addiu $a1,$s0,0xD) and corrected here;
 * fusing them did not even improve the score (still 95).
 *
 * Draws the relic HUD counter: an "x" icon then the relic count as a number.
 * Field names from src/decompile/General/UI/UI_34_DrawNumRelic.c:
 *   D_8008D528 = sdata->s_x       ("x" glyph string)
 *   D_8008D520 = sdata->s_longInt ("%ld" format)
 *   D_8008D2AC = sdata->gGT
 *   gGT + 0x1E34 = gGT->currAdvProfile.numRelics
 *   gGT + 0x08   = gGT->gameMode2 (bit 0x1000000 => relic-incomplete, show count-1)
 *   func_80022878 = DecalFont_DrawLine (mode 2 = FONT_SMALL icon, 1 = FONT_BIG number;
 *                   arg5 0 = ORANGE), func_80078348 = sprintf.
 */

typedef unsigned int uint;

extern char *D_8008D2AC;                                       /* sdata->gGT */
extern char D_8008D528;                                        /* sdata->s_x ("x" glyph) */
extern char D_8008D520;                                        /* sdata->s_longInt ("%ld") */
extern void func_80022878(char *str, int x, int y, int font, int color); /* DecalFont_DrawLine */
extern int func_80078348(char *buf, const char *fmt, ...);               /* sprintf */

void UI_DrawNumRelic(int posX, int posY)
{
    int num;
    int textX;
    char string[16];

    func_80022878(&D_8008D528, (int) ((short) posX), ((posY + 4) * 0x10000) >> 0x10, 2, 0);

    num = *((int *) (D_8008D2AC + 0x1E34));
    textX = posX + 0xD;

    if ((*((uint *) (D_8008D2AC + 8)) & 0x1000000) != 0)
        num = num - 1;

    func_80078348(string, &D_8008D520, num);
    func_80022878(string, (textX * 0x10000) >> 0x10, (int) ((short) posY), 1, 0);
}
