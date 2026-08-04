/*
 * UI_JumpMeter_Draw @ 0x80051E24 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg128, score 4460 — register cascade from the /0x3c0
 * division-magic being hoisted into a callee-saved reg, same family as UI_DrawSpeedNeedle; the
 * three POLY_F4 blocks match, the digit-setup register allocation diverges). Real types.
 *
 * Draws the jump/hang-time meter HUD widget at (posX,posY):
 *  (1) 3-glyph numeric readout of driver->jumpMeter (@0x46): seconds = jumpMeter/0x3C0,
 *      tenths = (jumpMeter%0x3C0)/0x60, hundredths, via DebugFont_DrawNumbers at y = posY-0x2B.
 *  (2) two black (memset-zeroed color) wire boxes via CTR_Box_DrawWireBox: a 0x22x10 digit frame
 *      and a 0xC x 0x26 vertical bar frame, into gGT->pushBuffer_UI.ptrOT.
 *  (3) up to three POLY_F4 flat quads (6 words, GPU code 0x28-semitrans, OT tag |0x5000000) into
 *      backBuffer->primMem: a white frame fill, the colored fill bar (height = jumpMeter*0x26/0x960,
 *      color tiered by jumpMeter: <0x280 red 0x28ff0000, <0x3C0 green 0x2800ff00, <0x5A0 yellow
 *      0x2800ffff, else blue 0x280000ff), and a gray 0x28808080 background bar.
 *
 * NOTE(claude) vs in-repo decomp (REFERENCE; 926 asm authoritative): the WireBox color is a
 * memset(&buf,0,4) BLACK buffer (func_800718BC = memset), NOT the decomp's data.colors[21][0];
 * digitBox.w = 0x22 (decomp WIDE_PICK picks 0x1D); x1/x3 of the frame fill = posX+0xe (decomp
 * WIDE_34(13)); numbersYOffset = -0x2d, barHeight = 0x26 (non-online path). USE_ONLINE/widescreen
 * branches are absent in 926. Phase B (saphi): see note below.
 */
#include "../uitypes.h"

extern struct GameTracker *D_8008D2AC;      /* gGT (absolute) */
extern void func_80022318(int value, int x, int y);                 /* DebugFont_DrawNumbers */
extern void func_80021594(struct RECT *r, unsigned char *color,
                          void *ot, struct PrimMem *pm);            /* CTR_Box_DrawWireBox */
extern void *func_800718BC(void *dst, int c, int n);               /* memset */

