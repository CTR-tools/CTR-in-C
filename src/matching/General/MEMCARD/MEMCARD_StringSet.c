/*
 * MEMCARD_StringSet @ 0x8003D730  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact match: base score 600, Branch 0, Stack 0, Register 20
 * (register-coloring/scheduling residual only; natural C, no hacks).
 *
 * MEMCARD_StringInit then appends srcString up to a total length of 63 chars, NUL-terminated.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_String.c audited CLEAN vs this asm + Ghidra.
 */
extern char *func_8003D6E8(int slot, char *dst);   /* StringInit */
void MEMCARD_StringSet(char *dstString, int slotIndex, char *srcString)
{
    int i;
    char c;
    int iScan;
    func_8003D6E8(slotIndex, dstString);
    i = 0;
    if (*dstString != '\0') {
        iScan = 1;
        do {
            i = iScan;
            iScan = i + 1;
        } while (dstString[i] != '\0');
    }
    c = *srcString;
    for (; (c != '\0' && i < 0x3f); i = i + 1) {
        c = *srcString;
        srcString = srcString + 1;
        dstString[i] = c;
        c = *srcString;
    }
    dstString[i] = '\0';
}
