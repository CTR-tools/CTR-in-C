/*
 * UI_DrawSpeedBG @ 0x800516AC (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg37, score 3065 — residual = color if-nest + pass-3 tail
 * scheduling). Real types (uitypes.h Point / POLY_G4 / POLY_G3).
 *
 * Draws the speedometer dial background into gGT->pushBuffer_UI.ptrOT, from two Point[14] tables:
 * D_8008646C (radial verts) + D_800864A4 (outer tick-line verts), all offset by (xOffset=0x1E0=480,
 * yOffset=0xBE=190). Three passes:
 *  (1) 7 horizontal tick lines: white (0xff) wire + black (0) shadow offset (+1,+1) via
 *      CTR_Box_DrawWirePrims.
 *  (2) 6 radial segments: white + black wire verticals (pt0-pt2, pt1-pt3) then a gouraud PolyG4
 *      quad (9 words, GPU code 0x38) — green->yellow->red gradient (bottom/top vertex colors step
 *      along the arc: GREEN=0xb500, YELLOW=0xd1ff, RED=0xdb).
 *  (3) 6 semi-transparent PolyG3 triangles (DR_TPAGE 0xE1000A00 + setPolyG3 code 0x32, black) fanned
 *      to boundary verts [0xd].x (p2x) and [1].y (p2y) for the dark dial face.
 *
 * NOTE(claude) vs in-repo decomp (REFERENCE; 926 asm authoritative):
 *   1) The 926 uses TWO separate Point[14] tables (D_8008646C radial, D_800864A4 tick); the decomp
 *      models one table with [0]/[1] offset indexing.
 *   2) The quad color gradient is computed INLINE by an iSegIdx if-nest, NOT a Colors[] table lookup;
 *      the produced sequence is {G/G, G/G, G/Y, Y/R, R/R, R/R} (== table {G,G,G,Y,R,R,R}), which is
 *      one step earlier than the decomp's non-online DrawSpeedBG_Colors[7]={G,G,G,G,Y,R,R}.
 *   3) p2x/p2y (verts [0xd].x, [1].y) are RE-READ each pass-3 iteration (the decomp hoists them).
 * Point.nX/nY are read via lhu (unsigned short) but the +offset and 16-bit store wrap correctly for
 * the negative table coordinates. Phase B (saphi): see note below.
 */
#include "../uitypes.h"

extern struct GameTracker *D_8008D2AC;      /* gGT (absolute) */
extern struct Point D_8008646C[];           /* g_aSpeedometerBG_VertData  (radial verts) */
extern struct Point D_800864A4[];           /* g_aSpeedometerBG_TickVertData (tick-line verts) */
extern void func_80021500(short x0, short y0, short x1, short y1, int r, int g, int b,
                          void *ot, struct PrimMem *pm);   /* CTR_Box_DrawWirePrims */

typedef struct { unsigned int tag; unsigned int tpageCode; POLY_G3 g3; } TPage_PolyG3;

