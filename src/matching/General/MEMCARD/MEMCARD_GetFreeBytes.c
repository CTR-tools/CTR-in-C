/*
 * MEMCARD_GetFreeBytes @ 0x8003DD10  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact match: base score 120, Branch 0, Stack 0, Register 4
 * (register-coloring/scheduling residual only; natural C, no hacks).
 *
 * Enumerates all files (firstfile/nextfile), sums each size rounded up to 0x2000 blocks, sets g_nMemcardSizeRemaining = 0x1E000 - used. Retail is known-buggy (false out-of-room).
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_RetailCard.c audited CLEAN vs this asm + Ghidra.
 */
typedef struct { char name[20]; unsigned long attr; unsigned long size; void *next; unsigned long head; char system[4]; } DIRENTRY;
extern void func_8003D730(char *dst, int slot, char *name);   /* StringSet */
extern DIRENTRY *func_80078088(char *path, DIRENTRY *dir);    /* firstfile */
extern DIRENTRY *func_80077FE8(DIRENTRY *dir);                /* nextfile */
extern char D_800990A4[];   /* g_szMemcardFileCurr */
extern char D_8008D41C[];   /* any-file pattern (g_szMemcardDirHeader+8) */
int D_8008D8AC = 0;  /* g_nMemcardSizeRemaining (gp+0x940) */
void MEMCARD_GetFreeBytes(int slotIdx)
{
    DIRENTRY *pEntry;
    DIRENTRY *pWalk;
    int iUsedBytes;
    DIRENTRY dirEntry;
    func_8003D730(D_800990A4, slotIdx, D_8008D41C);
    iUsedBytes = 0;
    pEntry = func_80078088(D_800990A4, &dirEntry);
    pWalk = &dirEntry;
    while (pEntry == pWalk) {
        iUsedBytes += (dirEntry.size + 0x1fff) & 0xffffe000;
        pWalk = func_80077FE8(&dirEntry);
    }
    D_8008D8AC = 0x1e000 - iUsedBytes;
}
