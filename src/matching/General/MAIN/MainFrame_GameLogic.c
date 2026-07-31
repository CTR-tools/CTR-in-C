// MainFrame_GameLogic @ 0x80034d54  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 14600, Register 136; target 588 insns vs 581).
// Residual: whole-function gp-reload register allocation on a 588-insn dispatcher
// (retail reloads gGT via gp per block; gcc-2.8.1 caches the param differently).
// Behavior identical. Main per-frame game-logic tick: screen-blur clock loop,
// timer/frozen-time advance + tick SFX, CTR_CycleTex, top-attacker scan, the
// 0x11-bucket camera dispatch (VehPickupItem + funcPtrs[0..0xc] + ThreadBucketTick),
// particle/bots update, then the pause/unpause + end-of-race/high-score state machine.
// All struct offsets traced from asm; all 29 func_ IDs via syms926. gGT/gGamepads
// gp-relative (gp+0x340/0x344 → declared defined `=0`).
// (Phase B audit: saphi MainFrame_GameLogic verified CLEAN across all sections.)
#include "CTR.h"

/* ---- masks / constants (from asm) ---- */
#define PAUSE_ALL           0xF
#define PAUSE_1             0x1
#define DEBUG_MENU          0x10
#define END_OF_RACE         0x200000
#define ARCADE_MODE         0x400000
#define ACTION_RACE_FINISHED 0x2000000
#define GAME_CUTSCENE_ETC   0x20202000   /* GAME_CUTSCENE|END_OF_RACE|MAIN_MENU */
#define NEW_NAME            0x1000000
#define NEW_HIGH_SCORE      0x8000000
#define DRAW_HIGH_SCORES    0x2
#define BTN_START           0x1000
#define DRIVER_FUNC_COUNT   0xD
#define NUM_DISPATCH        0x11

/* ---- sub-structs (offsets traced from asm) ---- */
struct Level {
    char pad_14[0x14];
    int numModels;             /* 0x14 */
    void *ptrModelsPtrArray;   /* 0x18 */
};

struct Driver {
    char pad_C[0xC];
    short clockReceive;        /* 0xC  */
    char pad_45[0x45 - 0xE];
    unsigned char clockSend;   /* 0x45 */
    char pad_4A[0x4A - 0x46];
    unsigned char driverID;    /* 0x4A */
    char pad_54[0x54 - 0x4B];
    void *funcPtrs[13];        /* 0x54 (stride 4) */
    char pad_2C8[0x2C8 - (0x54 + 13 * 4)];
    int actionsFlagSet;        /* 0x2C8 */
    char pad_367[0x367 - 0x2CC];
    unsigned char clockFlash;  /* 0x367 (forcedJump_trampoline+1) */
    char pad_4F2[0x4F2 - 0x368];
    short quip2;               /* 0x4F2 */
    char pad_559[0x559 - 0x4F4];
    unsigned char numTimesAttacking; /* 0x559 */
};

struct Thread {
    char pad_10[0x10];
    struct Thread *siblingThread;  /* 0x10 */
    char pad_2C[0x2C - 0x14];
    void *funcThTick;              /* 0x2C */
    struct Driver *object;         /* 0x30 */
};

struct ThreadBucket {
    struct Thread *thread;     /* 0x0 */
    char pad_C[0xC - 0x4];
    int boolCantPause;         /* 0xC */
    char pad_14[0x14 - 0x10];
};

struct PushBuffer { char pad[0x110]; };

struct Gamepad {
    char pad_14[0x14];
    int buttonsTapped;         /* 0x14 */
    char pad_50[0x50 - 0x18];
};
struct GamepadSystem { struct Gamepad gamepad[8]; };