void UI_DrawSpeedBG(void)
{
    int i;
    POLY_G4 *pQuad;
    TPage_PolyG3 *pTri;
    void *pPrimCurr;
    struct PushBuffer *pBackBuf;
    unsigned int colorBottom, colorTop;
    unsigned int *ot;

    /* Pass 1 - horizontal tick lines (white + black shadow). */
    for (i = 0; i < 14; i += 2) {
        func_80021500(D_800864A4[i].nX + 0x1e0, D_800864A4[i].nY + 0xbe,
                      D_800864A4[i + 1].nX + 0x1e0, D_800864A4[i + 1].nY + 0xbe,
                      0xff, 0xff, 0xff, D_8008D2AC->pushBuffer_UI.ptrOT,
                      &D_8008D2AC->backBuffer->primMem);
        func_80021500(D_800864A4[i].nX + 0x1e1, D_800864A4[i].nY + 0xbf,
                      D_800864A4[i + 1].nX + 0x1e1, D_800864A4[i + 1].nY + 0xbf,
                      0, 0, 0, D_8008D2AC->pushBuffer_UI.ptrOT,
                      &D_8008D2AC->backBuffer->primMem);
    }

    /* Pass 2 - radial segments (white + black verticals) + gouraud PolyG4. */
    for (i = 0; i < 12; i += 2) {
        func_80021500(D_8008646C[i].nX + 0x1e0, D_8008646C[i].nY + 0xbe,
                      D_8008646C[i + 2].nX + 0x1e0, D_8008646C[i + 2].nY + 0xbe,
                      0xff, 0xff, 0xff, D_8008D2AC->pushBuffer_UI.ptrOT,
                      &D_8008D2AC->backBuffer->primMem);
        func_80021500(D_8008646C[i + 1].nX + 0x1e0, D_8008646C[i + 1].nY + 0xbe,
                      D_8008646C[i + 3].nX + 0x1e0, D_8008646C[i + 3].nY + 0xbe,
                      0xff, 0xff, 0xff, D_8008D2AC->pushBuffer_UI.ptrOT,
                      &D_8008D2AC->backBuffer->primMem);
        func_80021500(D_8008646C[i].nX + 0x1e1, D_8008646C[i].nY + 0xbf,
                      D_8008646C[i + 2].nX + 0x1e1, D_8008646C[i + 2].nY + 0xbf,
                      0, 0, 0, D_8008D2AC->pushBuffer_UI.ptrOT,
                      &D_8008D2AC->backBuffer->primMem);
        func_80021500(D_8008646C[i + 1].nX + 0x1e1, D_8008646C[i + 1].nY + 0xbf,
                      D_8008646C[i + 3].nX + 0x1e1, D_8008646C[i + 3].nY + 0xbf,
                      0, 0, 0, D_8008D2AC->pushBuffer_UI.ptrOT,
                      &D_8008D2AC->backBuffer->primMem);

        pBackBuf = D_8008D2AC->backBuffer;
        pPrimCurr = pBackBuf->primMem.curr;
        pQuad = 0;
        if ((unsigned int)pPrimCurr <= (unsigned int)pBackBuf->primMem.endMin100) {
            pBackBuf->primMem.curr = (char *)pPrimCurr + 0x24;
            pQuad = (POLY_G4 *)pPrimCurr;
        }
        if (pQuad == 0) return;

        colorBottom = 0xb500;
        colorTop = colorBottom;
        if ((2 < i) && (colorTop = 0xd1ff, 4 < i)) {
            colorBottom = 0xd1ff;
            if (i < 7) {
                colorTop = 0xdb;
            } else {
                colorBottom = 0xdb;
                colorTop = colorBottom;
            }
        }
        *(int *)&pQuad->r0 = colorBottom | 0x38000000;
        *(int *)&pQuad->r1 = colorBottom | 0x38000000;
        *(int *)&pQuad->r2 = colorTop | 0x38000000;
        *(int *)&pQuad->r3 = colorTop | 0x38000000;
        pQuad->x0 = D_8008646C[i].nX + 0x1e0;
        pQuad->y0 = D_8008646C[i].nY + 0xbe;
        pQuad->x1 = D_8008646C[i + 1].nX + 0x1e0;
        pQuad->y1 = D_8008646C[i + 1].nY + 0xbe;
        pQuad->x2 = D_8008646C[i + 2].nX + 0x1e0;
        pQuad->y2 = D_8008646C[i + 2].nY + 0xbe;
        pQuad->x3 = D_8008646C[i + 3].nX + 0x1e0;
        pQuad->y3 = D_8008646C[i + 3].nY + 0xbe;
        ot = D_8008D2AC->pushBuffer_UI.ptrOT;
        *(int *)pQuad = *ot | 0x8000000;
        *ot = (unsigned int)pQuad & 0xffffff;
    }

    /* Pass 3 - semi-transparent dark backing triangles (TPage + PolyG3). */
    for (i = 0; i < 12; i += 2) {
        pBackBuf = D_8008D2AC->backBuffer;
        pPrimCurr = pBackBuf->primMem.curr;
        pTri = 0;
        if ((unsigned int)pPrimCurr <= (unsigned int)pBackBuf->primMem.endMin100) {
            pBackBuf->primMem.curr = (char *)pPrimCurr + 0x24;
            pTri = (TPage_PolyG3 *)pPrimCurr;
        }
        if (pTri == 0) return;

        pTri->g3.x0 = D_8008646C[i + 1].nX + 0x1e0;
        pTri->g3.y0 = D_8008646C[i + 1].nY + 0xbe;
        pTri->g3.x1 = D_8008646C[i + 3].nX + 0x1e0;
        pTri->g3.y1 = D_8008646C[i + 3].nY + 0xbe;
        pTri->g3.x2 = D_8008646C[0xd].nX + 0x1e0;
        ((char *)pTri)[3] = 8;
        pTri->g3.code = 0x32;
        pTri->g3.r1 = 0;
        pTri->g3.g1 = 0;
        pTri->g3.b1 = 0;
        pTri->g3.r0 = 0;
        pTri->g3.g0 = 0;
        pTri->g3.b0 = 0;
        pTri->g3.r2 = 0;
        pTri->g3.g2 = 0;
        pTri->g3.b2 = 0;
        pTri->tpageCode = 0xe1000a00;
        pTri->g3.tag = 0;
        pTri->g3.y2 = D_8008646C[1].nY + 0xbe;
        ot = D_8008D2AC->pushBuffer_UI.ptrOT;
        *(int *)pTri = *ot | 0x8000000;
        *ot = (unsigned int)pTri & 0xffffff;
    }
}
