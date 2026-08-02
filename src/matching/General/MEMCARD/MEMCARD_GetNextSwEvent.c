/*
 * MEMCARD_GetNextSwEvent @ 0x8003D9EC  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Polls the software card events in priority order (IOE/ERROR/TIMEOUT/NEW) and returns the matching MC_RETURN code, or PENDING(7) if none fired.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_Events.c audited CLEAN vs this asm + Ghidra.
 */
extern long func_80078068(long event);   /* TestEvent */
int D_8008D8D4 = 0;  /* SwEvIOE     (gp+0x968) */
int D_8008D8C4 = 0;  /* SwEvError   (gp+0x958) */
int D_8008D8E0 = 0;  /* SwEvTimeout (gp+0x974) */
int D_8008D8C0 = 0;  /* SwEvNew     (gp+0x954) */
int MEMCARD_GetNextSwEvent(void)
{
    if (func_80078068(D_8008D8D4) == 1) return 0;
    if (func_80078068(D_8008D8C4) == 1) return 1;
    if (func_80078068(D_8008D8E0) == 1) return 2;
    if (func_80078068(D_8008D8C0) == 1) return 3;
    return 7;
}