struct GameTracker {
    int gameMode1;                     /* 0x0    */
    int gameMode1_prevFrame;           /* 0x4    */
    char pad_160[0x160 - 0x8];
    struct Level *level1;              /* 0x160  */
    char pad_168[0x168 - 0x164];
    struct PushBuffer pushBuffer[1];   /* 0x168 (stride 0x110, walked) */
    char pad_1B2C[0x1B2C - (0x168 + 0x110)];
    struct ThreadBucket threadBuckets[0x12]; /* 0x1B2C (stride 0x14) */
    char pad_1CA8[0x1CA8 - (0x1B2C + 0x12 * 0x14)];
    unsigned char numPlyrCurrGame;     /* 0x1CA8 */
    unsigned char numPlyrNextGame;     /* 0x1CA9 */
    char pad_1CD4[0x1CD4 - 0x1CAA];
    int unk1cd4;                       /* 0x1CD4 (unk1cc4[4]) */
    char pad_1CEC[0x1CEC - 0x1CD8];
    int timer;                         /* 0x1CEC */
    char pad_1CFC[0x1CFC - 0x1CF0];
    int framesInThisLEV;               /* 0x1CFC */
    int msInThisLEV;                   /* 0x1D00 */
    int elapsedTimeMS;                 /* 0x1D04 */
    int clockFrameStart;               /* 0x1D08 */
    int trafficLightsTimer;            /* 0x1D0C */
    int elapsedEventTime;              /* 0x1D10 */
    char pad_1D34[0x1D34 - 0x1D14];
    short unk_timerCooldown;           /* 0x1D34 */
    short timerEndOfRaceVS;            /* 0x1D36 */
    unsigned char cooldownfromPauseUntilUnpause;  /* 0x1D38 */
    unsigned char cooldownFromUnpauseUntilPause;  /* 0x1D39 */
    char pad_1D44[0x1D44 - 0x1D3A];
    int gameModeEnd;                   /* 0x1D44 */
    char pad_1D49[0x1D49 - 0x1D48];
    signed char newHighScoreIndex;     /* 0x1D49 */
    char pad_1E20[0x1E20 - 0x1D4A];
    int frozenTimeRemaining;           /* 0x1E20 */
    char pad_2541[0x2541 - 0x1E24];
    unsigned char overlayIndex_Threads;/* 0x2541 */
    char pad_2570[0x2570 - 0x2542];
    unsigned short clockEffectEnabled; /* 0x2570 */
};

/* ---- gp-relative globals (defined = 0 → small-data) ---- */
struct GameTracker  *D_8008D2AC = 0;   /* sdata->gGT       (gp+0x340) */
struct GamepadSystem *D_8008D2B0 = 0;  /* sdata->gGamepads (gp+0x344) */
void *D_8008D870 = 0;                  /* sdata->PLYROBJECTLIST (gp+0x904) */
short D_8008D874 = 0;                   /* sdata->AkuAkuHintState (gp+0x908) */

/* ---- absolute globals ---- */
extern void *D_8008D908;               /* sdata->ptrActiveMenu */
extern int   D_8008D950;               /* sdata->AnyPlayerTap  */
extern unsigned short D_8008D918;      /* sdata->boolSaveCupProgress */
extern char D_80084190[];              /* menuRacingWheelConfig */
extern char D_800B518C[];              /* menuHintMenu */
extern char D_80085BE0[];              /* menuWarning2 */

