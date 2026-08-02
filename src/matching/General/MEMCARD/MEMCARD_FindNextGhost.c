/*
 * MEMCARD_FindNextGhost @ 0x8003E678  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Continues a ghost search (only while GHOST_FOUND): nextfile(); on a hit copies the name and returns it; on end stage=IDLE and returns NULL.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_RetailCard.c audited CLEAN vs this asm + Ghidra.
 */
extern char *func_80077FE8(void *dir);                 /* nextfile */
extern char *func_80075CC0(char *dst, char *src);      /* strcpy */
extern char D_800990C4[];   /* g_szMemcardFindGhostFile */
int D_8008D404 = 0;   /* g_nMemcardStage (gp+0x498) */
char *MEMCARD_FindNextGhost(void)
{
    char *pEntry;
    char dirEntry[40];
    if (D_8008D404 == 0xf) {
        pEntry = func_80077FE8(dirEntry);
        if (pEntry == dirEntry) {
            func_80075CC0(D_800990C4, pEntry);
            return D_800990C4;
        }
        D_8008D404 = 0;
    }
    return 0;
}
