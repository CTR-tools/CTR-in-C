#ifndef UITYPES_H
#define UITYPES_H
/* UI-namespace real types (926 / NTSC-U). Includes gametracker.h (-> driver.h) for
 * GameTracker / Driver / MATRIX. Holds struct Instance and other UI-shared structs.
 * Grow with real fields as UI functions need them. */
#include "gametracker.h"

struct Model;
struct InstDef;
struct Thread;
struct SpawnType1;

/* DecalMP off-screen render target (g_aGpuDmaQueue[2].pInlineData[0xf]); partial — the wumpa/HUD
 * sub-renderer fields. xOff/yOff = center offset; prim/primEnd = the rendered primitive span. */
struct DecalMP
{
    char _pad_0000[0x1c];
    short xOff;                                     /* 0x1c */
    short yOff;                                     /* 0x1e */
    char _pad_0020[0xf4 - 0x20];
    unsigned int *prim;                             /* 0xf4 */
    unsigned int *primEnd;                          /* 0xf8 */
};

/* Per-lap time digits (g_lapTimes) — 5 char[0x38] planes, each indexed [driverID*7 + lap]. */
struct LapTimes
{
    char pLapDigit_Min1s[0x38];                     /* 0x00 */
    char pLapDigit_Sec10s[0x38];                    /* 0x38 */
    char pLapDigit_Sec1s[0x38];                     /* 0x70 */
    char pLapDigit_Ms10s[0x38];                     /* 0xA8 */
    char pLapDigit_Ms1s[0x38];                      /* 0xE0 */
};

/* VS end-of-race "quip" rule engine. VsQuipLine = candidate comment line (8B, nWeight@4).
 * VsQuipRule (0x18) = one rule: candidate line span + ruleType/flags/threshold/statIndex1/2. */
struct VsQuipLine { char _pad_0000[4]; int nWeight; };
struct VsQuipRule
{
    struct VsQuipLine *pLinesStart;                 /* 0x00 */
    struct VsQuipLine *pLinesEnd;                   /* 0x04 */
    short ruleType;                                 /* 0x08 */
    short nFlags;                                   /* 0x0A (&1 scale-by-laps, &0xC assign-mid-loop) */
    int threshold;                                  /* 0x0C */
    int statIndex1;                                 /* 0x10 */
    int statIndex2;                                 /* 0x14 */
};

/* RECTMENU row (0x6 stride); only stringIndex named. */
struct MenuRow
{
    short stringIndex;                              /* 0x0 */
    char _pad_02[0x6 - 0x2];
};

/* RECTMENU box (partial — fields the menu callbacks touch). */
struct RectMenu
{
    char _pad_0000[0xC];
    struct MenuRow *rows;                           /* 0x0C */
    void (*funcPtr)(struct RectMenu *);             /* 0x10 */
    unsigned short drawStyle;                       /* 0x14 (bit8 0x100, bit-mask 0xFEFF) */
    char _pad_0016[0x1A - 0x16];
    short rowSelected;                              /* 0x1A */
    short unk1c;                                    /* 0x1C */
    short unk1e;                                    /* 0x1E (proc call phase; 0 = commit) */
    short width, height;                            /* 0x20 */
};

/* PSX libgpu POLY_FT4 — flat textured quad (0x28 bytes). */
typedef struct {
    unsigned long tag;                              /* 0x00 */
    unsigned char r0, g0, b0, code;                 /* 0x04 */
    short x0, y0;                                   /* 0x08 */
    unsigned char u0, v0; unsigned short clut;      /* 0x0C */
    short x1, y1;                                   /* 0x10 */
    unsigned char u1, v1; unsigned short tpage;     /* 0x14 */
    short x2, y2;                                   /* 0x18 */
    unsigned char u2, v2; unsigned short pad1;      /* 0x1C */
    short x3, y3;                                   /* 0x20 */
    unsigned char u3, v3; unsigned short pad2;      /* 0x24 */
} POLY_FT4;