/* ---- callees ---- */
extern void func_80023D4C(void *pushBuffer, int amt);                 /* DISPLAY_Blur_Main */
extern int  func_8004B41C(int prev, int *store);                      /* Timer_GetTime_Elapsed */
extern void func_800284D0(int a, int b, int c);                       /* OtherFX_Play_LowLevel */
extern void func_80021AC0(int numModels, void *models, int timer);    /* CTR_CycleTex_AllModels */
extern void func_800666E4(struct Driver *drv);                        /* VehPickupItem_ShootOnCirclePress */
extern void func_800715E8(struct Thread *thread);                     /* ThTick_RunBucket */
extern void func_80013374(void);                                      /* BOTS_UpdateGlobals */
extern void func_80027F20(int a);                                     /* GhostTape_WriteMoves */
extern void func_8003F434(void);                                      /* Particle_UpdateAllParticles */
extern int  func_800348E8(void);                                      /* LOAD_IsOpen_RacingOrBattle */
extern void func_800B39DC(void);                                      /* RB_Bubbles_RoosTubes */
extern void func_800B25B8(struct GameTracker *gGT);                   /* RB_Burst_DrawAll */
extern void func_80041FF4(void);                                      /* PROC_CheckAllForDead */
extern void func_8002D67C(void);                                      /* Audio_Update1 */
extern int  func_800255B4(struct GamepadSystem *gp);                  /* GAMEPAD_GetNumConnected */
extern void func_800550F4(void);                                      /* UI_VsQuipDrawAll */
extern void func_800552A4(void);                                      /* UI_VsWaitForPressX */
extern short func_8004AA60(int a);                                    /* SubmitName_DrawMenu */
extern void func_80048E2C(int a);                                     /* SelectProfile_ToggleMode */
extern void func_80046990(char *menu);                               /* RECTMENU_Show */
extern int  func_80043F1C(void);                                      /* RaceFlag_IsFullyOnScreen */
extern void func_80046404(void);                                      /* RECTMENU_ClearInput */
extern void func_80034B48(int bool_pause);                            /* MainFrame_TogglePauseAudio */
extern void func_80028468(int a, int b);                             /* OtherFX_Play */
extern void func_800399FC(void);                                      /* MainFreeze_SafeAdvDestroy */
extern void func_80024C08(struct GameTracker *gGT);                   /* ElimBG_Deactivate */
extern void func_800469C8(void *menu);                               /* RECTMENU_Hide */
extern int  func_80035D70(int numPlyrNext);                           /* MainFrame_HaveAllPads */
extern void func_80039E98(void);                                      /* MainFreeze_IfPressStart */

typedef void (*VehProc)(struct Thread *, struct Driver *);

