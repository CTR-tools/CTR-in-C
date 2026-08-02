/*
 * MEMCARD_WaitForHwEvent @ 0x8003DAE4  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Busy-waits for a hardware card event and returns its MC_RETURN code; never returns PENDING.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_Events.c audited CLEAN vs this asm + Ghidra.
 */
extern long func_80078068(long event);   /* TestEvent */
int D_8008D8C8 = 0;  /* HwEvIOE     (gp+0x95C) */
int D_8008D8B4 = 0;  /* HwEvError   (gp+0x948) */
int D_8008D8DC = 0;  /* HwEvTimeout (gp+0x970) */
int D_8008D8BC = 0;  /* HwEvNew     (gp+0x950) */
int MEMCARD_WaitForHwEvent(void)
{
    long r;
    do {
        if (func_80078068(D_8008D8C8) == 1) return 0;
        if (func_80078068(D_8008D8B4) == 1) return 1;
        if (func_80078068(D_8008D8DC) == 1) return 2;
        r = func_80078068(D_8008D8BC);
    } while (r != 1);
    return 3;
}
