// LOAD_Hub_Main @ 0x80033318  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Per-frame adventure-hub driver: for each player, read its stepFlagSet trigger
// bits and either queue the connected hub level (LOAD_Hub_ReadFile via the
// connectedLev[currHub][nextLevelID-1] table, only when in a hub 0..4) or perform
// a deferred LOAD_Hub_SwapNow. connectedLev is a local table copied from retail
// rdata @0x80011180 (values Ghidra-verified; match saphi s_advHubConnectedLevID).
// Match levers: local `int connectedLev[5][3]` initializer (copied to stack),
// per-iteration Loading.stage!=LOAD_IDLE(-1) `continue`, direct D_8008D2AC access
// (gGT reloaded per iteration), and ReadFile(nextLevelID!=0) branch as fall-through.
// NOTE: saphi checks Loading.stage once (early return) instead of per-iteration —
// behaviorally identical because no hub loader/callback changes it mid-loop.
#include "CTR.h"

struct Driver {
    char pad0[0xBC];
    int stepFlagSet;   /* 0xBC */
};

struct GameTracker {
    char pad_1A10[0x1A10];
    int levelID;                          /* 0x1A10 */
    char pad_1CA8[0x1CA8 - 0x1A14];
    unsigned char numPlyrCurrGame;        /* 0x1CA8 */
    char pad_24EC[0x24EC - 0x1CA9];
    struct Driver *drivers[8];            /* 0x24EC */
    char pad_254A[0x254A - (0x24EC + 32)];
    short activeMempackIndex;             /* 0x254A */
    char pad_2574[0x2574 - 0x254C];
    unsigned char bool_AdvHub_NeedToSwapLEV;   /* 0x2574 */
};

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
extern int D_8008D0F8;                    /* sdata->Loading.stage (absolute); LOAD_IDLE = -1 */

extern void func_80033108(void);                             /* LOAD_Hub_SwapNow */
extern void func_80032FFC(void *bigfile, int levID, int packID);   /* LOAD_Hub_ReadFile */

void LOAD_Hub_Main(void *bigfilePtr)
{
    int connectedLev[5][3] = {
        {0x1A, 0x1B, -1},
        {0x19, 0x1C, -1},
        {0x19, 0x1C, -1},
        {0x1A, 0x1B, 0x1D},
        {0x1C, -1, -1},
    };
    int i;

    for (i = 0; i < D_8008D2AC->numPlyrCurrGame; i++)
    {
        int stepFlagSet, nextLevelID, needSwapNow;

        if (D_8008D0F8 != -1)
            continue;

        stepFlagSet = D_8008D2AC->drivers[i]->stepFlagSet;
        nextLevelID = (stepFlagSet & 0x30) >> 4;
        needSwapNow = (stepFlagSet & 0xC0) >> 6;

        if (nextLevelID != 0)
        {
            u32 currLevelID = D_8008D2AC->levelID - 0x19;
            if (currLevelID < 5)
                func_80032FFC(bigfilePtr, connectedLev[currLevelID][nextLevelID - 1], 3 - D_8008D2AC->activeMempackIndex);
        }
        else
        {
            if (needSwapNow != 0 || D_8008D2AC->bool_AdvHub_NeedToSwapLEV != 0)
            {
                D_8008D2AC->bool_AdvHub_NeedToSwapLEV = 0;
                func_80033108();
            }
        }
    }
}
