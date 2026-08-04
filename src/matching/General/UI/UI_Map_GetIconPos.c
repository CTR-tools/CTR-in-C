/*
 * UI_Map_GetIconPos @ 0x8004D8B4 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg-coloring residual on a division-heavy leaf). Real types.
 * Projects a world position (*posX,*posY) into minimap-icon space per the level's ST1_MAP block
 * (worldRange = worldEnd-worldStart), rotated by map->mode (0/1/2/3 = 0/90/180/270 deg). Writes back
 * *posX = iconStartX +/- (posX or posY)*iconSize/worldRange, *posY = iconStartY-0x10 +/- ... ; in 3P,
 * posX -= 60, posY += 10. Levers: negate the QUOTIENT and compute the final pos IN each mode branch
 * (folds the sign into a subu, no negu); hoist iconStartY-0x10 (shared-tail temp).
 * Phase B (saphi UI_Map.c:141): CLEAN — identical mode formulas / numPlyr / writes; saphi's negate-
 * dividend `-(A)/B` is value-identical to the asm's negate-quotient (C truncation: (-A)/B == -(A/B)).
 */
#include "../uitypes.h"

/* Minimap projection params (the level's ST1_MAP block). */
struct Map
{
    short worldEndX;    /* 0x0 */
    short worldEndY;    /* 0x2 */
    short worldStartX;  /* 0x4 */
    short worldStartY;  /* 0x6 */
    short iconSizeX;    /* 0x8 */
    short iconSizeY;    /* 0xA */
    short iconStartX;   /* 0xC */
    short iconStartY;   /* 0xE */
    short mode;         /* 0x10 */
};

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */

void UI_Map_GetIconPos(short *m, int *posX, int *posY)
{
    struct Map *map = (struct Map *)m;
    short mode;
    int px;
    int py;
    int worldRangeX;
    int worldRangeY;
    int iconStartYm16;

    mode = map->mode;
    worldRangeX = map->worldEndX - map->worldStartX;
    worldRangeY = map->worldEndY - map->worldStartY;
    iconStartYm16 = map->iconStartY - 0x10;

    if (mode == 0) {
        px = map->iconStartX + (*posX * map->iconSizeX) / worldRangeX;
        py = iconStartYm16 + (*posY * map->iconSizeY * 2) / worldRangeY;
    } else if (mode == 1) {
        px = map->iconStartX - (*posY * map->iconSizeX) / worldRangeY;
        py = iconStartYm16 + (*posX * map->iconSizeY * 2) / worldRangeX;
    } else if (mode == 2) {
        px = map->iconStartX - (*posX * map->iconSizeX) / worldRangeX;
        py = iconStartYm16 - (*posY * map->iconSizeY * 2) / worldRangeY;
    } else {
        px = map->iconStartX + (*posY * map->iconSizeX) / worldRangeY;
        py = iconStartYm16 - (*posX * map->iconSizeY * 2) / worldRangeX;
    }

    if (D_8008D2AC->numPlyrCurrGame == 3) {
        px -= 60;
        py += 10;
    }

    *posX = px;
    *posY = py;
}
