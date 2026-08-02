/*
 * MEMCARD_WriteFile @ 0x8003DC9C  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Starts an async write of size bytes from buffer to start_offset; PENDING(7) on success, else CloseFile + TIMEOUT(1).
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_FileIO.c audited CLEAN vs this asm + Ghidra.
 */
extern int func_80077EC8(int fd, int off, int whence);  /* lseek */
extern int func_80077FB8(int fd, void *buf, int n);       /* write */
extern void func_8003DBF8(void);                           /* CloseFile */
int D_8008D40C = 0;   /* g_nMemcardFd (gp+0x4A0) */
int MEMCARD_WriteFile(int start_offset, void *buffer, int size)
{
    if (func_80077EC8(D_8008D40C, start_offset, 0) != -1 &&
        func_80077FB8(D_8008D40C, buffer, size) != -1)
        return 7;
    func_8003DBF8();
    return 1;
}
