// MainStats_RestartRaceCountLoss @ 0x8003d068  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 1785, grind floor ~1055 non-natural).
// Residual: gcc-2.8.1 strength-reduces the team-loop bit test to (teamFlags>>i)&1
// (srav+andi) where retail keeps (teamFlags & 1<<i) (sllv+and, per-iter reload) +
// the cascading register allocation. Behavior identical (71 vs 70 insns).
// On race restart: clear HUD bit0; per team set pointsPerTeam[i]=0 if active else
// -500 (so an inactive team doesnt out-rank a -1 player); zero the finish/winner/
// crystal/time-crate/missile/frozen counters + TurboDisplayPos/numPlayersFinished.
// Then ONLY in plain Adventure (not Cup/Relic) and only if quit on lap 3: bump the
// per-level loss counter (advProgress+levelID+0x30), or the boss counter
// (+bossID+0x47) when in a boss race UNLESS it is capped at 10 (then falls through
// to bump the level counter), each capped at 10.
#include "CTR.h"

#define ADVENTURE_MODE  0x80000
#define RELIC_RACE      0x4000000
#define ADVENTURE_CUP   0x10000000

struct Driver { char pad0[0x44]; unsigned char lapIndex; };

struct GameTracker {
    int gameMode1;                       /* 0x0    */
    char pad_1A10[0x1A10 - 0x4];
    int levelID;                         /* 0x1A10 */
    char pad_1D31[0x1D31 - 0x1A14];
    unsigned char hudFlags;              /* 0x1D31 */
    char pad_1D44[0x1D44 - 0x1D32];
    int gameModeEnd;                     /* 0x1D44 */
    char pad_1D90[0x1D90 - 0x1D48];
    int pointsPerTeam[4];                /* 0x1D90 */
    char pad_1DD8[0x1DD8 - 0x1DA0];
    int teamFlags;                       /* 0x1DD8 */
    char pad_1E20[0x1E20 - 0x1DDC];
    int frozenTimeRemaining;             /* 0x1E20 */
    char pad_1E28[0x1E28 - 0x1E24];
    int numCrystalsInLEV;                /* 0x1E28 */
    int timeCratesInLEV;                 /* 0x1E2C */
    char pad_1EB8[0x1EB8 - 0x1E30];
    int bossID;                          /* 0x1EB8 */
    char pad_1EC0[0x1EC0 - 0x1EBC];
    int numMissiles;                     /* 0x1EC0 */
    int numPlayersWith3Missiles;         /* 0x1EC4 */
    char pad_24EC[0x24EC - 0x1EC8];
    struct Driver *drivers[8];           /* 0x24EC */
    char pad_2568[0x2568 - (0x24EC + 32)];
    int numWinners;                      /* 0x2568 */
};

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */
short D_8008D2A8 = 0;   /* sdata->TurboDisplayPos_Only1P (gp+0x33C) */
short D_8008D2AA = 0;   /* sdata->numPlayersFinishedRace (gp+0x33E) */
extern char D_8008FBA4[];   /* adv-progress: level counters @+0x30, boss @+0x47 */

void MainStats_RestartRaceCountLoss(void)
{
    struct GameTracker *gGT = D_8008D2AC;
    int i;
    int gameMode1;

    gGT->hudFlags &= 0xFE;

    for (i = 0; i < 4; i++)
    {
        if ((gGT->teamFlags & (1 << i)) == 0)
            gGT->pointsPerTeam[i] = -500;
        else
            gGT->pointsPerTeam[i] = 0;
    }

    gameMode1 = D_8008D2AC->gameMode1;
    D_8008D2A8 = 0;
    D_8008D2AA = 0;
    D_8008D2AC->numWinners = 0;
    D_8008D2AC->numCrystalsInLEV = 0;
    D_8008D2AC->timeCratesInLEV = 0;
    D_8008D2AC->gameModeEnd = 0;
    D_8008D2AC->frozenTimeRemaining = 0;
    D_8008D2AC->numMissiles = 0;
    D_8008D2AC->numPlayersWith3Missiles = 0;

    if ((gameMode1 & ADVENTURE_MODE) == 0)
        return;
    if (D_8008D2AC->drivers[0]->lapIndex != 2)
        return;
    if ((gameMode1 & (ADVENTURE_CUP | RELIC_RACE)) != 0)
        return;

    if (gameMode1 < 0)
    {
        char *bc = &D_8008FBA4[D_8008D2AC->bossID + 0x17];
        if ((signed char)bc[0x30] < 10)
        {
            bc[0x30] = bc[0x30] + 1;
            return;
        }
    }
    {
        char *lc = &D_8008FBA4[D_8008D2AC->levelID];
        if ((signed char)lc[0x30] < 10)
            lc[0x30] = lc[0x30] + 1;
    }
}
