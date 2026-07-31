// LOAD_Hub_SwapNow @ 0x80033108  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Finalizes an adventure-hub double-buffer swap: stall until level2 loaded,
// RePack, HubSwapPtrs, flip activeMempackIndex (3-idx), prevLEV/levelID update +
// swap song, LibraryOfModels_Clear, optional GlobalModelPtrs_MPK, then per-level
// re-init (Store/LevInitAll/UnPack/DecalGlobal), SwapPacks, MainInit_VisMem, and
// clear cameraDC[0]/visMem1/driver/frame counters.
// Match levers: access gGT via D_8008D2AC directly (retail reloads per block, CSE
// within call-free stretches — do NOT cache gGT/level1/visMem in locals);
// activeMempackIndex is `short` (retail lh, signed); cameraDC store order is
// ptrQuadBlock,visInstSrc,visLeafSrc,visFaceSrc,visOVertSrc,visSCVertSrc.
// NOTE: retail passes a dead a1=1 to LOAD_HubSwapPtrs (callee ignores it).
#include "CTR.h"

struct Level {
    void *ptr_mesh_info;       /* 0x0  */
    char pad0[0x8];
    int numInstances;          /* 0xC  */
    void *ptrInstDefs;         /* 0x10 */
    int numModels;             /* 0x14 */
    void *ptrModelsPtrArray;   /* 0x18 */
    char pad1[0x3C - 0x1C];
    void *levTexLookup;        /* 0x3C */
};

struct CameraDC {
    void *ptrQuadBlock;   /* 0x0  */
    void *visLeafSrc;     /* 0x4  */
    void *visFaceSrc;     /* 0x8  */
    void *visInstSrc;     /* 0xC  */
    void *visOVertSrc;    /* 0x10 */
    void *visSCVertSrc;   /* 0x14 */
};

struct VisMem {
    char pad0[0x40];
    void *visLeafSrc[4];    /* 0x40 */
    void *visFaceSrc[4];    /* 0x50 */
    void *visOVertSrc[4];   /* 0x60 */
    void *visSCVertSrc[4];  /* 0x70 */
};

struct Driver {
    char pad0[0x350];
    int underDriver;       /* 0x350 */
};

struct GameTracker {
    char pad_160[0x160];
    struct Level *level1;              /* 0x160 */
    int level2;                        /* 0x164 */
    char pad_14B4[0x14B4 - 0x168];
    struct CameraDC cameraDC[1];       /* 0x14B4 */
    char pad_1A10[0x1A10 - (0x14B4 + 0x18)];
    int levelID;                       /* 0x1A10 */
    char pad_1A38[0x1A38 - 0x1A14];
    struct VisMem *visMem1;            /* 0x1A38 */
    char pad_1CFC[0x1CFC - 0x1A3C];
    int framesInThisLEV;               /* 0x1CFC */
    int msInThisLEV;                   /* 0x1D00 */
    char pad_1EB4[0x1EB4 - 0x1D04];
    int prevLEV;                       /* 0x1EB4 */
    char pad_24EC[0x24EC - 0x1EB8];
    struct Driver *drivers[1];         /* 0x24EC */
    char pad_254A[0x254A - 0x24F0];
    short activeMempackIndex;          /* 0x254A */
    short levID_in_each_mempack[8];    /* 0x254C */
};

extern struct GameTracker *D_8008D2AC;   /* sdata->gGT */
extern void *D_8008D870;                 /* sdata->PLYROBJECTLIST (absolute) */

extern void func_80032DC0(void);                       /* LOAD_NextQueuedFile */
extern void func_80075350(int mode);                   /* VSync */
extern void func_80031268(void *meshInfo, int one);    /* LevInstDef_RePack */
extern void func_80031BDC(void *gGT, int one);         /* LOAD_HubSwapPtrs */
extern void func_8002D50C(int levelID);                /* Audio_AdvHub_SwapSong */
extern void func_800314C0(void *gGT);                  /* LibraryOfModels_Clear */
extern void func_80031B50(void);                       /* LOAD_GlobalModelPtrs_MPK */
extern void func_8003147C(void *gGT, int numModels, void *ptrArr);   /* LibraryOfModels_Store */
extern void func_80030AD4(void *instDefs, int numInst);/* INSTANCE_LevInitAll */
extern void func_8003116C(void *meshInfo);             /* LevInstDef_UnPack */
extern void func_80022BDC(void *gGT, void *texLookup); /* DecalGlobal_Store */
extern void func_8003E80C(int packID);                 /* MEMPACK_SwapPacks */
extern void func_8003AF84(void *gGT);                  /* MainInit_VisMem */

void LOAD_Hub_SwapNow(void)
{
    while (D_8008D2AC->level2 == 0)
    {
        func_80032DC0();
        func_80075350(0);
    }

    func_80031268(D_8008D2AC->level1->ptr_mesh_info, 1);
    func_80031BDC(D_8008D2AC, 1);

    D_8008D2AC->activeMempackIndex = 3 - D_8008D2AC->activeMempackIndex;
    D_8008D2AC->prevLEV = D_8008D2AC->levelID;
    D_8008D2AC->levelID = D_8008D2AC->levID_in_each_mempack[D_8008D2AC->activeMempackIndex];
    func_8002D50C(D_8008D2AC->levelID);

    func_800314C0(D_8008D2AC);

    if (D_8008D870 != 0)
        func_80031B50();

    if (D_8008D2AC->level1 != 0)
    {
        func_8003147C(D_8008D2AC, D_8008D2AC->level1->numModels, D_8008D2AC->level1->ptrModelsPtrArray);
        func_80030AD4(D_8008D2AC->level1->ptrInstDefs, D_8008D2AC->level1->numInstances);
        func_8003116C(D_8008D2AC->level1->ptr_mesh_info);
        func_80022BDC(D_8008D2AC, D_8008D2AC->level1->levTexLookup);
    }

    func_8003E80C(D_8008D2AC->activeMempackIndex);
    func_8003AF84(D_8008D2AC);

    D_8008D2AC->cameraDC[0].ptrQuadBlock = 0;
    D_8008D2AC->cameraDC[0].visInstSrc = 0;
    D_8008D2AC->cameraDC[0].visLeafSrc = 0;
    D_8008D2AC->cameraDC[0].visFaceSrc = 0;
    D_8008D2AC->cameraDC[0].visOVertSrc = 0;
    D_8008D2AC->cameraDC[0].visSCVertSrc = 0;

    D_8008D2AC->visMem1->visLeafSrc[0] = 0;
    D_8008D2AC->visMem1->visFaceSrc[0] = 0;
    D_8008D2AC->visMem1->visOVertSrc[0] = 0;
    D_8008D2AC->visMem1->visSCVertSrc[0] = 0;

    D_8008D2AC->drivers[0]->underDriver = 0;

    D_8008D2AC->framesInThisLEV = 0;
    D_8008D2AC->msInThisLEV = 0;
}
