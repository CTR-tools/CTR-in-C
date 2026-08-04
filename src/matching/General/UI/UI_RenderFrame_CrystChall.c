/*
 * UI_RenderFrame_CrystChall @ 0x8005435C (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg13, score 465 — sole residual is a v0/v1 + nop scheduling
 * nuance in the matrix-convert >>8). Real types.
 *
 * Crystal Challenge HUD frame renderer (single player, drivers[0]). Layout from
 * g_apHudLayout[numPlyrCurrGame-1] (a UiElement2D[] per player count). Unless paused, runs
 * UI_JumpMeter_Update, then the instrument cluster (DrawSpeedNeedle / JumpMeter_Draw / DrawSlideMeter
 * / DrawSpeedBG), DrawNumCrystal, Weapon_DrawSelf, the "TIME" label (lngStrings[0x12]) and the
 * DrawLimitClock countdown. Then the crystal-pickup animation: while PickupWumpaHUD.numCollected>0 the
 * floating crystal icon (gp-rel ptrHudCrystal) is shown (clears flags 0x80) and eased in from the
 * pickup point via UI_Lerp2D_HUD, its matrix.t set from the screen pos through the inlined
 * UI_ConvertX/Y_2 (center-relative *z >>8); when the cooldown elapses numCrystals++ and — on reaching
 * numCrystalsInLEV — the event ends (funcPtrs[0]=VehPhysProc_FreezeEndEvent_Init, actionsFlagSet
 * finished, MainGameEnd_Initialize) with the collect SFX (OtherFX 0x42). Else the icon is hidden.
 * Finally stops the weapon-roulette loop SFX (OtherFX 0x5d) once the item stops rolling (gameMode1
 * ROLLING_ITEM 0x800000 cleared).
 *
 * NOTE(claude) vs in-repo decomp (REFERENCE; 926 asm authoritative):
 *   1) HUD layout is g_apHudLayout[numPlyrCurrGame-1] (D_8008625C), NOT data.hudStructPtr[0].
 *   2) The "TIME" DecalFont_DrawLine color arg is 0 (decomp uses ORANGE).
 *   3) ptrHudCrystal is a gp-relative global (gp+0xA50 = D_8008D9BC), not gGT-relative.
 *   4) numCrystals is a SIGNED byte for the win compare (sll24/sra24). USE_ONLINE/REBUILD absent.
 * Phase B (saphi): see note below.
 */
#include "../uitypes.h"

#define PAUSE_ALL     0xF
#define ROLLING_ITEM  0x800000
#define HIDE_MODEL    0x80

extern struct GameTracker *D_8008D2AC;      /* gGT (absolute) */
extern struct UiElement2D *D_8008625C[];    /* g_apHudLayout[numPlyrCurrGame-1] */
extern char **D_8008D878;                   /* sdata->lngStrings */
struct Instance *D_8008D9BC = 0;            /* sdata->ptrHudCrystal (gp-relative) */

extern void func_80062D04(void);            /* VehPhysProc_FreezeEndEvent_Init */
extern void func_80051C64(struct Driver *d);                          /* UI_JumpMeter_Update */
extern void func_800511C0(short x, short y, struct Driver *d);        /* UI_DrawSpeedNeedle */
extern void func_80051E24(short x, short y, struct Driver *d);        /* UI_JumpMeter_Draw */
extern void func_80052250(short x, short y, struct Driver *d);        /* UI_DrawSlideMeter */
extern void func_800516AC(int x, int y, struct Driver *d);           /* UI_DrawSpeedBG (vestigial args) */
extern void func_8005111C(short x, short y, struct Driver *d);        /* UI_DrawNumCrystal */
extern void func_800507E0(short x, short y, short scale, struct Driver *d); /* UI_Weapon_DrawSelf */
extern void func_80022878(char *s, int x, int y, int font, int color);/* DecalFont_DrawLine */
extern void func_8004F894(int x, int y, int fontType);               /* UI_DrawLimitClock */
extern void func_8004EC18(short *out, short sx, short sy, int px, int py, int t, int dur); /* UI_Lerp2D_HUD */
extern void func_80028468(int id, int arg);                          /* OtherFX_Play */
extern void func_80028844(int id);                                   /* OtherFX_Stop2 */
extern void func_8003A3FC(void);                                     /* MainGameEnd_Initialize */

