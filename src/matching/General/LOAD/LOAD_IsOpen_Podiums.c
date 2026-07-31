// LOAD_IsOpen_Podiums @ 0x80034940  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Predicate: gGT->overlayIndex_Threads (@0x2541) == 3 (podiums overlay loaded).
// (== N compiles to xori;sltiu, or bare sltiu for ==0.)
#include "CTR.h"

struct GameTracker { char pad0[0x2541]; unsigned char overlayIndex_Threads; };   /* @0x2541 */

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */

int LOAD_IsOpen_Podiums(void)
{
    return D_8008D2AC->overlayIndex_Threads == 3;
}
