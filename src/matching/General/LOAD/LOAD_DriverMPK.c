// LOAD_DriverMPK @ 0x8003282c  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 485, grind found nothing lower).
// Residual: pure register-allocation cascade — retail holds the gGT pointer in
// $a2 (and the -2 SetPointer sentinel in a saved reg), gcc-2.8.1 uses $a1 and
// rematerializes -2; behavior identical.
// Driver/MPK loader dispatcher keyed on levelLOD (3P/4P, 1P w/ adventure &
// time-trial & purple-gem sub-cases, hi+pack, med/2P). Match levers: defer the
// gGT/gameMode1 loads into the branches that use them (retail rematerializes),
// UNROLL the model-load loops (gcc-2.8.1 -O2 does not auto-unroll), and INLINE
// the final AppendQueue into each block (no shared QueueLastPack tail).
// LOAD_DriverMPK_SetPointer == (void*)-2 sentinel (see saphi LOAD_Assets.c).
#include "CTR.h"

#define LT_GETADDR          2
#define BI_RACERMODELHI     0xF2
#define BI_1PARCADEPACK     0x104
#define BI_ADVENTUREPACK    0x114
#define BI_TIMETRIALPACK    0x124
#define BI_RACERMODELMED    0x134
#define BI_2PARCADEPACK     0x144
#define BI_RACERMODELLOW    0x14C
#define BI_4PARCADEPACK     0x15C

#define TIME_TRIAL          0x20000
#define MAIN_MENU           0x2000
#define GAME_CUTSCENE_ARENA 0x20100000
#define CREDITS             0x80
#define ADVENTURE_GARAGE    0x28
#define ADVENTURE_BOSS      0x80000000
#define ADVENTURE_CUP       0x10000000

#define RIPPER_ROO 0xA
#define PAPU_PAPU  0x9
#define KOMODO_JOE 0xB
#define PINSTRIPE  0x8

#define SetPointer ((void *)-2)

struct GameTracker {
    int gameMode1;      /* 0x0    */
    char pad0[4];
    int gameMode2;      /* 0x8    */
    char pad1[0x1A04];
    int levelID;        /* 0x1A10 */
    char pad2[0x444];
    int cupID;          /* 0x1E58 */
};

extern short D_80086E84[];               /* data.characterIDs */
extern int   D_80083A10[];               /* data.driverModelExtras[i].fileBase (stride 4) */
extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
int D_8008D088;                          /* sdata->ptrMPK (gp+0x11C) */

extern void func_80032D30(void *bigfile, int type, int index, void *dest, void *callback); /* LOAD_AppendQueue */
extern void func_800327DC(int characterID);                                                /* LOAD_Robots1P   */
extern void func_80032700(void *bigfile, int p1, int p2, void *callback);                  /* LOAD_Robots2P   */

int LOAD_DriverMPK(void *bigfile, int levelLOD, void *callback)
{
    if (levelLOD - 3U < 2)
    {
        func_80032D30(bigfile, LT_GETADDR, BI_RACERMODELLOW + D_80086E84[0], &D_80083A10[0], SetPointer);
        func_80032D30(bigfile, LT_GETADDR, BI_RACERMODELLOW + D_80086E84[1], &D_80083A10[1], SetPointer);
        func_80032D30(bigfile, LT_GETADDR, BI_RACERMODELLOW + D_80086E84[2], &D_80083A10[2], SetPointer);
        func_80032D30(bigfile, LT_GETADDR, BI_4PARCADEPACK + D_80086E84[3], 0, callback);
    }
    else if (levelLOD == 1)
    {
        struct GameTracker *gGT = D_8008D2AC;
        int gameMode1 = gGT->gameMode1;

        if ((gameMode1 & (TIME_TRIAL | MAIN_MENU)) == TIME_TRIAL)
            goto LoadHighAndPack;

        if (((gameMode1 & GAME_CUTSCENE_ARENA) != 0) ||
            ((gGT->gameMode2 & CREDITS) != 0) ||
            (gGT->levelID == ADVENTURE_GARAGE))
        {
            func_80032D30(bigfile, LT_GETADDR, BI_ADVENTUREPACK + D_80086E84[0], 0, callback);
            return D_8008D088;
        }

        if (gameMode1 & ADVENTURE_BOSS)
            goto LoadHighAndPack;

        if (((gameMode1 & ADVENTURE_CUP) != 0) && (gGT->cupID == 4))
        {
            func_80032D30(bigfile, LT_GETADDR, BI_RACERMODELHI + D_80086E84[0], &D_80083A10[0], SetPointer);
            func_80032D30(bigfile, LT_GETADDR, BI_2PARCADEPACK + 7, 0, callback);
            D_80086E84[1] = RIPPER_ROO;
            D_80086E84[2] = PAPU_PAPU;
            D_80086E84[3] = KOMODO_JOE;
            D_80086E84[4] = PINSTRIPE;
            return D_8008D088;
        }

        if ((D_8008D2AC->gameMode1 & (TIME_TRIAL | MAIN_MENU)) != MAIN_MENU)
            func_800327DC(D_80086E84[0]);

        func_80032D30(bigfile, LT_GETADDR, BI_1PARCADEPACK + D_80086E84[0], 0, callback);
    }
    else if ((levelLOD == 8) || ((D_8008D2AC->gameMode1 & TIME_TRIAL) != 0))
    {
    LoadHighAndPack:
        func_80032D30(bigfile, LT_GETADDR, BI_RACERMODELHI + D_80086E84[0], &D_80083A10[0], SetPointer);
        func_80032D30(bigfile, LT_GETADDR, BI_TIMETRIALPACK + D_80086E84[1], 0, callback);
    }
    else
    {
        func_80032D30(bigfile, LT_GETADDR, BI_RACERMODELMED + D_80086E84[0], &D_80083A10[0], SetPointer);
        func_80032D30(bigfile, LT_GETADDR, BI_RACERMODELMED + D_80086E84[1], &D_80083A10[1], SetPointer);
        func_80032700(bigfile, D_80086E84[0], D_80086E84[1], callback);
    }

    return D_8008D088;
}