void UI_RenderFrame_CrystChall(void)
{
    struct Driver *driver;
    struct UiElement2D *hud;
    struct Instance *pCrystalInst;
    signed char cNewCrystalCount;
    int iScratch;
    short crystalXY[2];

    driver = D_8008D2AC->drivers[0];
    hud = D_8008625C[D_8008D2AC->numPlyrCurrGame - 1];

    if ((D_8008D2AC->gameMode1 & PAUSE_ALL) == 0) {
        func_80051C64(driver);
    }
    func_800511C0(hud[9].x, hud[9].y, driver);
    func_80051E24(hud[6].x, hud[6].y, driver);
    func_80052250(hud[8].x, hud[8].y, driver);
    func_800516AC(hud[9].x, hud[9].y, driver);
    func_8005111C(hud[0x11].x + 0x10, hud[0x11].y + -0x10, driver);
    func_800507E0(hud[0].x, hud[0].y, hud[0].scale, driver);
    func_80022878(D_8008D878[0x12], 0x14, 8, 2, 0);
    func_8004F894(0x14, 0x10, 1);

    if ((D_8008D2AC->gameMode1 & PAUSE_ALL) != 0) {
        return;
    }
    if (driver->PickupWumpaHUD.numCollected != 0) {
        crystalXY[0] = hud[0x11].x;
        crystalXY[1] = hud[0x11].y;
        D_8008D9BC->flags &= ~HIDE_MODEL;
        if (driver->PickupWumpaHUD.cooldown != 0) {
            func_8004EC18(&crystalXY[0], driver->PickupWumpaHUD.startX, driver->PickupWumpaHUD.startY,
                          hud[0x11].x, hud[0x11].y, driver->PickupWumpaHUD.cooldown, 5);
            iScratch = driver->PickupWumpaHUD.cooldown + -1;
            goto set_cooldown;
        }
        cNewCrystalCount = driver->numCrystals + 1;
        driver->PickupWumpaHUD.numCollected = driver->PickupWumpaHUD.numCollected - 1;
        driver->numCrystals = cNewCrystalCount;
        if (D_8008D2AC->numCrystalsInLEV <= (int)cNewCrystalCount) {
            driver->funcPtrs[0] = (void *)func_80062D04;
            driver->actionsFlagSet |= 0x2000000;
            func_8003A3FC();
        }
        func_80028468(0x42, 1);
        iScratch = 5;
        if (driver->PickupWumpaHUD.numCollected == 0) goto after_cooldown;
    set_cooldown:
        driver->PickupWumpaHUD.cooldown = iScratch;
    after_cooldown:

        pCrystalInst = D_8008D9BC;
        iScratch = (crystalXY[0] + -0x100) * hud[0x11].z;
        if (iScratch < 0) {
            iScratch = iScratch + 0xff;
        }
        pCrystalInst->matrix.t[0] = iScratch >> 8;
        iScratch = (crystalXY[1] + -0x6c) * hud[0x11].z;
        if (iScratch < 0) {
            iScratch = iScratch + 0xff;
        }
        pCrystalInst->matrix.t[1] = iScratch >> 8;
        pCrystalInst->matrix.t[2] = hud[0x11].z;
    } else {
        D_8008D9BC->flags |= HIDE_MODEL;
    }

    if ((D_8008D2AC->gameMode1 & PAUSE_ALL) != 0) return;
    if (driver->itemRollTimer != 0) return;
    if ((D_8008D2AC->gameMode1 & ROLLING_ITEM) == 0) return;
    func_80028844(0x5d);
    D_8008D2AC->gameMode1 &= ~ROLLING_ITEM;
}
