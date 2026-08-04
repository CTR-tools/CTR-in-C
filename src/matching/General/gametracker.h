#ifndef GAMETRACKER_H
#define GAMETRACKER_H
/* struct GameTracker — 0x2584 (926 / NTSC-U). Real-named for the fields matching code
 * touches; padded elsewhere. Extend by replacing padding with real fields as needed.
 * Includes driver.h for the primitive typedefs and struct Driver (stored by pointer below). */
#include "driver.h"

struct Level;
struct Icon;

/* Primitive scratch buffer (0x1C) — the sub-struct embedded in PushBuffer @ 0x74 and also
 * passed by pointer to the UI prim drawers. curr @ 0xC (=0x80 in PushBuffer). */
struct PrimMem
{
    int size;         /* 0x00 */
    void *start;      /* 0x04 */
    void *end;        /* 0x08 */
    void *curr;       /* 0x0C */
    void *endMin100;  /* 0x10 */
    int unk1;         /* 0x14 */
    int unk2;         /* 0x18 */
};

/* Render view / push-buffer (stride 0x110). gGT->backBuffer points at one of these; the
 * per-player pushBuffer[] views are embedded in GameTracker @ 0x190. Named: matrix_ViewProj
 * (loaded into the GTE via SetRot/TransMatrix), primMem @ 0x74 (curr @ 0x80, endMin100 @ 0x84),
 * otMem.startPlusFour @ 0xA0, ptrOT @ 0xCC (OT head for AddPrim linking). */
struct PushBuffer
{
    short pos[3];                          /* 0x00 (screen pos) */
    char _pad_0006[0x18 - 0x06];
    int distanceToScreen_PREV;             /* 0x18 */
    unsigned short rectX, rectY;           /* 0x1C (viewport rect x,y — lhu) */
    unsigned short rectW, rectH;           /* 0x20 (viewport rect w,h — lhu) */
    char _pad_0024[0x28 - 0x24];
    MATRIX matrix_ViewProj;                /* 0x28 */
    char _pad_0048[0x74 - 0x48];
    struct PrimMem primMem;                /* 0x74 (curr @ 0x80, endMin100 @ 0x84) */
    char _pad_0090[0xA0 - 0x90];
    void *otMem_startPlusFour;             /* 0xA0 - (otMem).startPlusFour */
    char _pad_00A4[0xF4 - 0xA4];
    unsigned long *ptrOT;                  /* 0xF4 */
    char _pad_00F8[0x110 - 0xF8];
};

