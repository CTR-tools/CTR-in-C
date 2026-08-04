/*
 * UI_RenderFrame_AdvHub @ 0x80054298 (SCUS_944.26 / NTSC-U 926)
 * Byte-exact match (score 0). Real types (uitypes.h).
 * Adventure-hub HUD: draws relic/key/trophy counters at g_apHudLayout[numPlyrCurrGame-1]
 * entries 0xE/0xF/0x10 (+0x10 x, -10 y), passing gGT->drivers[0] as the 3rd arg.
 * Phase B (saphi UI_RenderFrame.c:879): CLEAN — saphi's 2-arg DrawNum* calls omit the vestigial
 * drivers[0] (a2) retail passes; the 2-arg DrawNum* callees ignore it, so behavior is identical.
 */
#include "../uitypes.h"

extern struct GameTracker *D_8008D2AC;    /* sdata->gGT */
extern struct UiElement2D *D_8008625C[];  /* g_apHudLayout[numPlyrCurrGame-1] */
extern void func_80050F18(short x, short y, struct Driver *d);  /* UI_DrawNumRelic */
extern void func_80050FC4(short x, short y, struct Driver *d);  /* UI_DrawNumKey */
extern void func_80051070(short x, short y, struct Driver *d);  /* UI_DrawNumTrophy */

void UI_RenderFrame_AdvHub(void)
{
    struct GameTracker *gGT = D_8008D2AC;
    struct UiElement2D *hud = D_8008625C[gGT->numPlyrCurrGame - 1];
    struct Driver *d = gGT->drivers[0];

    func_80050F18(hud[0xE].x + 0x10, hud[0xE].y - 10, d);
    func_80050FC4(hud[0xF].x + 0x10, hud[0xF].y - 10, d);
    func_80051070(hud[0x10].x + 0x10, hud[0x10].y - 10, d);
}