void MainFrame_GameLogic(struct GameTracker *gGT, struct GamepadSystem *gGamepads)
{
    int bPaused;
    int iVar4;
    unsigned int uVar3;
    unsigned int uVar5;
    struct Driver *pTopAttacker;
    struct Driver *pDriverP2;
    struct Driver *pDriverTmp;
    struct Driver *curDriver;
    struct Driver *driver;
    struct PushBuffer *pushBuffer;
    struct Thread *pThread;
    int iProc;

    bPaused = 1;
    if ((gGT->gameMode1 & PAUSE_ALL) == 0)
    {
        bPaused = 0;
        pushBuffer = D_8008D2AC->pushBuffer;
        for (pThread = D_8008D2AC->threadBuckets[0].thread; pThread != 0; pThread = pThread->siblingThread)
        {
            driver = pThread->object;
            if (driver->clockSend)
                driver->clockSend--;
            uVar3 = driver->clockFlash;
            if (uVar3 == 0)
            {
                if (driver->clockReceive == 0)
                {
                    uVar3 = (unsigned int)driver->clockSend;
                    if (uVar3 == 0)
                    {
                        if ((D_8008D2AC->clockEffectEnabled & 1) == 0)
                            goto LAB_80034e74;
                        uVar3 = 10000;
                    }
                }
                else
                {
                    if ((driver->actionsFlagSet & ACTION_RACE_FINISHED) != 0)
                        driver->clockReceive = 0;
                    uVar3 = (unsigned int)driver->clockReceive;
                }
                func_80023D4C(pushBuffer, uVar3);
            }
            else
            {
                func_80023D4C(pushBuffer, -uVar3);
                driver->clockFlash--;
            }
        LAB_80034e74:
            pushBuffer = pushBuffer + 1;
        }

        gGT->timer = gGT->timer + 1;
        gGT->framesInThisLEV = gGT->framesInThisLEV + 1;
        D_8008D2AC->unk1cd4 = 0;

        iVar4 = func_8004B41C(gGT->clockFrameStart, &gGT->clockFrameStart);
        iVar4 = (iVar4 << 5) / 100;
        gGT->elapsedTimeMS = iVar4;
        if (iVar4 < 0)
            gGT->elapsedTimeMS = 0x20;
        if (0x40 < gGT->elapsedTimeMS)
            gGT->elapsedTimeMS = 0x40;
        if ((D_8008D2AC->gameMode1_prevFrame & PAUSE_ALL) != 0)
            gGT->elapsedTimeMS = 0x20;
        gGT->msInThisLEV = gGT->msInThisLEV + gGT->elapsedTimeMS;

        if (gGT->trafficLightsTimer < 1)
        {
            if ((D_8008D2AC->gameMode1 & DEBUG_MENU) == 0)
            {
                if (gGT->frozenTimeRemaining < 1)
                {
                    if ((D_8008D2AC->gameMode1 & END_OF_RACE) == 0)
                        gGT->elapsedEventTime = gGT->elapsedEventTime + gGT->elapsedTimeMS;
                }
                else
                {
                    iVar4 = gGT->frozenTimeRemaining - gGT->elapsedTimeMS;
                    gGT->frozenTimeRemaining = iVar4;
                    if (iVar4 < 0)
                        gGT->frozenTimeRemaining = 0;
                    else
                    {
                        uVar3 = D_8008D2AC->timer;
                        if (uVar3 == (uVar3 / 6) * 6)
                        {
                            if (uVar3 == (uVar3 / 0xc) * 0xc)
                                func_800284D0(0x40, 0, 0x8c9080);
                            else
                                func_800284D0(0x40, 0, 0x8c8880);
                        }
                    }
                }
            }
        }
        else
            gGT->elapsedEventTime = 0;

        func_80021AC0(-1, D_8008D870, gGT->timer);
        func_80021AC0(gGT->level1->numModels, gGT->level1->ptrModelsPtrArray, gGT->timer);

        pTopAttacker = 0;
        pDriverP2 = 0;
        for (pThread = D_8008D2AC->threadBuckets[0].thread; pThread != 0; pThread = pThread->siblingThread)
        {
            curDriver = pThread->object;
            pDriverTmp = pDriverP2;
            if (curDriver->driverID == 0)
            {
            LAB_80035098:
                pTopAttacker = curDriver;
                pDriverP2 = pDriverTmp;
            }
            else
            {
                if (curDriver->driverID == 1)
                    pDriverP2 = curDriver;
                pDriverTmp = pTopAttacker;
                if ((unsigned char)curDriver->numTimesAttacking < (unsigned char)pTopAttacker->numTimesAttacking)
                    goto LAB_80035098;
            }
        }
        if (pTopAttacker != 0 && pDriverP2 != 0 &&
            (iVar4 = (unsigned int)(unsigned char)pDriverP2->numTimesAttacking - (unsigned int)(unsigned char)pTopAttacker->numTimesAttacking,
             pTopAttacker->quip2 < iVar4))
        {
            pTopAttacker->quip2 = (short)iVar4;
        }

        for (iVar4 = 0; iVar4 < NUM_DISPATCH; iVar4++)
        {
            if ((((D_8008D2AC->gameMode1 & DEBUG_MENU) == 0) || ((gGT->threadBuckets[iVar4].boolCantPause & 1) != 0)) &&
                (gGT->threadBuckets[iVar4].thread != 0))
            {
                if (iVar4 == 0)
                {
                    for (pThread = D_8008D2AC->threadBuckets[0].thread; pThread != 0; pThread = pThread->siblingThread)
                        func_800666E4(pThread->object);

                    for (iProc = 0; iProc < DRIVER_FUNC_COUNT; iProc++)
                    {
                        for (pThread = D_8008D2AC->threadBuckets[0].thread; pThread != 0; pThread = pThread->siblingThread)
                        {
                            if (pThread->funcThTick != 0)
                                continue;
                            driver = pThread->object;
                            if (driver->funcPtrs[iProc] != 0)
                                ((VehProc)driver->funcPtrs[iProc])(pThread, driver);
                        }
                    }
                }
                func_800715E8(gGT->threadBuckets[iVar4].thread);
            }
        }

        func_80013374();
        func_80027F20(0);
        D_8008D2AC->unk1cd4 = (unsigned int)(D_8008D2AC->unk1cd4 * 10000) / 0x147e;
        func_8003F434();
    }
    else
    {
        pThread = gGT->threadBuckets[0xe].thread;
        if (pThread != 0)
            func_800715E8(pThread);
    }

    uVar5 = func_800348E8();
    if (uVar5 != 0)
    {
        if ((D_8008D2AC->gameMode1 & PAUSE_ALL) == 0)
            func_800B39DC();
        if (D_8008D2AC->threadBuckets[7].thread != 0)
            func_800B25B8(D_8008D2AC);
    }

    func_80041FF4();
    if ((D_8008D2AC->gameMode1 & PAUSE_ALL) == 0)
        func_8002D67C();

    gGT->gameMode1_prevFrame = gGT->gameMode1;
    uVar5 = func_800255B4(D_8008D2B0);
    uVar3 = D_8008D2AC->gameMode1;

    if ((uVar3 & END_OF_RACE) == 0)
    {
        if (bPaused || ((gGT->gameMode1 & PAUSE_ALL) != 0))
        {
            if (D_8008D2AC->cooldownfromPauseUntilUnpause == 0)
            {
                if ((D_8008D908 != (void *)D_80084190) && (D_8008D908 != (void *)D_800B518C) &&
                    ((D_8008D950 & BTN_START) != 0))
                {
                    func_80046404();
                    D_8008D2AC->gameMode1 = D_8008D2AC->gameMode1 & ~PAUSE_1;
                    func_80034B48(0);
                    func_80028468(1, 1);
                    func_800399FC();
                    func_80024C08(D_8008D2AC);
                    func_800469C8(D_8008D908);
                    D_8008D2AC->cooldownFromUnpauseUntilPause = 5;
                }
            }
            else
                D_8008D2AC->cooldownfromPauseUntilUnpause--;
        }
        else if (D_8008D2AC->cooldownFromUnpauseUntilPause == 0)
        {
            if ((uVar3 & GAME_CUTSCENE_ETC) == 0 && D_8008D908 == 0 && D_8008D874 == 0 &&
                func_80043F1C() == 0 && gGT->numPlyrCurrGame != 0)
            {
                for (iVar4 = 0; iVar4 < gGT->numPlyrCurrGame; iVar4++)
                {
                    if ((((uVar5 != 0) &&
                          ((uVar3 = func_80035D70((unsigned short)(unsigned char)D_8008D2AC->numPlyrNextGame),
                            (uVar3 & 0xffff) == 0 && ((gGT->gameMode1 & PAUSE_ALL) == 0)))) ||
                         ((gGamepads->gamepad[iVar4].buttonsTapped & BTN_START) != 0)) &&
                        (D_8008D2AC->overlayIndex_Threads != 0xff))
                    {
                        D_8008D2AC->gameModeEnd = (D_8008D2AC->gameMode1 & 0x3e0020) | PAUSE_1;
                        func_80039E98();
                        D_8008D2AC->cooldownfromPauseUntilUnpause = 5;
                    }
                }
            }
        }
        else
            D_8008D2AC->cooldownFromUnpauseUntilPause--;
    }
    else if (D_8008D2AC->timerEndOfRaceVS == 0)
    {
        uVar3 = D_8008D2AC->gameModeEnd;
        if ((uVar3 & NEW_NAME) == 0)
        {
            if ((uVar3 & NEW_HIGH_SCORE) == 0)
            {
                if (D_8008D2AC->unk_timerCooldown == 0)
                    return;
            }
            else if (D_8008D2AC->unk_timerCooldown == 0)
            {
                short sSubmitResult;
                if ((uVar3 & DRAW_HIGH_SCORES) == 0)
                    return;
                sSubmitResult = func_8004AA60(0x140);
                if (sSubmitResult == 0)
                    return;
                if (sSubmitResult == 1)
                {
                    D_8008D918 = 0;
                    func_80048E2C(0x41);
                    func_80046990(D_80085BE0);
                    D_8008D2AC->gameModeEnd = D_8008D2AC->gameModeEnd | NEW_NAME;
                    return;
                }
                D_8008D2AC->newHighScoreIndex = -1;
                D_8008D2AC->gameModeEnd = D_8008D2AC->gameModeEnd & ~(NEW_HIGH_SCORE | 0x4000000);
                return;
            }
            D_8008D2AC->unk_timerCooldown--;
        }
    }
    else if ((uVar3 & ARCADE_MODE) == 0)
    {
        if (D_8008D2AC->timerEndOfRaceVS < 0x96)
        {
            func_800550F4();
            func_800552A4();
        }
        if (0x1e < D_8008D2AC->timerEndOfRaceVS)
            D_8008D2AC->timerEndOfRaceVS--;
    }
    else
        D_8008D2AC->timerEndOfRaceVS = 0;
}
