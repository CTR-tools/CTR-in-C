/*
 * MEMCARD_FindFirstGhost @ 0x8003E600  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact match: base score 345, Branch 0, Stack 0, Register 5
 * (register-coloring/scheduling residual only; natural C, no hacks).
 *
 * Begins a ghost-file directory search (if idle): firstfile(); on a hit stage=GHOST_FOUND, copies the name to g_szMemcardFindGhostFile and returns it; NULL otherwise.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_RetailCard.c audited CLEAN vs this asm + Ghidra.
 */
extern void func_8003D730(char *dst, int slot, char *name); /* StringSet */
extern char *func_80078088(char *path, void *dir);           /* firstfile */
extern char *func_80075CC0(char *dst, char *src);            /* strcpy */
extern char D_800990A4[];   /* g_szMemcardFileCurr; findGhost at +0x20 */
int D_8008D404 = 0;   /* g_nMemcardStage (gp+0x498) */
char *MEMCARD_FindFirstGhost(int slotIdx, char *srcString)
{
    char *pEntry;
    char dirEntry[40];
    char *pBase = D_800990A4;
    if (D_8008D404 == 0) {
        func_8003D730(pBase, slotIdx, srcString);
        pEntry = func_80078088(pBase, dirEntry);
        if (pEntry == dirEntry) {
            D_8008D404 = 0xf;
            func_80075CC0(pBase + 0x20, pEntry);
            return pBase + 0x20;
        }
    }
    return 0;
}
