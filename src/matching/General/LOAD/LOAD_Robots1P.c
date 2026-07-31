// LOAD_Robots1P @ 0x800327dc  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// 1P AI fill: characterIDs[0]=player, then fill characterIDs[1..7] with 0..7
// skipping the player id. Key match levers: compound loop cond `newCID<8 && i<8`
// (retail guards i<8 at top, newCID<8 at back-edge) + hoisting `i=1` above the
// characterIDs[0] store so gcc emits it before the store.
#include "CTR.h"

extern short D_80086E84[];   // data.characterIDs (short[])

void LOAD_Robots1P(int characterID)
{
    int i = 1;
    int newCID;

    D_80086E84[0] = characterID;

    for (newCID = 0; newCID < 8 && i < 8; newCID++)
    {
        if (newCID != characterID)
        {
            D_80086E84[i] = newCID;
            i++;
        }
    }
}
