/*
 * UI_DrawSpeedNeedle @ 0x800511C0 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg107, score 11035). Real types (uitypes.h POLY_G3).
 *
 * Draws the rotating speedometer needle as two gouraud triangles (front + mirrored back face for
 * a pseudo-3D look) at (posX,posY). Speed source = driver->unk36E (smoothed needle/RPM value),
 * scaled by kart-class stats: maxScale = FP8_INT((short)const_AccelSpeed_ClassStat + const_SacredFireSpeed).
 * Two regimes: if const_AccelSpeed < speed the needle sweeps the boost arc (maxAngle ANG(157.5)=0x700,
 * minAngle ANG(213.75)=0x980, raised minScale); else the normal arc (0x980 .. ANG(305.2)=0xD90).
 * angle1 = VehCalc_MapToRange(speedScale, minScale, maxScale, minAngle, maxAngle) (all scales
 * *108000/64000); angle2 = angle1 + ANG(90)=0x400. Per vertex the 926 RE-CALLS MATH_Sin (x span)
 * and MATH_Cos (y span, 0.625 squash via *0x140>>9 with round-to-zero), so Sin/Cos are invoked 6x
 * each (NOT cached in cos[2]/sin[2] like the decomp). Two PolyG3 (7 words, GPU code 0x06) into
 * backBuffer->primMem, linked to pushBuffer_UI.ptrOT; front colors (91,91,0)/(50,43,1)/(255,187,0),
 * back white/(156,105,0)/(255,255,0). Front/back share v0,v1 and mirror v2 (needle tip).
 *
 * NOTE(claude): the reg residual is a gcc-optimizer wall in the SCALE SETUP — retail hoists the
 * /64000 magic (0x10624DD3) into one register and branch-duplicates the three `*108000/64000`
 * divisions across the if/else + MapToRange call; the natural C above emits them straight-line, so
 * the setup register allocation diverges and cascades (posY reg etc.). Behavior is identical
 * (verified vs Ghidra @0x800511C0); a decomp-mangled permuter variant only reached 9785, so the
 * natural form is delivered (no hand-forcing). vs decomp: mask/speed source unk36E, the maxScale
 * base is `((short)accel + sacredFire) >> 8` (single shift; decomp's FP8_INT(a)+FP8_INT(b) differs),
 * and Sin/Cos are recomputed per vertex; USE_ONLINE speed-text path absent in 926.
 * Phase B (saphi): see note below.
 */
#include "../uitypes.h"

extern struct GameTracker *D_8008D2AC;      /* gGT (absolute) */
extern int func_80058F9C(int val, int inMin, int inMax, int outMin, int outMax); /* VehCalc_MapToRange */
extern int func_8003D184(unsigned int angle);   /* MATH_Sin */
extern int func_8003D1C0(unsigned int angle);   /* MATH_Cos */

