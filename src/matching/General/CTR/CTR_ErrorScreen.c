// CTR_ErrorScreen @ 0x80021c94  (vs SCUS_944.26.exe)
//
// SCORE 0 exact match.
//
// Draws a full-screen solid-color TILE (the fatal-error flash) three times,
// swapping the display buffer each pass. Key matching facts recovered here:
//   * The retail loop is PEELED: the last (i==2) pass only does the
//     DrawSync/VSync/DISPLAY_Swap and returns, so gcc emits a do/while(i<2)
//     with a trailing sync sequence after the loop (NOT a for + in-loop
//     `if(i==2)return`, which does not peel under gcc-2.8.1 and scored ~2570).
//   * The DMA tag is built in two steps: addr via `tag.word |= 0xffffff`
//     (lui/ori 0xffffff + lw/or/sw) and len via a clean byte overwrite
//     `tag.b.len = 3` (sb 3 -> byte 3). A single `p.tag = 0x03ffffff`
//     literal collapses to one word store and cannot match (it would also
//     OR-in a garbage-derived len instead of overwriting it).
//   * Clip rect is read w,h,x,y (source order), sdata cached once, the clip
//     pointer (sdata+0x14) reloaded per field.
//
// D_8008D2AC = sdata; sdata+0x14 -> front-buffer clip rect {x@0,y@2,w@4,h@6}.
// func_80072FF0=DrawSync, func_80075350=VSync, func_80023FFC=DISPLAY_Swap,
// func_80073398=DrawOTag.

#include "CTR.h"

struct ErrClip { s16 x, y, w, h; };                         // offsets 0,2,4,6
struct ErrSData { char pad0[0x14]; struct ErrClip *clip; }; // clip ptr @ 0x14

extern struct ErrSData *D_8008D2AC;   // sdata

extern void func_80072FF0(int);    // DrawSync
extern void func_80075350(int);    // VSync
extern void func_80023FFC(void);   // DISPLAY_Swap
extern void func_80073398(void *); // DrawOTag

typedef union {
    u_int word;                              // whole DMA tag (addr | len<<24)
    struct { u_char a0, a1, a2, len; } b;    // addr bytes 0-2, len byte 3
} ErrTag;

typedef struct {
    ErrTag tag;
    u_char r0, g0, b0, code;
    s16 x0, y0;
    s16 w, h;
} ErrTile;

void CTR_ErrorScreen(char r, char g, char b)
{
    ErrTile p;
    int i;

    i = 0;
    do
    {
        func_80072FF0(0);
        func_80075350(0);
        func_80023FFC();

        i++;

        p.code = 2;
        p.tag.word |= 0xffffff;
        p.tag.b.len = 3;

        p.r0 = r;
        p.g0 = g;
        p.b0 = b;

        p.w = D_8008D2AC->clip->w;
        p.h = D_8008D2AC->clip->h;
        p.x0 = D_8008D2AC->clip->x;
        p.y0 = D_8008D2AC->clip->y;

        func_80073398(&p);
    } while (i < 2);

    func_80072FF0(0);
    func_80075350(0);
    func_80023FFC();
}
