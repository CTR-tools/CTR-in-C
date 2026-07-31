// MainInit_JitPoolsNew @ 0x8003b43c  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 1765, Reg 41; grind floor ~1705 non-natural).
// Residual: whole-function register allocation — retail holds gGT(param) in $s2 and
// allocSize in $s3; gcc-2.8.1 swaps them (cascades throughout). Behavior identical.
// Allocates + inits all 8 GameTracker JIT pools (JitPool_Init) sized by game mode:
// sizeFactor 0x400 menu / 0x800 adv-arena / 0x1000 racing; allocSize same but 0x800
// for garage. Pools: thread(allocSize*3>>7,0x48), instance(allocSize>>5,0x74+numPlyr*0x88),
// small(sizeFactor*0x19>>10,0x48), medium(sizeFactor>>7,0x88), large(menu?4:sizeFactor>>9,
// 0x670), particle(sizeFactor>>5,0x7c), oscillator(sizeFactor>>5,0x18), rain(sizeFactor>>9,
// 0x28). Then ptrRenderBucketInstance=AllocMem(allocSize), self-link small/medium/large free
// items (3 unrolled loops), and per-player clip buffers = AllocMem(GetClipSize<<2).
// Match note: gameMode1/levelID/JitPools via the gGT param; instance-numPlyr,
// ptrRenderBucketInstance & the clip loop read sdata->gGT gp-relative (D_8008D2AC).
#include "CTR.h"

#define ADVENTURE_ARENA  0x100000
#define MAIN_MENU        0x2000
#define ADVENTURE_GARAGE 0x28

struct JitPool { void *freeFirst; char pad_28[0x28 - 4]; };   /* free.first @0x0, stride 0x28 */

struct GameTracker {
    int gameMode1;                       /* 0x0    */
    char pad_18D0[0x18D0 - 0x4];
    struct JitPool JitPools[8];          /* 0x18D0 (thread,instance,small,medium,large,particle,osc,rain) */
    int levelID;                         /* 0x1A10 */
    char pad_1C94[0x1C94 - 0x1A14];
    void *ptrRenderBucketInstance;       /* 0x1C94 */
    char pad_1CA8[0x1CA8 - 0x1C98];
    unsigned char numPlyrCurrGame;       /* 0x1CA8 */
};

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT (gp+0x340) */
extern void *D_80084228[];               /* data.PtrClipBuffer[] */

/* pool-name strings */
extern char D_80011398[], D_800113A4[], D_800113B4[], D_800113C4[], D_800113D4[];
extern char D_800113E4[], D_800113F4[], D_80011404[], D_80011410[], D_80011428[];

extern void  func_8003E978(void);                              /* MEMPACK_PushState */
extern void  func_8003105C(void *pool, int count, int itemSize, char *name);  /* JitPool_Init */
extern void *func_8003E874(int size, char *name);              /* MEMPACK_AllocMem */
extern int   func_80034960(int levelID, int numPlyr);          /* MainDB_GetClipSize */

void MainInit_JitPoolsNew(struct GameTracker *gGT)
{
    int gameMode1 = gGT->gameMode1;
    int sizeFactor;
    int allocSize;
    int maxItems;
    void *item;
    int i;

    sizeFactor = 0x800;
    if ((gameMode1 & ADVENTURE_ARENA) == 0)
    {
        sizeFactor = 0x1000;
        if ((gameMode1 & MAIN_MENU) != 0)
            sizeFactor = 0x400;
    }

    allocSize = 0x800;
    if ((gameMode1 & ADVENTURE_ARENA) == 0)
    {
        if ((gameMode1 & MAIN_MENU) == 0)
            allocSize = 0x1000;
        else
        {
            allocSize = 0x400;
            if (gGT->levelID == ADVENTURE_GARAGE)
                allocSize = 0x800;
        }
    }

    func_8003E978();
    func_8003105C(&gGT->JitPools[0], allocSize * 3 >> 7, 0x48, D_80011398);
    func_8003105C(&gGT->JitPools[1], allocSize >> 5, 0x294 - (4 - D_8008D2AC->numPlyrCurrGame) * 0x88, D_800113A4);
    func_8003105C(&gGT->JitPools[2], sizeFactor * 0x19 >> 10, 0x48, D_800113B4);
    func_8003105C(&gGT->JitPools[3], sizeFactor >> 7, 0x88, D_800113C4);

    if ((gGT->gameMode1 & MAIN_MENU) == 0)
        maxItems = sizeFactor >> 9;
    else
        maxItems = 4;
    func_8003105C(&gGT->JitPools[4], maxItems, 0x670, D_800113D4);

    func_8003105C(&gGT->JitPools[5], sizeFactor >> 5, 0x7c, D_800113E4);
    func_8003105C(&gGT->JitPools[6], sizeFactor >> 5, 0x18, D_800113F4);
    func_8003105C(&gGT->JitPools[7], sizeFactor >> 9, 0x28, D_80011404);

    D_8008D2AC->ptrRenderBucketInstance = func_8003E874(allocSize, D_80011410);

    for (item = gGT->JitPools[2].freeFirst; item != 0; item = *(void **)item)
        *(void **)((char *)item + 8) = (char *)item + 8;
    for (item = gGT->JitPools[3].freeFirst; item != 0; item = *(void **)item)
        *(void **)((char *)item + 8) = (char *)item + 8;
    for (item = gGT->JitPools[4].freeFirst; item != 0; item = *(void **)item)
        *(void **)((char *)item + 8) = (char *)item + 8;

    for (i = 0; i < D_8008D2AC->numPlyrCurrGame; i++)
        D_80084228[i] = func_8003E874(func_80034960(D_8008D2AC->levelID, D_8008D2AC->numPlyrCurrGame) << 2, D_80011428);
}