/* PSX libgpu POLY_GT4 — gouraud textured quad (0x34 bytes). */
typedef struct {
    unsigned long tag;                              /* 0x00 */
    unsigned char r0, g0, b0, code;                 /* 0x04 */
    short x0, y0;                                   /* 0x08 */
    unsigned char u0, v0; unsigned short clut;      /* 0x0C */
    unsigned char r1, g1, b1, p1;                   /* 0x10 */
    short x1, y1;                                   /* 0x14 */
    unsigned char u1, v1; unsigned short tpage;     /* 0x18 */
    unsigned char r2, g2, b2, p2;                   /* 0x1C */
    short x2, y2;                                   /* 0x20 */
    unsigned char u2, v2; unsigned short pad2;      /* 0x24 */
    unsigned char r3, g3, b3, p3;                   /* 0x28 */
    short x3, y3;                                   /* 0x2C */
    unsigned char u3, v3; unsigned short pad3;      /* 0x30 */
} POLY_GT4;

/* Speedometer/dial vertex table entry (2 signed shorts, read via lhu). */
struct Point { unsigned short nX, nY; };

/* g_anMetaDataCharacters[] entry (0x10 stride); name_LNG_long @ 0x4, iconID @ 0x8 (read lh -> ptrIcons index). */
struct MetaDataCharacter { char _pad_00[4]; short name_LNG_long; short _pad_06; short iconID; char _pad_0A[0x10 - 0xA]; };

/* data_metaDataLEV[] entry (0x18 stride); only name_LNG @ 0x8 named. */
struct MetaDataLEV { char _pad_00[8]; short name_LNG; char _pad_0A[0x18 - 0xA]; };

/* sdata_gameProgress — persistent unlock bit array (.unlocks @ 0x4, indexed bit>>5). */
struct GameProgress { char _pad_00[4]; unsigned int unlocks[16]; };

/* HUD icon group (gGT->iconGroup[]); icons[] table of Icon* @ 0x14. */
struct IconGroup { char _pad_00[0x14]; struct Icon *icons[8]; };

/* sdata_gGamepads element (stride 0x50); buttonsTapped/held @ 0x14. */
struct Gamepad { char _pad_00[0x14]; unsigned int buttonsTapped; char _pad_18[0x50 - 0x18]; };

/* CTR box RECT (matches General/CTR: u_short x,y,w,h). */
struct RECT { unsigned short x, y, w, h; };

/* PSX libgpu POLY_F4 — flat quad (0x18 bytes / 6 words). */
typedef struct {
    unsigned long tag;                              /* 0x00 */
    unsigned char r0, g0, b0, code;                 /* 0x04 */
    short x0, y0;                                   /* 0x08 */
    short x1, y1;                                   /* 0x0C */
    short x2, y2;                                   /* 0x10 */
    short x3, y3;                                   /* 0x14 */
} POLY_F4;

/* PSX libgpu POLY_G4 — gouraud quad (0x24 bytes / 9 words). */
typedef struct {
    unsigned long tag;                              /* 0x00 */
    unsigned char r0, g0, b0, code;                 /* 0x04 */
    short x0, y0;                                   /* 0x08 */
    unsigned char r1, g1, b1, p1;                   /* 0x0C */
    short x1, y1;                                   /* 0x10 */
    unsigned char r2, g2, b2, p2;                   /* 0x14 */
    short x2, y2;                                   /* 0x18 */
    unsigned char r3, g3, b3, p3;                   /* 0x1C */
    short x3, y3;                                   /* 0x20 */
} POLY_G4;

/* PSX libgpu POLY_G3 — gouraud triangle (0x1C bytes). */
typedef struct {
    unsigned long tag;                              /* 0x00 */
    unsigned char r0, g0, b0, code;                 /* 0x04 */
    short x0, y0;                                   /* 0x08 */
    unsigned char r1, g1, b1, p1;                   /* 0x0C */
    short x1, y1;                                   /* 0x10 */
    unsigned char r2, g2, b2, p2;                   /* 0x14 */
    short x2, y2;                                   /* 0x18 */
} POLY_G3;

/* Icon UV/tpage descriptor (0xC bytes). */
struct TextureLayout
{
    unsigned char u0, v0; unsigned short clut;      /* 0x0 */
    unsigned char u1, v1; unsigned short tpage;     /* 0x4 */
    unsigned char u2, v2;                           /* 0x8 */
    unsigned char u3, v3;                           /* 0xA */
};

