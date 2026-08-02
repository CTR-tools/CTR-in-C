/*
 * MEMCARD_GetNextHwEvent @ 0x8003DA68  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Hardware-event counterpart of GetNextSwEvent; returns the MC_RETURN code (PENDING if none).
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_Events.c audited CLEAN vs this asm + Ghidra.
 */
extern long func_80078068(long event);   /* TestEvent */
int D_8008D8C8 = 0;  /* HwEvIOE     (gp+0x95C) */
int D_8008D8B4 = 0;  /* HwEvError   (gp+0x948) */
int D_8008D8DC = 0;  /* HwEvTimeout (gp+0x970) */
int D_8008D8BC = 0;  /* HwEvNew     (gp+0x950) */
int MEMCARD_GetNextHwEvent(void)
{
    if (func_80078068(D_8008D8C8) == 1) return 0;
    if (func_80078068(D_8008D8B4) == 1) return 1;
    if (func_80078068(D_8008D8DC) == 1) return 2;
    if (func_80078068(D_8008D8BC) == 1) return 3;
    return 7;
}
