/*
 * UI_WeaponBG_AnimateShine @ 0x8004E0E0 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (large reg residual = byte-packing scheduling + frame-size wall;
 * base 167 = target 167, byte formula verified vs Ghidra). Per-frame weapon/wumpa shine gradient from
 * abs(MATH_Sin(theta)): result = (sine*0xFF>>13)+0x80; warm gradient color1 {(7f,7f,0),(7f,32,0),(21,10,0)}
 * and gray gradient color2 (5f,5f,5f), each channel = base + (base*sine>>12), built in a packed byte buffer.
 * Phase B (saphi UI_Icon.c:4): CLEAN — identical color values (byte-array writes produce the same
 * packed bytes as retail's word writes); result/gradients verified.
 */
/* UI_WeaponBG_AnimateShine @ 0x8004E0E0 — per-frame wumpa/weapon shine gradient from sin(theta). */
extern int func_8003D184(int theta);   /* MATH_Sin */

short D_8008D990 = 0;      /* g_nWumpaShineTheta (gp+0xA24) */
int D_8008D994 = 0;        /* g_nWumpaShineResult      (gp+0xA28) */
int D_8008D998 = 0;        /* g_anWumpaShineColor1[0]  (gp+0xA2C) */
int D_8008D99C = 0;        /* g_anWumpaShineColor1[1]  (gp+0xA30) */
int D_8008D9A0 = 0;        /* g_anWumpaShineColor1[2]  (gp+0xA34) */
int D_8008D9A4 = 0;        /* g_anWumpaShineColor2[0]  (gp+0xA38) */
int D_8008D9A8 = 0;        /* g_anWumpaShineColor2[1]  (gp+0xA3C) */
int D_8008D9AC = 0;        /* g_anWumpaShineColor2[2]  (gp+0xA40) */

void UI_WeaponBG_AnimateShine(void)
{
    int sine;
    unsigned char buf[12];

    sine = func_8003D184((int)D_8008D990);
    *(int *)buf = 0x7F7F;
    if (sine < 0) {
        sine = -sine;
    }
    D_8008D994 = ((sine * 0xff) >> 0xd) + 0x80;
    *(int *)(buf + 4) = 0x327F;
    *(int *)(buf + 8) = 0x1021;

    /* warm gradient: each channel = base + (base*sine >> 12) (3 bytes/word; blue base 0) */
    buf[0] += (sine * buf[0]) >> 0xc;
    buf[1] += (sine * buf[1]) >> 0xc;
    buf[2] += (sine * buf[2]) >> 0xc;
    buf[4] += (sine * buf[4]) >> 0xc;
    buf[5] += (sine * buf[5]) >> 0xc;
    buf[6] += (sine * buf[6]) >> 0xc;
    buf[8] += (sine * buf[8]) >> 0xc;
    buf[9] += (sine * buf[9]) >> 0xc;
    buf[10] += (sine * buf[10]) >> 0xc;
    D_8008D998 = *(int *)buf;
    D_8008D99C = *(int *)(buf + 4);
    D_8008D9A0 = *(int *)(buf + 8);

    /* uniform gray (0x5f) pulsing gradient (buffer re-used) */
    *(int *)buf = 0x5F5F5F;
    *(int *)(buf + 4) = 0x5F5F5F;
    *(int *)(buf + 8) = 0x5F5F5F;
    buf[0] += (sine * buf[0]) >> 0xc;
    buf[1] += (sine * buf[1]) >> 0xc;
    buf[2] += (sine * buf[2]) >> 0xc;
    buf[4] += (sine * buf[4]) >> 0xc;
    buf[5] += (sine * buf[5]) >> 0xc;
    buf[6] += (sine * buf[6]) >> 0xc;
    buf[8] += (sine * buf[8]) >> 0xc;
    buf[9] += (sine * buf[9]) >> 0xc;
    buf[10] += (sine * buf[10]) >> 0xc;
    D_8008D9A4 = *(int *)buf;
    D_8008D9A8 = *(int *)(buf + 4);
    D_8008D9AC = *(int *)(buf + 8);
}