struct GameTracker
{
    unsigned int gameMode1;                /* 0x00 (& BATTLE_MODE 0x20) */
    char _pad_0004[0x8 - 0x4];
    unsigned int gameMode2;                /* 0x08 (& CUP_ANY_KIND 0x10, TOKEN_RACE 0x8) */
    char _pad_000C[0x10 - 0xC];
    struct PushBuffer *backBuffer;         /* 0x10 */
    char _pad_0014[0x160 - 0x14];
    struct Level *level1;                  /* 0x160 */
    char _pad_0164[0x168 - 0x164];
    struct PushBuffer pushBuffer[4];       /* 0x168 (per-player render views, stride 0x110;
                                              rect@0x184, matrix@0x190, ptrOT@0x25C for [0]) */
    char _pad_05A8[0x1388 - 0x5A8];
    struct PushBuffer pushBuffer_UI;       /* 0x1388 (UI/menu view; matrix@0x13B0, ptrOT@0x147C) */
    char _pad_1498[0x1526 - 0x1498];
    short cameraDC0_nTransitionFrame;      /* 0x1526 (cameraDC[0].nTransitionFrame, fly-in slide) */
    char _pad_1528[0x1A10 - 0x1528];
    int levelID;                           /* 0x1A10 */
    char _pad_1A14[0x1B04 - 0x1A14];
    short confetti_numParticles_max;       /* 0x1B04 (confetti.numParticles_max) */
    short confetti_nParticleRampStep;      /* 0x1B06 (confetti.nParticleRampStep) */
    char _pad_1B08[0x1B2C - 0x1B08];
    struct Thread *threadBucket0Thread;    /* 0x1B2C (threadBuckets[0].thread; stride 0x14) */
    char _pad_1B30[0x1B40 - 0x1B30];
    struct Thread *threadBucket1Thread;    /* 0x1B40 (threadBuckets[1].thread) */
    char _pad_1B44[0x1B54 - 0x1B44];
    struct Thread *threadBucket2Thread;    /* 0x1B54 (threadBuckets[2].thread — ghosts) */
    char _pad_1B58[0x1BA4 - 0x1B58];
    struct Thread *threadBucket6Thread;    /* 0x1BA4 (threadBuckets[6].thread — tracking/warpball bucket) */
    char _pad_1BA8[0x1BE0 - 0x1BA8];
    struct Thread *threadBucket9Thread;    /* 0x1BE0 (threadBuckets[9].thread — turbo-pad) */
    char _pad_1BE4[0x1CA8 - 0x1BE4];
    unsigned char numPlyrCurrGame;         /* 0x1CA8 */
    char _pad_1CA9[0x1CAB - 0x1CA9];
    unsigned char numBotsNextGame;         /* 0x1CAB */
    char _pad_1CAC[0x1CEC - 0x1CAC];
    unsigned int timer;                    /* 0x1CEC (frame timer; &1 flicker) */
    char _pad_1CF0[0x1D10 - 0x1CF0];
    int elapsedEventTime;                  /* 0x1D10 */
    char _pad_1D14[0x1D31 - 0x1D14];
    unsigned char hudFlags;                /* 0x1D31 (bit0 = HUD visible) */
    char _pad_1D32[0x1D33 - 0x1D32];
    char numLaps;                          /* 0x1D33 */
    char _pad_1D34[0x1D36 - 0x1D34];
    short timerEndOfRaceVS;                /* 0x1D36 (battle end-of-race press-X cooldown) */
    char _pad_1D38[0x1D44 - 0x1D38];
    unsigned int gameModeEnd;              /* 0x1D44 (bit0 PLAYER_GHOST_BEAT) */
    char _pad_1D48[0x1D4C - 0x1D48];
    short TitleOSK_CursorPosition;         /* 0x1D4C */
    char _pad_1D4E[0x1D54 - 0x1D4E];
    char prevNameEntered[0x11];            /* 0x1D54 */
    char currNameEntered[0x11];            /* 0x1D65 */
    char _pad_1D76[0x1D84 - 0x1D76];
    int originalEventTime;                 /* 0x1D84 */
    char _pad_1D88[0x1D90 - 0x1D88];
    int pointsPerTeam[4];                  /* 0x1D90 (battleSetup) */
    char _pad_1DA0[0x1DA8 - 0x1DA0];
    int finishedRankOfEachTeam[4];         /* 0x1DA8 (battleSetup) */
    char _pad_1DB8[0x1E20 - 0x1DB8];
    int frozenTimeRemaining;               /* 0x1E20 (TT/relic clock freeze countdown) */
    int timeCrateTypeSmashed;              /* 0x1E24 (relic time-crate value smashed; sprintf arg) */
    int numCrystalsInLEV;                  /* 0x1E28 (crystals needed to win Crystal Challenge) */
    char _pad_1E2C[0x1E58 - 0x1E2C];
    int cup_cupID;                         /* 0x1E58 (cup.cupID) */
    int cup_trackIndex;                    /* 0x1E5C (cup.trackIndex) */
    int cup_points[8];                     /* 0x1E60 (cup.points[driverID]) */
    char _pad_1E80[0x1EB4 - 0x1E80];
    int prevLEV;                           /* 0x1EB4 (previous LEV id; read as short) */
    int bossID;                            /* 0x1EB8 */
    int arcadeDifficulty;                  /* 0x1EBC (0x50->diff0 / 0xa0->diff1 / else diff2) */
    char _pad_1EC0[0x1EEC - 0x1EC0];
    struct Icon *ptrIcons[0x33];           /* 0x1EEC (HUD weapon/item icon pointer table) */
    char _pad_1FB8[0x2114 - 0x1FB8];
    struct IconGroup *iconGroup[0xc];      /* 0x2114 (iconGroup[0xb]->icons[0] = wumpa popup icon) */
    char _pad_2144[0x2160 - 0x2144];
    struct Model *modelPtr[227];           /* 0x2160 .. 0x24EC (static-model table) */
    struct Driver *drivers[8];             /* 0x24EC */
    struct Driver *driversInRaceOrder[8];  /* 0x250C */
    char _pad_252C[0x256C - 0x252C];
    int renderFlags;                       /* 0x256C (|=0x8000 to render this frame) */
    char _pad_2570[0x2572 - 0x2570];
    short podiumRewardID;                  /* 0x2572 (STATIC_GEM 0x5f / STATIC_BIG1 0x38) */
    char _pad_2574[0x2575 - 0x2574];
    unsigned char podium_modelIndex_First; /* 0x2575 */
    unsigned char podium_modelIndex_Second;/* 0x2576 */
    unsigned char podium_modelIndex_Third; /* 0x2577 */
    unsigned char podium_modelIndex_tawna; /* 0x2578 */
    char _pad_2579[0x2584 - 0x2579];
};

#endif /* GAMETRACKER_H */
