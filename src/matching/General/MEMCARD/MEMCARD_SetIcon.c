/*
 * MEMCARD_SetIcon @ 0x8003D4E4  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact match: base score 220, Branch 0, Stack 0, Register 4
 * (register-coloring/scheduling residual only; natural C, no hacks).
 *
 * Copies one of two 256-byte save-icon images (variant 0 -> icon0, else icon1) into the active icon buffer via a 16-byte-per-iteration word copy.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_Icon.c audited CLEAN vs this asm + Ghidra.
 */
typedef struct { int w0, w1, w2, w3; } IconBlk;
extern IconBlk D_800857A0[16];  /* g_abMemcardIconActive (dst) */
extern IconBlk D_800855A0[16];  /* icon variant !=0 src */
extern IconBlk D_800856A0[16];  /* icon variant ==0 src */
void MEMCARD_SetIcon(short iconVariant)
{
    IconBlk *dst = D_800857A0;
    IconBlk *src;
    IconBlk *end;
    if (iconVariant == 0)
        src = D_800856A0;
    else
        src = D_800855A0;
    end = src + 16;
    do {
        *dst = *src;
        src++;
        dst++;
    } while (src != end);
}
