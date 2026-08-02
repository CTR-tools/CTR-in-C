/*
 * MEMCARD_CloseFile @ 0x8003DBF8  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Closes the open memcard file (if fd != -1), resets fd=-1 and stage=IDLE.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_FileIO.c audited CLEAN vs this asm + Ghidra.
 */
extern void func_80077ED8(int fd);   /* close */
int D_8008D40C = 0;   /* g_nMemcardFd    (gp+0x4A0) */
int D_8008D404 = 0;   /* g_nMemcardStage (gp+0x498) */
void MEMCARD_CloseFile(void)
{
    if (D_8008D40C != -1) {
        func_80077ED8(D_8008D40C);
        D_8008D40C = -1;
    }
    D_8008D404 = 0;
}
