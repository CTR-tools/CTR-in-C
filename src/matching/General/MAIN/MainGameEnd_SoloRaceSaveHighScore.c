// MainGameEnd_SoloRaceSaveHighScore @ 0x8003a2b4  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 1530, Reg 18). Residual: instruction
// scheduling — retail folds the characterID store into the memmove branch-delay
// slot + HIGH_SCORE_SAVED-check delay-slot fill. Behavior identical.
// Commits the solo-race result into the high-score table (ptrActiveHighScoreEntry):
// GetReward(0), then (once, gated by nGameModeEnd&HIGH_SCORE_SAVED=0x10000 which it
// sets): if NEW_BEST_LAP(0x4000000) write entry[0]={bestLapTime, prevNameEntered,
// characterIDs[driverID]}; if newHighScoreIndex>=0 shift entries [idx+1..5] down
// ((4-idx)*0x18 when idx<4) and insert {timeElapsedInRace, name, charID} at [idx+1].
// gGT gp-relative (gp+0x340; player=drivers[0] held, gGT reloaded per block);
// ptrActiveHighScoreEntry + characterIDs absolute; memmove=func_80077E38.
#include "CTR.h"

#define HIGH_SCORE_SAVED 0x10000
#define NEW_BEST_LAP     0x4000000

struct HighScoreEntry {
    int  time;              /* 0x0  */
    char name[0x11];        /* 0x4  */
    char pad15[1];          /* 0x15 */
    short characterID;      /* 0x16 */
};                          /* size 0x18 */

struct Driver { char pad0[0x4A]; unsigned char driverID; char pad4b[0x514 - 0x4B]; int timeElapsedInRace; };

struct GameTracker {
    char pad_1D3C[0x1D3C];
    int bestLapTime;               /* 0x1D3C */
    char pad_1D44[0x1D44 - 0x1D40];
    int nGameModeEnd;              /* 0x1D44 */
    char pad_1D49[0x1D49 - 0x1D48];
    signed char newHighScoreIndex; /* 0x1D49 */
    char pad_1D54[0x1D54 - 0x1D4A];
    char prevNameEntered[0x11];    /* 0x1D54 */
    char pad_24EC[0x24EC - (0x1D54 + 0x11)];
    struct Driver *drivers[8];     /* 0x24EC */
};

struct GameTracker *D_8008D2AC = 0;         /* sdata->gGT (gp+0x340) */
extern struct HighScoreEntry *D_8008D738;   /* sdata->ptrActiveHighScoreEntry (absolute) */
extern short D_80086E84[];                  /* data.characterIDs (absolute) */

extern void func_80039FA8(int);             /* MainGameEnd_SoloRaceGetReward */
extern void *func_80077E38(void *dst, void *src, int n);   /* memmove */

void MainGameEnd_SoloRaceSaveHighScore(void)
{
    struct Driver *player = D_8008D2AC->drivers[0];
    int gameModeEnd;

    func_80039FA8(0);

    gameModeEnd = D_8008D2AC->nGameModeEnd;
    if ((gameModeEnd & HIGH_SCORE_SAVED) == 0)
    {
        struct HighScoreEntry *entry;
        int idx;

        D_8008D2AC->nGameModeEnd = gameModeEnd | HIGH_SCORE_SAVED;

        if ((gameModeEnd & NEW_BEST_LAP) != 0)
        {
            D_8008D738[0].time = D_8008D2AC->bestLapTime;
            D_8008D738[0].name[0] = 0;
            D_8008D738[0].characterID = D_80086E84[player->driverID];
            func_80077E38(D_8008D738[0].name, D_8008D2AC->prevNameEntered, 0x11);
        }

        idx = D_8008D2AC->newHighScoreIndex;
        if (idx >= 0)
        {
            entry = &D_8008D738[idx + 1];
            if (idx < 4)
                func_80077E38(&entry[1], entry, (4 - idx) * 0x18);
            entry->time = player->timeElapsedInRace;
            entry->name[0] = 0;
            entry->characterID = D_80086E84[player->driverID];
            func_80077E38(entry->name, D_8008D2AC->prevNameEntered, 0x11);
        }
    }
}
