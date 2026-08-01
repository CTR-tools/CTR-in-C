/*
 * UI_Map_DrawAdvPlayer @ 0x8004DBAC (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 1055, Branch 0 / Stack 0, Register 7 — a genuine
 * gcc-2.8.1 instruction-scheduling wall (retail materialises the icon/template
 * addresses early and fills the load-delay slots with the `lh` coord loads; gcc from
 * natural C emits nops and orders them later). The prior "aibox" seed reached 450 only
 * via a `do { if (1) {} } while (0)` empty-block hack — barred by the no-hacky-match
 * mandate; this delivery is the NATURAL form. From aibox staging.
 *
 * Draws a player's arrow on the minimap: transforms the world position to map space
 * (UI_Map_GetIconPos), picks the arrow colour that blinks on gGT->timer bit 1, and
 * submits the arrow primitive (AH_Map_HubArrow). Verified vs asm + saphi game/UI/UI_Map.c.
 *
 * saphi UI_Map_DrawAdvPlayer audited CLEAN: it passes posX/posY as full int (retail
 * passes (short)), but AH_Map_HubArrow uses them only in s16-truncated stores, so the
 * two are equivalent mod 2^16 — behavior-neutral, not a bug.
 */
typedef unsigned char undefined;
typedef unsigned int uint;

extern char *D_8008D2AC;            /* gGT (GameTracker*) */
extern undefined D_80086424;        /* playerIconAdvMap.vertCol1 (normal colour) */
extern undefined D_80086430;        /* playerIconAdvMap.vertCol2 (blink colour, +0xC) */
extern undefined D_80086418;        /* playerIconAdvMap.unk_playerAdvMap (vert template) */

extern void func_8004D8B4(int ptrMap, int *mapX, int *mapZ);   /* UI_Map_GetIconPos */
extern void func_800B0F18(int posX, int posY, undefined *vertPos, undefined *vertCol,
                          int scale, int angle);                /* AH_Map_HubArrow */

void UI_Map_DrawAdvPlayer(int ptrMap, int *worldPos, int unused1, int unused2,
                          short angle, short scale)
{
  int mapX;
  int mapZ;
  undefined *arrowColor;

  mapX = worldPos[0];
  mapZ = worldPos[2];
  func_8004D8B4(ptrMap, &mapX, &mapZ);   /* UI_Map_GetIconPos */

  arrowColor = &D_80086424;
  if (((*(uint *)(D_8008D2AC + 0x1CEC)) & 2) != 0)
  {
    arrowColor = &D_80086430;
  }
  func_800B0F18((int)(short)mapX, (int)(short)mapZ, &D_80086418, arrowColor,  /* AH_Map_HubArrow */
                (int)scale, (int)angle);
}
