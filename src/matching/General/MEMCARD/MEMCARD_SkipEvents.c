/*
 * MEMCARD_SkipEvents @ 0x8003DB54  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact match: base score 200, Branch 0, Stack 0, Register 0
 * (register-coloring/scheduling residual only; natural C, no hacks).
 *
 * Flushes all pending sw then hw card events by polling until each returns PENDING. Residual = a spare delay-slot constant reload; scheduling-only.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_Events.c audited CLEAN vs this asm + Ghidra.
 */
extern int func_8003D9EC(void);   /* GetNextSwEvent */
extern int func_8003DA68(void);   /* GetNextHwEvent */
void MEMCARD_SkipEvents(void)
{
    while (func_8003D9EC() != 7);
    while (func_8003DA68() != 7);
}
