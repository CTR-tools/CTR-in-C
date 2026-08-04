/*
 * UI_Weapon_DrawSelf @ 0x800507E0 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg4, reorder2, ins2, del1 — score 440; residual = gcc
 * CSE/reload granularity on d->heldItemID + the s_spacebar address). Real types.
 *
 * Draws the local player's held-weapon icon (+ optional quantity digit) in the HUD.
 *   - heldItemID 0xF  = no weapon -> nothing drawn.
 *   - heldItemID 0x10 = roulette shuffle: unless paused (gameMode1 & 0xF), rolls a random item
 *       (rand()%0xe in Battle, else rand()%0xc) remapping 5->0 / 8->1 / 9->3; lerps the icon in
 *       from PickupTimeboxHUD.start* over its cooldown.
 *   - otherwise: iconID = heldItemID+5; mask item (7) swaps to Uka (0x32) for villain characters;
 *       with >=10 wumpas a TNT/Potion/Shield shows its "armed" icon (heldItemID+0x11); the icon
 *       flickers off (returns) while noItemTimer!=0 on even frames; a held count >1 draws the digit.
 *   Finally DecalHUD_DrawWeapon(ptrIcons[iconID], x, y, &backBuffer->primMem, pushBuffer_UI.ptrOT,
 *   TRANS_50_DECAL, scale, 1).
 *
 * NOTE(claude) vs in-repo decomp (REFERENCE; 926 asm is authoritative):
 *   1) The villain-mask test reads a per-driver character-id table D_80086E84[driverID] (halfword)
 *      and compares against {0,3,6,heldItemID}; the decomp instead uses data.characterIDs + a 0xC9
 *      bitmask (an equivalent reformulation of the same {0,3,6,7} set).
 *   2) s_spacebar[0] = numHeldItems+'0' is written UNCONDITIONALLY (the decomp moved it inside the
 *      `numHeldItems != 0` draw guard).
 *   3) The 926 has no USE_60FPS icon-latch block.
 *   4) numWumpas is a SIGNED byte (lb/slti); this toolchain compiles `char` as unsigned, so driver.h
 *      declares it `signed char`.
 * Phase B (saphi): see note below.
 */
#include "../uitypes.h"

#define PAUSE_ALL   0xF
#define BATTLE_MODE 0x20

extern struct GameTracker *D_8008D2AC;      /* gGT (absolute) */
extern char D_8008D524[];                   /* s_spacebar (single-digit HUD string) */
extern short D_80086E84[];                  /* character-id table indexed by driverID */

extern int  func_80078BE8(void);            /* rand */
extern void func_8004EC18(short *out, short startX, short startY, int posX, int posY,
                          int cooldown, int dur);                     /* UI_Lerp2D_HUD */
extern void func_80022878(char *s, int x, int y, int font, int color);  /* DecalFont_DrawLine */
extern void func_80022EC4(struct Icon *icon, int posX, int posY, struct PrimMem *pm,
                          unsigned long *ot, int trans, int scale, int one); /* DecalHUD_DrawWeapon */

void UI_Weapon_DrawSelf(short posX, short posY, short scale, struct Driver *d)
{
    int iconID;
    short posXY[2];

    if (d->heldItemID == 0x10) {
        int itemID;

        posXY[0] = posX;
        posXY[1] = posY;
        itemID = 0;

        if ((D_8008D2AC->gameMode1 & PAUSE_ALL) == 0) {
            if (D_8008D2AC->gameMode1 & BATTLE_MODE) {
                itemID = func_80078BE8() % 0xe;
                if (itemID == 5) goto set_item0;
                if (itemID == 8) itemID = 1;
                else if (itemID == 9) itemID = 3;
            } else {
                itemID = func_80078BE8() % 0xc;
                if (itemID == 5) {
                set_item0:
                    itemID = 0;
                }
            }
        }

        if (d->PickupTimeboxHUD.cooldown != 0) {
            func_8004EC18(&posXY[0], d->PickupTimeboxHUD.startX, d->PickupTimeboxHUD.startY,
                          posX, posY, d->PickupTimeboxHUD.cooldown, 5);
            d->PickupTimeboxHUD.cooldown--;
        }

        func_80022EC4(D_8008D2AC->ptrIcons[itemID + 5], posXY[0], posXY[1],
                      &D_8008D2AC->backBuffer->primMem, D_8008D2AC->pushBuffer_UI.ptrOT,
                      1, scale, 1);
    } else {
        if (d->heldItemID == 0xf) return;

        iconID = d->heldItemID + 5;
        D_8008D524[0] = d->numHeldItems + 0x30;

        if (d->heldItemID == 7) {
            short charID = D_80086E84[d->driverID];
            if (charID != 0 && charID != 3 && charID != 6 && charID != d->heldItemID) {
                iconID = 0x32;
            }
        }

        if (d->numWumpas >= 10 &&
            ((unsigned int)(d->heldItemID - 3) < 2 || d->heldItemID == 6)) {
            iconID = d->heldItemID + 0x11;
        }

        if (d->noItemTimer != 0 && (D_8008D2AC->timer & 1) == 0) return;

        if (d->numHeldItems != 0) {
            func_80022878(D_8008D524, posX, posY, 2, 4);
        }

        func_80022EC4(D_8008D2AC->ptrIcons[iconID], posX, posY,
                      &D_8008D2AC->backBuffer->primMem, D_8008D2AC->pushBuffer_UI.ptrOT,
                      1, scale, 1);
    }
}
