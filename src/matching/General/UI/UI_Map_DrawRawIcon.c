/*
 * UI_Map_DrawRawIcon @ 0x8004DC44 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 3725, Branch 0 / Stack 0, Register 29.
 * NATURAL form (from aibox staging) — the aibox seed hit a lower 2445 only via an
 * `if (1) { ... }` empty-conditional hack + a `new_var` reused across two unrelated
 * argument values; both are barred match-forcing tricks and were removed. The honest
 * behavior-exact form (matching ghidra_source.c + saphi) is 3725.
 *
 * Residual is a genuine structural wall, NOT a behavior/register difference:
 * retail copies the 4 colour words from `*colorPtr` with UNALIGNED `lwl/lwr` + `swl/swr`
 * (a 16-byte struct-by-value copy from an unaligned source), reloading `*colorPtr`
 * before each. The 4-separate-`int`-arg form gcc emits aligned `lw`, so the byte-exact
 * match would require re-typing func_80023054's colour args as one unaligned 16-byte
 * struct-by-value parameter — left as a future refinement; behavior is identical either way.
 *
 * Draws a raw minimap icon: transforms worldPos -> screen via UI_Map_GetIconPos, then
 * DecalHUD_DrawPolyGT4 with the icon sprite (iconGroup entry, iconID-indexed) and the
 * 4-corner colour block ptrColor[colorID].
 *
 * saphi UI_Map_DrawRawIcon (game/UI/UI_Map.c:228) audited CLEAN vs this asm + Ghidra.
 */
typedef unsigned int undefined4;
extern char *D_8008D2AC;                 /* gGT */
extern int D_80081D70;                   /* data.ptrColor[] table base */
extern void func_8004D8B4(int ptrMap, int *posX, int *posY);   /* UI_Map_GetIconPos */
extern void func_80023054(int tex, int x, int y, int prim, int ot,
                          int c0, int c1, int c2, int c3, int p10, int scale); /* DecalHUD_DrawPolyGT4 */

void UI_Map_DrawRawIcon(int ptrMap, int *worldPos, int iconID, int colorID, int unused, short scale)
{
  int posX;
  int posY;
  int *colorPtr;

  posX = worldPos[0];
  posY = worldPos[2];
  func_8004D8B4(ptrMap, &posX, &posY);

  colorPtr = (int *)((int)(&D_80081D70) + ((colorID << 0x10) >> 0xE));

  func_80023054(
      *((int *)((*((int *)(D_8008D2AC + 0x2128)) + ((iconID << 0x10) >> 0xE)) + 0x14)),
      posX, posY,
      (*((int *)(D_8008D2AC + 0x10))) + 0x74,
      *((int *)(D_8008D2AC + 0x147C)),
      *((int *)*colorPtr),
      *((int *)(*colorPtr + 4)),
      *((int *)(*colorPtr + 8)),
      *((int *)(*colorPtr + 0xC)),
      0, (int)scale);
}
