/*
 * MEMCARD_LoadFile @ 0x8003E29C  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Begins loading a save (only if idle and NewTask succeeds): opens FASYNC|FREAD, sets/clears status bit8 from flags&2, stage=LOAD_PART0_START, kicks off the 0x80-byte header read; NODATA(6) on open fail, TIMEOUT(1) if busy.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_RetailTransfer.c audited CLEAN vs this asm + Ghidra.
 */
extern int func_8003DB98(int slot, char *name, void *ptr, int size);  /* NewTask */
extern int func_80078078(char *path, int mode);                       /* open */
extern void func_8003DBF8(void);                                       /* CloseFile */
extern int func_8003DC30(int off, int size);                          /* ReadFile */
extern char D_800990A4[];   /* g_szMemcardFileCurr */
int D_8008D404 = 0;  /* stage 0x498 */
int D_8008D40C = 0;  /* fd 0x4A0 */
int D_8008D8CC = 0;  /* statusFlags 0x960 */
int MEMCARD_LoadFile(int slotIdx, char *name, void *ptrMemcard, int memcardFileSize, unsigned int flags)
{
    if (D_8008D404 != 0)
        return 1;
    if (func_8003DB98(slotIdx, name, ptrMemcard, memcardFileSize) != 0)
        return 1;
    D_8008D40C = func_80078078(D_800990A4, 0x8001);
    if (D_8008D40C == -1) {
        func_8003DBF8();
        return 6;
    }
    if ((flags & 2) != 0)
        D_8008D8CC = D_8008D8CC | 8;
    else
        D_8008D8CC = D_8008D8CC & 0xfffffff7;
    D_8008D404 = 3;
    return func_8003DC30(0, 0x80);
}
