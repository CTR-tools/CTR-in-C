/*
 * MEMCARD_ReadFile @ 0x8003DC30  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Starts an async read of size bytes from start_offset into g_pMemcardStart; PENDING(7) on success, else CloseFile + TIMEOUT(1).
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_FileIO.c audited CLEAN vs this asm + Ghidra.
 */
extern int func_80077EC8(int fd, int off, int whence);   /* lseek */
extern int func_80078058(int fd, void *buf, int n);        /* read */
extern void func_8003DBF8(void);                            /* CloseFile */
int D_8008D40C = 0;    /* g_nMemcardFd     (gp+0x4A0) */
void *D_8008D408 = 0;  /* g_pMemcardStart  (gp+0x49C) */
int MEMCARD_ReadFile(int start_offset, int size)
{
    if (func_80077EC8(D_8008D40C, start_offset, 0) != -1 &&
        func_80078058(D_8008D40C, D_8008D408, size) != -1)
        return 7;
    func_8003DBF8();
    return 1;
}
