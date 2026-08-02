/*
 * MEMCARD_NewTask @ 0x8003DB98  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Sets up a memcard task: stores slot, builds the file path (MEMCARD_StringSet), stores buffer/size, retry count 8; returns 0.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_FileIO.c audited CLEAN vs this asm + Ghidra.
 */
extern char D_800990A4[];                                   /* task filename buffer */
extern void func_8003D730(char *dst, int slot, char *name); /* StringSet */
int D_8008D8B8 = 0;   /* slot          (gp+0x94C) */
int D_8008D8A8 = 0;   /* fileSize      (gp+0x93C) */
int D_8008D408 = 0;   /* ptrStart      (gp+0x49C) */
int D_8008D410 = 0;   /* remainingAttempts (gp+0x4A4) */
int MEMCARD_NewTask(int slotIdx, char *name, int ptrMemcard, int memcardFileSize)
{
    D_8008D8B8 = slotIdx;
    func_8003D730(D_800990A4, slotIdx, name);
    D_8008D408 = ptrMemcard;
    D_8008D8A8 = memcardFileSize;
    D_8008D410 = 8;
    return 0;
}