void UI_JumpMeter_Draw(short posX, short posY, struct Driver *driver)
{
    int index, index_00, iRemainder, iPosX, screenPosY;
    short sBoxX, sBoxY, sBarRight, jmColor, jmHeight;
    unsigned int uBarColor;
    int jumpMeterHeight;
    struct PushBuffer *pBackBuf;
    void *pPrimCurr;
    POLY_F4 *p;
    unsigned int *ot;
    struct RECT digitBox;
    unsigned char color1[4];
    struct RECT barBox;
    unsigned char color2[4];

    index = ((int)driver->jumpMeter / 0x3c0) * 0x10000 >> 0x10;
    iRemainder = (int)driver->jumpMeter + index * -0x3c0;
    index_00 = (iRemainder / 0x60) * 0x10000 >> 0x10;
    iPosX = (int)posX;
    screenPosY = posY + -0x2b;
    func_80022318(index, iPosX + -0x10, screenPosY);
    func_80022318(index_00, iPosX + -4, screenPosY);
    func_80022318((((iRemainder + index_00 * -0x60) * 100) / 0x3c0) * 0x10000 >> 0x10,
                  iPosX + 4, screenPosY);

    sBoxX = posX + -0x14;
    sBoxY = posY + -0x2d;
    digitBox.w = 0x22;
    digitBox.h = 10;
    digitBox.x = sBoxX;
    digitBox.y = sBoxY;
    func_800718BC(color1, 0, 4);
    func_80021594(&digitBox, color1, D_8008D2AC->pushBuffer_UI.ptrOT,
                  &D_8008D2AC->backBuffer->primMem);

    pBackBuf = D_8008D2AC->backBuffer;
    pPrimCurr = pBackBuf->primMem.curr;
    p = 0;
    if ((unsigned int)pPrimCurr <= (unsigned int)pBackBuf->primMem.endMin100) {
        pBackBuf->primMem.curr = (char *)pPrimCurr + 0x18;
        p = (POLY_F4 *)pPrimCurr;
    }
    if (p != 0) {
        *(unsigned int *)&p->r0 = 0x28ffffff;
        p->x1 = posX + 0xe;
        p->x3 = posX + 0xe;
        p->x0 = sBoxX;
        p->y0 = sBoxY;
        p->y1 = sBoxY;
        p->x2 = sBoxX;
        p->y2 = posY + -0x23;
        p->y3 = posY + -0x23;
        ot = D_8008D2AC->pushBuffer_UI.ptrOT;
        *(int *)p = *ot | 0x5000000;
        *ot = (unsigned int)p & 0xffffff;

        barBox.y = posY + -0x26;
        barBox.w = 0xc;
        barBox.h = 0x26;
        barBox.x = posX;
        func_800718BC(color2, 0, 4);
        func_80021594(&barBox, color2, D_8008D2AC->pushBuffer_UI.ptrOT,
                      &D_8008D2AC->backBuffer->primMem);

        pBackBuf = D_8008D2AC->backBuffer;
        pPrimCurr = pBackBuf->primMem.curr;
        p = 0;
        if ((unsigned int)pPrimCurr <= (unsigned int)pBackBuf->primMem.endMin100) {
            pBackBuf->primMem.curr = (char *)pPrimCurr + 0x18;
            p = (POLY_F4 *)pPrimCurr;
        }
        if (p != 0) {
            jmColor = driver->jumpMeter;
            jmHeight = driver->jumpMeter;
            uBarColor = 0x28ff0000;
            if (0x27f < jmColor) {
                if (jmColor < 0x3c0) {
                    uBarColor = 0x2800ff00;
                } else if (jmColor < 0x5a0) {
                    uBarColor = 0x2800ffff;
                } else {
                    uBarColor = 0x280000ff;
                }
            }
            *(unsigned int *)&p->r0 = uBarColor;
            sBarRight = posX + 0xc;
            p->x0 = posX;
            p->x1 = sBarRight;
            p->x2 = posX;
            p->y2 = posY;
            p->x3 = sBarRight;
            p->y3 = posY;
            jumpMeterHeight = (int)jmHeight * 0x26;
            sBoxY = posY - (short)(jumpMeterHeight / 0x960);
            p->y0 = sBoxY;
            p->y1 = sBoxY;
            ot = D_8008D2AC->pushBuffer_UI.ptrOT;
            *(int *)p = *ot | 0x5000000;
            *ot = (unsigned int)p & 0xffffff;

            pBackBuf = D_8008D2AC->backBuffer;
            pPrimCurr = pBackBuf->primMem.curr;
            p = 0;
            if ((unsigned int)pPrimCurr <= (unsigned int)pBackBuf->primMem.endMin100) {
                pBackBuf->primMem.curr = (char *)pPrimCurr + 0x18;
                p = (POLY_F4 *)pPrimCurr;
            }
            if (p != 0) {
                *(unsigned int *)&p->r0 = 0x28808080;
                p->x0 = posX;
                p->y0 = posY + -0x26;
                p->x1 = sBarRight;
                p->y1 = posY + -0x26;
                p->x2 = posX;
                p->y2 = posY;
                p->x3 = sBarRight;
                p->y3 = posY;
                ot = D_8008D2AC->pushBuffer_UI.ptrOT;
                *(int *)p = *ot | 0x5000000;
                *ot = (unsigned int)p & 0xffffff;
            }
        }
    }
}
