// MainGameStart_Initialize @ 0x8003aee8  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 275; grind floor 55 was a non-natural
// gameModeFlag-reuse hack, not shipped). Residual: v0/v1 register naming for the
// gameMode1 temp + the final ~END_OF_RACE mask materialization. Behavior identical.
// Race-start init: if NOT cutscene/menu (gameMode1 & (GAME_CUTSCENE|MAIN_MENU)
// == 0x20002000) start traffic-light countdown 0xF00 + set START_OF_RACE(0x40);
// else clear both. Then clear END_OF_RACE(0x200000); if !boolStopAudio stop music+fx;
// VehBirth_TeleportAll(gGT,2). Check reads sdata->gGT (gp+0x340); stores use the
// gGT param. boolStopAudio is int (asm tests a1 as a full word).
#include "CTR.h"

#define GAME_CUTSCENE 0x20000000
#define MAIN_MENU     0x00002000
#define START_OF_RACE 0x40
#define END_OF_RACE   0x00200000

struct GameTracker { int gameMode1; char pad4[0x1D0C - 4]; int trafficLightsTimer; };  /* gameMode1@0, trafficLightsTimer@0x1D0C */

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */

extern void func_8002E4EC(void);                       /* Music_Stop */
extern void func_8002C8A8(int, int, int);              /* howl_StopAudio */
extern void func_80058898(struct GameTracker *, int);  /* VehBirth_TeleportAll */

void MainGameStart_Initialize(struct GameTracker *gGT, int boolStopAudio)
{
    u32 gameModeFlag;

    if ((D_8008D2AC->gameMode1 & (GAME_CUTSCENE | MAIN_MENU)) != 0)
    {
        gGT->trafficLightsTimer = 0;
        gameModeFlag = gGT->gameMode1 & ~START_OF_RACE;
    }
    else
    {
        gGT->trafficLightsTimer = 0xf00;
        gameModeFlag = gGT->gameMode1 | START_OF_RACE;
    }

    gGT->gameMode1 = gameModeFlag;
    gGT->gameMode1 &= ~END_OF_RACE;

    if (boolStopAudio == 0)
    {
        func_8002E4EC();
        func_8002C8A8(0, 0, 1);
    }

    func_80058898(gGT, 2);
}
