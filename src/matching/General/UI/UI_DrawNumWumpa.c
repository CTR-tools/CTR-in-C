/*
 * UI_DrawNumWumpa @ 0x80050C20 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 10220, Branch Differences 0, Stack Differences 0,
 * Register Differences 60. Finished from the "aibox" prior WIP set (base was already
 * behaviorally correct — NOT a false-friend, no hacks).
 *
 * The large residual is a STRUCTURAL wall, not behavior/registers: for the >=3-player
 * two-digit path retail copies each 4-word colour block from ptrColorOrange with an
 * UNALIGNED lwl/lwr + swl/swr 16-byte struct-by-value copy (source pointer treated as
 * unaligned), while the natural 4-separate-int-arg form gcc emits aligned lw. Byte-exact
 * would require re-typing DecalHUD_DrawPolyGT4's colour parameter as one unaligned
 * 16-byte struct-by-value arg (same wall as UI_Map_DrawRawIcon @0x8004DC44); behaviour is
 * identical either way. A 150-iter grind found nothing lower (only an unverified 9220
 * false-candidate on the same wall, rejected). No hacks.
 *
 * Draws the on-screen wumpa counter for driver `d`:
 *   - <3 players: a small 'x' icon glyph then the number (FONT_BIG) at posX+0xD, ORANGE.
 *   - 3..4 players: two individual digit sprites (tens at posX, ones at posX+0xC) via
 *     DecalHUD_DrawPolyGT4 using the ORANGE colour block.
 *
 * saphi UI_DrawNumWumpa (game/UI/UI_DrawNum.c:4) audited CLEAN vs this asm + Ghidra.
 */
typedef unsigned int uint;
typedef unsigned int undefined4;
typedef unsigned char byte;

extern char *D_8008D2AC;   /* sdata->gGT                         */
extern char *D_80081D70;   /* data.ptrColor[ORANGE] (4 colour words) */
extern char D_8008D528;    /* sdata->s_x    ("x" glyph string)   */
extern char D_8008D510;    /* sdata->s_int  ("%d" format string) */

extern void func_80022878(char *str, int x, int y, int font, int p5);          /* DecalFont_DrawLine */
extern int  func_80078348(char *buf, const char *fmt, ...);                    /* sprintf */
extern void func_80023054(undefined4 tex, int x, int y, int prim, undefined4 ot,
                          undefined4 c0, undefined4 c1, undefined4 c2, undefined4 c3,
                          int p10, int scale);                                 /* DecalHUD_DrawPolyGT4 */

void UI_DrawNumWumpa(int posX, int posY, int driver)
{
    char numWumpas;
    int currWumpa10s;
    short posXShort;
    short posYShort;
    char message[16];

    posXShort = (short)posX;
    posYShort = (short)posY;

    if ((byte)D_8008D2AC[0x1CA8] < 3)   /* gGT->numPlyrCurrGame < 3 */
    {
        func_80022878(&D_8008D528, (int)posXShort, (posY + 4) * 0x10000 >> 0x10, 2, 0);
        func_80078348(message, &D_8008D510, (int)(*(char *)(driver + 0x30)));
        func_80022878(message, (posX + 0xD) * 0x10000 >> 0x10, (int)posYShort, 1, 0);
    }
    else
    {
        numWumpas = *(char *)(driver + 0x30);
        currWumpa10s = ((int)numWumpas / 10) * 0x1000000 >> 0x18;             /* tens digit */
        func_80023054(*(undefined4 *)(*(int *)(D_8008D2AC + 0x2128) + currWumpa10s * 4 + 0x14),
            (int)posXShort, (int)posYShort,
            *(int *)(D_8008D2AC + 0x10) + 0x74,                              /* &gGT->backBuffer->primMem */
            *(undefined4 *)(D_8008D2AC + 0x147C),                            /* gGT->pushBuffer_UI.ptrOT  */
            *(undefined4 *)D_80081D70,
            *(undefined4 *)(D_80081D70 + 4),
            *(undefined4 *)(D_80081D70 + 8),
            *(undefined4 *)(D_80081D70 + 0xC),
            0, 0x1000);
        func_80023054(*(undefined4 *)(*(int *)(D_8008D2AC + 0x2128) + ((int)numWumpas + currWumpa10s * -10) * 4 + 0x14),
            (int)posXShort + 0xC, (int)posYShort,                           /* ones digit at posX+0xC */
            *(int *)(D_8008D2AC + 0x10) + 0x74,
            *(undefined4 *)(D_8008D2AC + 0x147C),
            *(undefined4 *)D_80081D70,
            *(undefined4 *)(D_80081D70 + 4),
            *(undefined4 *)(D_80081D70 + 8),
            *(undefined4 *)(D_80081D70 + 0xC),
            0, 0x1000);
    }
}
