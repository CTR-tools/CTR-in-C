// MainInit_PrimMem @ 0x8003b0f0  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Sizes both DB prim-buffers (db[0].primMem @gGT+0x8C, db[1].primMem @gGT+0x130) via
// MainDB_PrimMem, keyed on levelID/gameMode1/numPlyrCurrGame (retail INLINES the size
// logic that saphi factors into MainInit_GetPrimMemSize):
//   ADVENTURE_GARAGE(0x28)->0x1B800; MAIN_MENU(gameMode1&0x2000)->0x17C00; else switch
//   numPlyrCurrGame: 0->0x25800; 1->(ADVENTURE_ARENA 0x1C000 / INTRO[0x1E,0x27) 0x1E000 /
//   primMem_SizePerLEV_1P[levelID]<<10 / default 0x5F<<10); 2->_2P[levelID]<<10 or 0x78<<10;
//   3/4->_4P[levelID]<<10 or 0x96<<10; >=5 -> no-op.
// Match levers: each case does its OWN two MainDB_PrimMem calls (gcc tail-merges the
// identical 2nd call), defaults 0x5F/0x78/0x96 hoisted to top, case-1 gameMode1/levelID
// read via gp-reloaded gGT (D_8008D2AC) while the table index uses the param.
#include "CTR.h"

struct GameTracker {
    int gameMode1;                    /* 0x0    */
    char pad_1A10[0x1A10 - 0x4];
    int levelID;                      /* 0x1A10 */
    char pad_1CA8[0x1CA8 - 0x1A14];
    unsigned char numPlyrCurrGame;    /* 0x1CA8 */
};

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT (gp+0x340) */
extern unsigned char D_8008454C[];    /* data.primMem_SizePerLEV_1P */
extern unsigned char D_80084568[];    /* data.primMem_SizePerLEV_2P */
extern unsigned char D_80084584[];    /* data.primMem_SizePerLEV_4P */

extern void func_800349C4(void *primMem, int size);   /* MainDB_PrimMem */

void MainInit_PrimMem(struct GameTracker *gGT)
{
    int a2 = 0x5F;
    int a1 = 0x78;
    int s0 = 0x96;

    if (gGT->levelID == 0x28)
    {
        func_800349C4((char *)gGT + 0x8C, 0x1B800);
        func_800349C4((char *)gGT + 0x130, 0x1B800);
        return;
    }
    if ((gGT->gameMode1 & 0x2000) != 0)
    {
        func_800349C4((char *)gGT + 0x8C, 0x17C00);
        func_800349C4((char *)gGT + 0x130, 0x17C00);
        return;
    }

    switch (gGT->numPlyrCurrGame)
    {
    case 0:
        func_800349C4((char *)gGT + 0x8C, 0x25800);
        func_800349C4((char *)gGT + 0x130, 0x25800);
        return;
    case 1:
        if ((D_8008D2AC->gameMode1 & 0x100000) != 0)
        {
            func_800349C4((char *)gGT + 0x8C, 0x1C000);
            func_800349C4((char *)gGT + 0x130, 0x1C000);
            return;
        }
        if ((unsigned int)(D_8008D2AC->levelID - 0x1E) < 9)
        {
            func_800349C4((char *)gGT + 0x8C, 0x1E000);
            func_800349C4((char *)gGT + 0x130, 0x1E000);
            return;
        }
        if (gGT->levelID < 0x19)
            a2 = D_8008454C[gGT->levelID];
        func_800349C4((char *)gGT + 0x8C, a2 << 10);
        func_800349C4((char *)gGT + 0x130, a2 << 10);
        return;
    case 2:
        if (gGT->levelID < 0x19)
            a1 = D_80084568[gGT->levelID];
        func_800349C4((char *)gGT + 0x8C, a1 << 10);
        func_800349C4((char *)gGT + 0x130, a1 << 10);
        return;
    case 3:
    case 4:
        if (gGT->levelID < 0x19)
            s0 = D_80084584[gGT->levelID];
        func_800349C4((char *)gGT + 0x8C, s0 << 10);
        func_800349C4((char *)gGT + 0x130, s0 << 10);
        return;
    }
}
