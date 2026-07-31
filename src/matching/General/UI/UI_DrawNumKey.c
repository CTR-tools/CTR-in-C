/*
 * UI_DrawNumKey @ 0x80050FC4 (func_80050FC4, SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact: base score 95, Branch 0 / Stack 0, Register 7 residual.
 * vs SCUS_944.26.exe; finished from the "near completed" prior grind set.
 *
 * Identical shape to UI_DrawNumRelic/UI_DrawNumTrophy (differs only in the counter
 * field offset and the gameMode2 bit). Residual is the same gcc-vs-retail reg-alloc
 * wall (gcc reuses $s0 for text-X vs retail's $a1). Behavior verified vs asm.
 *
 * Draws the key HUD counter: an "x" icon then the key count as a number.
 *   D_8008D528 = sdata->s_x, D_8008D520 = sdata->s_longInt, D_8008D2AC = sdata->gGT
 *   gGT + 0x1E38 = gGT->currAdvProfile.numKeys
 *   gGT + 0x08   = gGT->gameMode2 (bit 0x2000000 => keys-incomplete, show count-1)
 *   func_80022878 = DecalFont_DrawLine (2=FONT_SMALL, 1=FONT_BIG, arg5 0=ORANGE),
 *   func_80078348 = sprintf.
 */

typedef unsigned int uint;

extern char *D_8008D2AC;                                       /* sdata->gGT */
extern char D_8008D528;                                        /* sdata->s_x ("x" glyph) */
extern char D_8008D520;                                        /* sdata->s_longInt ("%ld") */
extern void func_80022878(char *str, int x, int y, int font, int color); /* DecalFont_DrawLine */
extern int func_80078348(char *buf, const char *fmt, ...);               /* sprintf */

void UI_DrawNumKey(int posX, int posY)
{
    int num;
    int textX;
    char string[16];

    func_80022878(&D_8008D528, (int) ((short) posX), ((posY + 4) * 0x10000) >> 0x10, 2, 0);

    num = *((int *) (D_8008D2AC + 0x1E38));
    textX = posX + 0xD;

    if ((*((uint *) (D_8008D2AC + 8)) & 0x2000000) != 0)
        num = num - 1;

    func_80078348(string, &D_8008D520, num);
    func_80022878(string, (textX * 0x10000) >> 0x10, (int) ((short) posY), 1, 0);
}
