/*
 * MEMCARD_EraseFile @ 0x8003E6D4  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Erases a save (only if idle): build path, stage=ERASE_PASS, erase(); on failure stage=ERASE_FAIL; refresh free space (GetFreeBytes); returns PENDING(7).
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_RetailCard.c audited CLEAN vs this asm + Ghidra.
 */
extern void func_8003D730(char *dst, int slot, char *name);  /* StringSet */
extern long func_80077EB8(char *path);                        /* erase */
extern void func_8003DD10(int slot);                          /* GetFreeBytes */
int D_8008D404 = 0;  /* stage 0x498 */
int MEMCARD_EraseFile(int slotIdx, char *srcString)
{
    long er;
    char path[64];
    if (D_8008D404 != 0)
        return 1;
    func_8003D730(path, slotIdx, srcString);
    D_8008D404 = 0xe;
    er = func_80077EB8(path);
    if (er != 1)
        D_8008D404 = 0xd;
    func_8003DD10(slotIdx);
    return 7;
}