void UI_DrawSpeedNeedle(short posX, short posY, struct Driver *driver)
{
    int iScaleByte;
    int iMinAngle;
    int iMaxScale;
    int iSpeedFixed;
    int iTrig;
    int maxAngle;
    int speedScale;
    struct PushBuffer *pBackBuf;
    void *pPrimCurr;
    POLY_G3 *pPrim;
    unsigned int angle, angle_00;
    unsigned int *ot;

    iScaleByte = (unsigned short)driver->const_AccelSpeed_ClassStat << 0x10;
    iMinAngle = iScaleByte >> 0x10;
    iMaxScale = (iMinAngle + driver->const_SacredFireSpeed) >> 8;
    iSpeedFixed = (unsigned short)driver->unk36E << 0x10;
    iTrig = 0;
    iScaleByte = iScaleByte >> 0x18;
    if (iMinAngle < iSpeedFixed >> 0x10) {
        maxAngle = 0x700;
        iMinAngle = 0x980;
        iTrig = (iScaleByte * 0x1a5e0) / 64000;
    } else {
        maxAngle = 0x980;
        iMinAngle = 0xd90;
        iMaxScale = iScaleByte;
    }
    speedScale = (iSpeedFixed >> 0x18) * 0x1a5e0 / 64000;
    iMaxScale = iMaxScale * 0x1a5e0 / 64000;
    iTrig = func_80058F9C(speedScale, iTrig, iMaxScale, iMinAngle, maxAngle);

    pBackBuf = D_8008D2AC->backBuffer;
    pPrimCurr = pBackBuf->primMem.curr;
    pPrim = 0;
    if ((unsigned int)pPrimCurr <= (unsigned int)pBackBuf->primMem.endMin100) {
        pBackBuf->primMem.curr = (char *)pPrimCurr + 0x1c;
        pPrim = (POLY_G3 *)pPrimCurr;
    }
    if (pPrim != 0) {
        angle = (unsigned int)(short)iTrig;
        angle_00 = angle + 0x400;

        *(int *)&pPrim->r0 = 0x30005b5b;
        *(int *)&pPrim->r1 = 0x30012b32;
        *(int *)&pPrim->r2 = 0x3000bbff;

        iTrig = func_8003D184(angle_00);
        pPrim->x2 = posX + (iTrig * 3 >> 0xb) + 0x41;
        iTrig = func_8003D1C0(angle_00);
        iTrig = ((iTrig << 2) >> 0xb) * 0x140;
        if (iTrig < 0) iTrig += 0x1ff;
        pPrim->y2 = posY + (iTrig >> 9) + 0x29;

        iTrig = func_8003D184(angle);
        pPrim->x1 = posX - ((iTrig * 3 >> 0xb) + -0x41);
        iTrig = func_8003D1C0(angle);
        iTrig = (iTrig * 3 >> 0xb) * 0x140;
        if (iTrig < 0) iTrig += 0x1ff;
        pPrim->y1 = posY - ((iTrig >> 9) + -0x29);

        iTrig = func_8003D184(angle);
        pPrim->x0 = posX + (iTrig * 0x1e >> 0xb) + 0x41;
        iTrig = func_8003D1C0(angle);
        iTrig = (iTrig * 0x1e >> 0xb) * 0x140;
        if (iTrig < 0) iTrig += 0x1ff;
        pPrim->y0 = posY + (iTrig >> 9) + 0x29;

        ot = D_8008D2AC->pushBuffer_UI.ptrOT;
        *(int *)pPrim = *ot | 0x6000000;
        *ot = (unsigned int)pPrim & 0xffffff;

        pBackBuf = D_8008D2AC->backBuffer;
        pPrimCurr = pBackBuf->primMem.curr;
        pPrim = 0;
        if ((unsigned int)pPrimCurr <= (unsigned int)pBackBuf->primMem.endMin100) {
            pBackBuf->primMem.curr = (char *)pPrimCurr + 0x1c;
            pPrim = (POLY_G3 *)pPrimCurr;
        }
        if (pPrim != 0) {
            *(int *)&pPrim->r0 = 0x30ffffff;
            *(int *)&pPrim->r1 = 0x3000699c;
            *(int *)&pPrim->r2 = 0x3000ffff;

            iTrig = func_8003D184(angle_00);
            pPrim->x2 = posX - ((iTrig * 3 >> 0xb) + -0x41);
            iTrig = func_8003D1C0(angle_00);
            iTrig = ((iTrig << 2) >> 0xb) * 0x140;
            if (iTrig < 0) iTrig += 0x1ff;
            pPrim->y2 = posY - ((iTrig >> 9) + -0x29);

            iTrig = func_8003D184(angle);
            pPrim->x1 = posX - ((iTrig * 3 >> 0xb) + -0x41);
            iTrig = func_8003D1C0(angle);
            iTrig = (iTrig * 3 >> 0xb) * 0x140;
            if (iTrig < 0) iTrig += 0x1ff;
            pPrim->y1 = posY - ((iTrig >> 9) + -0x29);

            iTrig = func_8003D184(angle);
            pPrim->x0 = posX + (iTrig * 0x1e >> 0xb) + 0x41;
            iTrig = func_8003D1C0(angle);
            iTrig = (iTrig * 0x1e >> 0xb) * 0x140;
            if (iTrig < 0) iTrig += 0x1ff;
            pPrim->y0 = posY + (iTrig >> 9) + 0x29;

            ot = D_8008D2AC->pushBuffer_UI.ptrOT;
            *(int *)pPrim = *ot | 0x6000000;
            *ot = (unsigned int)pPrim & 0xffffff;
        }
    }
}