struct Icon
{
    char name[0x10];                                /* 0x00 */
    int global_IconArray_Index;                     /* 0x10 */
    struct TextureLayout texLayout;                 /* 0x14 */
};

/* struct PrimMem now lives in gametracker.h (embedded in PushBuffer @ 0x74). */

/* Level ST1 spawn table header (count then void* pointers[]). */
struct SpawnType1 { int count; };

/* struct Level — only ptrSpawnType1 named so far. */
struct Level
{
    char _pad_0000[0x134];
    struct SpawnType1 *ptrSpawnType1;               /* 0x134 (ST1 table; [1] = ST1_MAP ptr) */
    char _pad_0138[0x148 - 0x138];
    int cnt_restart_points;                         /* 0x148 */
    struct CheckpointNode *ptr_restart_points;      /* 0x14C */
};

/* 2D HUD layout element (data.hudStructPtr entries). 8 bytes. */
struct UiElement2D
{
    short x;      /* 0x0 */
    short y;      /* 0x2 */
    short z;      /* 0x4 */
    short scale;  /* 0x6 */
};

/* 3D HUD element (a HUD thread's object). 0x38 bytes. */
struct UiElement3D
{
    short rot[4];       /* 0x00 */
    MATRIX m;           /* 0x08 */
    short lightDir[4];  /* 0x28 */
    short vel[4];       /* 0x30 */
};

/* struct Model (from decomp) — only the id is needed so far. */
struct Model
{
    char name[0x10];  /* 0x00 */
    short id;         /* 0x10 (see MODEL_ID; DYNAMIC_WARPBALL = 0x36) */
};

/* Track restart/checkpoint node (stride 0xc): pos@0, distToFinish@6, nextIndex_forward@8. */
struct CheckpointNode
{
    short pos[3];                                   /* 0x00 */
    unsigned short distToFinish;                    /* 0x06 */
    unsigned char nextIndex_forward;                /* 0x08 */
    char _pad_0009[0xc - 9];
};

/* Weapon/tracker thread payload (thread->object). driverTarget @ 0x0. */
struct TrackerWeapon
{
    struct Driver *driverTarget;  /* 0x00 */
    char _pad_0004[0x16 - 0x04];
    unsigned short flags;         /* 0x16 (&0x10 = ...) */
    char _pad_0018[0x28 - 0x18];
    unsigned int distanceToTarget;/* 0x28 */
    char _pad_002C[0x3c - 0x2c];
    struct CheckpointNode *ptrNodeCurr; /* 0x3c */
};

/* struct Thread (from decomp) — named fields UI touches; grow as needed. */
struct Thread
{
    char _pad_0000[0x10];
    struct Thread *siblingThread; /* 0x10 */
    char _pad_0014[0x30 - 0x14];
    void *object;                 /* 0x30 (e.g. TrackerWeapon*) */
    struct Instance *inst;        /* 0x34 (instcDC.inst) */
};

/* struct Instance — renderable instance node (from decomp). Named through the rendering
 * matrix; matrix.t[2] (Z) @ 0x4C. Grow past 0x50 as functions need it. */
struct Instance
{
    struct Instance *next;      /* 0x00 */
    struct Instance *prev;      /* 0x04 */
    char name[0x10];            /* 0x08 */
    struct Model *model;        /* 0x18 */
    short scale[3];             /* 0x1C */
    short alphaScale;           /* 0x22 */
    unsigned int colorRGBA;     /* 0x24 */
    unsigned int flags;         /* 0x28 */
    struct InstDef *instDef;    /* 0x2C */
    MATRIX matrix;              /* 0x30 (t[0..2] @ 0x44/0x48/0x4C) */
    char unk50;                 /* 0x50 (cOtDepthNear) */
    char unk51;                 /* 0x51 (cOtDepthFar) */
    char _pad_0052[0x6C - 0x52];
    struct Thread *thread;      /* 0x6C */
};

#endif /* UITYPES_H */
