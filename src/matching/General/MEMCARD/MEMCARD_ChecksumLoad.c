/*
 * MEMCARD_ChecksumLoad @ 0x8003D618  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact match: base score 585, Branch 0, Stack 0, Register 13
 * (register-coloring/scheduling residual only; natural C, no hacks).
 *
 * Incremental CRC verify (0x200 bytes/call, or all at once when status bit 0x8); on the final chunk folds in the stored 2-byte checksum and returns (crc != 0); returns 7 (PENDING) while chunks remain.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_Checksum.c audited CLEAN vs this asm + Ghidra.
 */
extern unsigned int func_8003D540(unsigned int crc, int b);  /* CRC16 */
int D_8008D8D0 = 0;   /* g_nMemcardCrc16        (gp+0x964) */
int D_8008D8CC = 0;   /* g_dwMemcardStatusFlags (gp+0x960) */
int D_8008D8B0 = 0;   /* g_nMemcardCrcProgress  (gp+0x944) */
int MEMCARD_ChecksumLoad(int saveBytes, int len)
{
    unsigned int uCrc;
    int iProgress;
    int iChunkEnd;
    int bFinalChunk;
    uCrc = D_8008D8D0;
    iProgress = D_8008D8B0;
    if ((D_8008D8CC & 8) == 0 && D_8008D8B0 + 0x200 < len - 2) {
        iChunkEnd = D_8008D8B0 + 0x200;
        bFinalChunk = 0;
    } else {
        iChunkEnd = len - 2;
        bFinalChunk = 1;
    }
    for (; iProgress < iChunkEnd; iProgress = iProgress + 1)
        uCrc = func_8003D540(uCrc, *(unsigned char *)(saveBytes + iProgress));
    D_8008D8D0 = uCrc;
    D_8008D8B0 = iProgress;
    if (bFinalChunk == 0)
        return 7;
    uCrc = func_8003D540(uCrc, *(unsigned char *)(saveBytes + iProgress));
    uCrc = func_8003D540(uCrc, *(unsigned char *)(saveBytes + iProgress + 1));
    return uCrc != 0;
}
