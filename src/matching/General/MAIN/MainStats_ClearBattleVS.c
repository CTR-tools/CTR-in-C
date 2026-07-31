// MainStats_ClearBattleVS @ 0x8003d024  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Zeroes gGT->standingsPoints[4][3] (4 players x 3 ranks = 12 ints) @0x1E80 via a
// nested 4x3 loop. gGT gp-relative (gp+0x340). Match lever: the gGT load sits at
// point-of-use inside the inner loop (gcc CSE-hoists it, but this yields the retail
// scheduling of counter-init before the gp-load). saphi uses an equivalent flat
// for(i<12) loop — behaviorally identical.
#include "CTR.h"

struct GameTracker { char pad0[0x1E80]; int standingsPoints[4][3]; };   /* @0x1E80, 4 players x 3 ranks */

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */

void MainStats_ClearBattleVS(void)
{
    int player, rank;

    for (player = 0; player < 4; player++)
    {
        for (rank = 0; rank < 3; rank++)
        {
            struct GameTracker *gGT = D_8008D2AC;
            gGT->standingsPoints[player][rank] = 0;
        }
    }
}
