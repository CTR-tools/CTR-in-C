/*
 * MEMCARD_StringInit @ 0x8003D6E8  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact match: base score 210, Branch 0, Stack 0, Register 2
 * (register-coloring/scheduling residual only; natural C, no hacks).
 *
 * Writes the memcard device path buXY: for the given slot (XY = port/slot digits) into dstString (defaults to g_szMemcardDirHeader) and returns it.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_String.c audited CLEAN vs this asm + Ghidra.
 */
extern char D_8008D414[];   /* g_szMemcardDirHeader default */
char *MEMCARD_StringInit(int slotIndex, char *dstString)
{
    if (dstString == 0)
        dstString = D_8008D414;
    *(unsigned int *)dstString =
          ((slotIndex >> 4 & 1U) + 0x30) * 0x10000
        | (((slotIndex & 3U) + 0x30) * 0x1000000 | 0x7562);
    *(unsigned short *)(dstString + 4) = 0x3a;
    return dstString;
}
