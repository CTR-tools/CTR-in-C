// MainInit_Drivers @ 0x8003b6d0  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 435, Reg 7; grind floor ~350 non-natural).
// Residual: branch delay-slot scheduling (retail fills slots gcc-2.8.1 leaves; 153
// vs 151 insns). Spawns human players (reverse order for thread-bucket list) + AI
// bots (arcade/adventure, <3 humans, not excluded: 8 for 1P / 6 for 2P / boss +1 /
// purple-gem-cup 1..4), engine-audio init if any bots, main-menu fill to 4, and
// time-trial ghost start. Match levers: store VehBirth_Player return only if nonzero
// (drivers[] pre-cleared); mixed access — some reads via the gGT param, others via
// gp-reloaded D_8008D2AC (gp+0x340), matching the asm exactly; bot-gate uses numPlyr!=3.
#include "CTR.h"

struct GameTracker {
    int gameMode1;                     /* 0x0    */
    char pad_1CA8[0x1CA8 - 0x4];
    unsigned char numPlyrCurrGame;     /* 0x1CA8 */
    char pad_1CAB[0x1CAB - 0x1CA9];
    unsigned char numBotsNextGame;     /* 0x1CAB */
    char pad_1E58[0x1E58 - 0x1CAC];
    int cupID;                         /* 0x1E58 (cup.cupID) */
    char pad_24EC[0x24EC - 0x1E5C];
    void *drivers[8];                  /* 0x24EC */
};

struct GameTracker *D_8008D2AC = 0;    /* sdata->gGT (gp+0x340) */

extern void  func_80012598(void);      /* BOTS_Adv_AdjustDifficulty */
extern void  func_80027838(void);      /* GhostReplay_Init1 */
extern int   func_800348E8(void);      /* LOAD_IsOpen_RacingOrBattle */
extern void  func_800ABFEC(void);      /* RB_MinePool_Init */
extern void *func_80058EC0(int);       /* VehBirth_Player */
extern void  func_80017164(int);       /* BOTS_Driver_Init */
extern void  func_80028880(int, int);  /* EngineAudio_InitOnce */
extern void  func_80027B88(void);      /* GhostReplay_Init2 */
extern void  func_80027DF4(void);      /* GhostTape_Start */

void MainInit_Drivers(struct GameTracker *gGT)
{
    int i;

    for (i = 7; i >= 0; i--)
        gGT->drivers[i] = 0;

    D_8008D2AC->numBotsNextGame = 0;

    if ((gGT->gameMode1 & 0x20102000) == 0)
        func_80012598();

    func_80027838();
    if (func_800348E8() != 0)
        func_800ABFEC();

    for (i = gGT->numPlyrCurrGame - 1; i >= 0; i--)
    {
        void *d = func_80058EC0(i);
        if (d != 0)
            gGT->drivers[i] = d;
    }

    if ((gGT->gameMode1 & 0x2c122020) == 0 && D_8008D2AC->numPlyrCurrGame != 3 &&
        (D_8008D2AC->gameMode1 & 0x480000) != 0)
    {
        if (D_8008D2AC->gameMode1 < 0)
        {
            func_80017164(1);
        }
        else if ((D_8008D2AC->gameMode1 & 0x10000000) != 0 && D_8008D2AC->cupID == 4)
        {
            for (i = 1; i < 5; i++)
                func_80017164(i);
        }
        else
        {
            int np = gGT->numPlyrCurrGame;
            int end;
            if (np == 1)
                end = 8;
            else if (np == 2)
                end = 6;
            else
                end = 4;
            for (i = np; i < end; i++)
                func_80017164(i);
        }
    }

    if (D_8008D2AC->numBotsNextGame != 0)
    {
        func_80028880(0x10, 0x8080);
        func_80028880(0x11, 0x8080);
    }

    if ((gGT->gameMode1 & 0x2000) != 0)
    {
        for (i = D_8008D2AC->numPlyrCurrGame; i < 4; i++)
        {
            void *d = func_80058EC0(i);
            if (d != 0)
                gGT->drivers[i] = d;
        }
    }

    if ((D_8008D2AC->gameMode1 & 0x20022000) == 0x20000)
    {
        func_80027B88();
        func_80027DF4();
    }
}
