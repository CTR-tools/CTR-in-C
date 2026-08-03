#ifndef GAMETRACKER_H
#define GAMETRACKER_H
/* struct GameTracker — 0x2584 (926 / NTSC-U). Real-named for the fields matching code
 * touches; padded elsewhere. Extend by replacing padding with real fields as needed.
 * Includes driver.h for the primitive typedefs and struct Driver (stored by pointer below). */
#include "driver.h"

/* Back/draw push-buffer. Only the OT-memory cursor (backBuffer->otMem.startPlusFour @ 0xA0),
 * used by the RECTMENU rect drawers, is named; everything else is padding for now. */
struct PushBuffer
{
    char _pad_0000[0xA0];
    void *otMem_startPlusFour;             /* 0xA0 - (otMem).startPlusFour */
};

struct GameTracker
{
    char _pad_0000[0x10];
    struct PushBuffer *backBuffer;         /* 0x10 */
    char _pad_0014[0x1D4C - 0x14];
    short TitleOSK_CursorPosition;         /* 0x1D4C */
    char _pad_1D4E[0x1D54 - 0x1D4E];
    char prevNameEntered[0x11];            /* 0x1D54 */
    char currNameEntered[0x11];            /* 0x1D65 */
    char _pad_1D76[0x24EC - 0x1D76];
    struct Driver *drivers[8];             /* 0x24EC */
    struct Driver *driversInRaceOrder[8];  /* 0x250C */
    char _pad_252C[0x2575 - 0x252C];
    unsigned char podium_modelIndex_First; /* 0x2575 */
    unsigned char podium_modelIndex_Second;/* 0x2576 */
    unsigned char podium_modelIndex_Third; /* 0x2577 */
    unsigned char podium_modelIndex_tawna; /* 0x2578 */
    char _pad_2579[0x2584 - 0x2579];
};

#endif /* GAMETRACKER_H */
