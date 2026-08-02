/*
 * MEMCARD_IsFile @ 0x8003E59C  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Tests whether a save exists: build path, open FASYNC|FWRITE; IOE(0)+close if found, else NODATA(6); fd wiped to -1.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_RetailCard.c audited CLEAN vs this asm + Ghidra.
 */
extern void func_8003D730(char *dst, int slot, char *name);  /* StringSet */
extern int func_80078078(char *path, int mode);              /* open */
extern void func_80077ED8(int fd);                            /* close */
int D_8008D40C = 0;   /* g_nMemcardFd (gp+0x4A0) */
int MEMCARD_IsFile(int slotIdx, char *save_name)
{
    char path[64];
    func_8003D730(path, slotIdx, save_name);
    D_8008D40C = func_80078078(path, 0x8002);
    if (D_8008D40C == -1)
        return 6;
    func_80077ED8(D_8008D40C);
    D_8008D40C = -1;
    return 0;
}
