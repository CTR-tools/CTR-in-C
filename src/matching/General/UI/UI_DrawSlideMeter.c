/*
 * UI_DrawSlideMeter @ 0x80052250 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg56, score 2620 — barHeight/posX register cascade + the
 * green/red color's shared-lui allocation; the two POLY_F4 quads match). Real types.
 *
 * Draws the power-slide / turbo-reserve "fire" gauge at (posX,posY). barWidth = 0x31 (49);
 * barHeight = 3 when 3/4-player (gGT->numPlyrCurrGame > 2) else 7. When turbo_MeterRoomLeft (@0x3DC)
 * != 0, the filled length = 0x31 - (turbo_MeterRoomLeft*0x31)/(const_turboMaxRoom@0x476 * 32).
 * Draws a black (memset-zeroed color) wire box via CTR_Box_DrawWireBox, then two POLY_F4 flat quads
 * (6 words, GPU code 0x28-semitrans, OT tag |0x5000000) into backBuffer->primMem: the filled portion
 * green (0x2800ff00) while still building reserve, or red (0x280000ff) once
 * const_turboLowRoomWarning@0x477 * 32 < turbo_MeterRoomLeft (boost ready); then a gray (0x28808080)
 * full-width background bar. (The decomp's 2-iteration loop is unrolled in the 926.)
 *
 * NOTE(claude) vs in-repo decomp (REFERENCE; 926 asm authoritative): the WireBox color is a
 * memset(&buf,0,4) BLACK buffer (func_800718BC = memset), NOT MakeColor(0,0,0) via a color arg;
 * barWidth = 0x31 (decomp WIDE_34(49)); ELAPSED_MS = 32. USE_ONLINE/widescreen/boostbar absent.
 * turbo_MeterRoomLeft read lh (signed short); const_turbo* read lbu (char). Phase B (saphi): below.
 */
#include "../uitypes.h"

extern struct GameTracker *D_8008D2AC;      /* gGT (absolute) */
extern void func_80021594(struct RECT *r, unsigned char *color,
                          void *ot, struct PrimMem *pm);            /* CTR_Box_DrawWireBox */
extern void *func_800718BC(void *dst, int c, int n);               /* memset */

void UI_DrawSlideMeter(short posX, short posY, struct Driver *driver)
{
    short meterLength;
    short barHeight;
    int iFillNum;
    int iFillDen;
    unsigned int uBarColor;
    struct PushBuffer *pBackBuf;
    void *pPrimCurr;
    POLY_F4 *p;
    unsigned int *ot;
    struct RECT meterBox;
    unsigned char color1[4];

    meterLength = 0;
    barHeight = 7;
    if (2 < (unsigned char)D_8008D2AC->numPlyrCurrGame) {
        barHeight = 3;
    }
    if (driver->turbo_MeterRoomLeft != 0) {
        iFillNum = driver->turbo_MeterRoomLeft * 0x31;
        iFillDen = (unsigned char)driver->const_turboMaxRoom << 5;
        meterLength = 0x31 - (short)(iFillNum / iFillDen);
    }

    meterBox.x = posX + -0x31;
    meterBox.y = posY - barHeight;
    meterBox.w = 0x31;
    meterBox.h = barHeight;
    func_800718BC(color1, 0, 4);
    func_80021594(&meterBox, color1, D_8008D2AC->pushBuffer_UI.ptrOT,
                  &D_8008D2AC->backBuffer->primMem);

    pBackBuf = D_8008D2AC->backBuffer;
    pPrimCurr = pBackBuf->primMem.curr;
    p = 0;
    if ((unsigned int)pPrimCurr <= (unsigned int)pBackBuf->primMem.endMin100) {
        pBackBuf->primMem.curr = (char *)pPrimCurr + 0x18;
        p = (POLY_F4 *)pPrimCurr;
    }
    if (p != 0) {
        if ((int)((unsigned char)driver->const_turboLowRoomWarning << 5) < (int)driver->turbo_MeterRoomLeft) {
            uBarColor = 0x2800ff00;
        } else {
            uBarColor = 0x280000ff;
        }
        *(unsigned int *)&p->r0 = uBarColor;
        barHeight = posY - barHeight;
        p->x0 = posX - meterLength;
        p->y0 = barHeight;
        p->x1 = posX;
        p->y1 = barHeight;
        p->y2 = posY;
        p->x3 = posX;
        p->y3 = posY;
        p->x2 = posX - meterLength;
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
            p->x0 = posX + -0x31;
            p->y0 = barHeight;
            p->x1 = posX;
            p->y1 = barHeight;
            p->y2 = posY;
            p->x3 = posX;
            p->y3 = posY;
            p->x2 = posX + -0x31;
            ot = D_8008D2AC->pushBuffer_UI.ptrOT;
            *(int *)p = *ot | 0x5000000;
            *ot = (unsigned int)p & 0xffffff;
        }
    }
}
