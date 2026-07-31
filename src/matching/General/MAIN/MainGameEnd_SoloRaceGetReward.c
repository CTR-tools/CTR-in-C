// MainGameEnd_SoloRaceGetReward @ 0x80039fa8  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 7450, 195==195 insns; grind floor ~7305 non-natural).
// Residual: whole-function register allocation (which reg holds applyCrateBonus/gGT
// cascades throughout this register-heavy fn). Behavior identical.
// Solo-race reward eval: clear rank/lap bits, high-score rank scan (crate time bonus
// 0x2580), best-lap check, and the time-trial unlock tiers (dev-time -> 0x08008000,
// event-first bit1 -> 0x18000000, event-repeat bit2 -> 0x08800000), then scrapbook
// ghost unlock. gGT gp-relative (gp+0x340); reward bit-indices/masks are runtime
// globals D_8008453C/40/44/48; gameProgress bit-array via D_8008E6E8 offsets.
#include "CTR.h"

struct HighScoreEntry { int time; char pad[0x18 - 4]; };

struct Driver { char pad0[0x32]; signed char numTimeCrates; char pad33[0x514 - 0x33]; int timeElapsedInRace; };
struct Thread { char pad0[0x30]; struct Driver *object; };

struct GameTracker {
    int gameMode1;                       /* 0x0    */
    char pad_1A10[0x1A10 - 0x4];
    int levelID;                         /* 0x1A10 */
    char pad_1B2C[0x1B2C - 0x1A14];
    struct Thread *threadBucket0Thread;  /* 0x1B2C */
    char pad_1D14[0x1D14 - 0x1B30];
    int lapTime[7];                      /* 0x1D14 (ends 0x1D30) */
    char pad_1D33[0x1D33 - 0x1D30];
    signed char numLaps;                 /* 0x1D33 */
    char pad_1D3C[0x1D3C - 0x1D34];
    int bestLapTime;                     /* 0x1D3C */
    int lapIndexNewBest;                 /* 0x1D40 */
    int nGameModeEnd;                    /* 0x1D44 */
    char pad_1D49[0x1D49 - 0x1D48];
    signed char newHighScoreIndex;       /* 0x1D49 */
    char pad_1D78[0x1D78 - 0x1D4A];
    int timeToBeatInTimeTrial;           /* 0x1D78 */
    char pad_1E2C[0x1E2C - 0x1D7C];
    int timeCratesInLEV;                 /* 0x1E2C */
    char pad_24EC[0x24EC - 0x1E30];
    struct Driver *drivers[8];           /* 0x24EC */
};

struct GameTracker *D_8008D2AC = 0;          /* sdata->gGT (gp+0x340) */
extern struct HighScoreEntry *D_8008D738;    /* ptrActiveHighScoreEntry (absolute) */
extern char D_8008E6E8[];                    /* gameProgress base (absolute) */
extern char D_80083A80[];                    /* metaDataLEV (stride 0x18, .timeTrial@0xC) */
extern unsigned short D_8008453C;            /* g_nTtBitEventFirst */
extern unsigned short D_80084540;            /* g_nTtBitEventRepeat */
extern int D_80084544;                       /* event-repeat reward mask */
extern int D_80084548;                       /* event-first reward mask */

extern int func_80026AE4(int);               /* GAMEPROG_CheckGhostsBeaten */

void MainGameEnd_SoloRaceGetReward(int applyCrateBonus)
{
    struct GameTracker *gGT = D_8008D2AC;
    struct Driver *player = gGT->threadBucket0Thread->object;
    int crateBonus;
    short i;

    gGT->newHighScoreIndex = -1;
    gGT->nGameModeEnd &= 0x7bffffff;

    crateBonus = 0;
    if ((int)gGT->drivers[0]->numTimeCrates == gGT->timeCratesInLEV && applyCrateBonus != 0)
        crateBonus = 0x2580;

    for (i = 0; i < 5; i++)
    {
        if (D_8008D2AC->drivers[0]->timeElapsedInRace - crateBonus < D_8008D738[i + 1].time)
        {
            gGT->newHighScoreIndex = (char)i;
            gGT->nGameModeEnd |= 0x88000000;
            break;
        }
    }

    gGT = D_8008D2AC;
    gGT->bestLapTime = D_8008D738[0].time;
    for (i = 0; i < gGT->numLaps; i++)
    {
        if (gGT->lapTime[i] < gGT->bestLapTime)
        {
            gGT->bestLapTime = gGT->lapTime[i];
            gGT->lapIndexNewBest = i;
            gGT->nGameModeEnd |= 0x8c000000;
        }
    }

    gGT = D_8008D2AC;
    if ((D_8008D2AC->gameMode1 & 0x20000) == 0)
        return;
    if ((D_8008D2AC->nGameModeEnd & 4) != 0)
        return;
    D_8008D2AC->nGameModeEnd |= 4;

    {
        int levelID = gGT->levelID;
        int firstIdx = (int)(short)D_8008453C >> 5;
        unsigned int *pFirst = (unsigned int *)(D_8008E6E8 + 0x12C + levelID * 0x124 + firstIdx * 4);
        if ((*pFirst >> (D_8008453C & 0x1f) & 1) == 0)
        {
            unsigned int flags = *(unsigned int *)(D_8008E6E8 + 0x12C + levelID * 0x124);
            if ((flags & 1) == 0)
            {
                if (*(int *)(D_80083A80 + levelID * 0x18 + 0xC) <= player->timeElapsedInRace)
                    goto ghosts;
                *(unsigned int *)(D_8008E6E8 + 0x12C + levelID * 0x124) = flags | 1;
                gGT->nGameModeEnd |= 0x8008000;
            }
            else
            {
                if (gGT->timeToBeatInTimeTrial <= player->timeElapsedInRace)
                    goto ghosts;
                *pFirst = *pFirst | (1 << (D_8008453C & 0x1f));
                gGT->nGameModeEnd |= D_80084548;
            }
        }
        else
        {
            if (gGT->timeToBeatInTimeTrial <= player->timeElapsedInRace)
                goto ghosts;
            {
                int repIdx = (int)(short)D_80084540 >> 5;
                unsigned int *pRep = (unsigned int *)(D_8008E6E8 + 0x12C + levelID * 0x124 + repIdx * 4);
                *pRep = *pRep | (1 << (D_80084540 & 0x1f));
                gGT->nGameModeEnd |= D_80084544;
            }
        }
    }

ghosts:
    if ((func_80026AE4(2) & 0xffff) != 0)
        *(int *)(D_8008E6E8 + 8) |= 0x10;
}
