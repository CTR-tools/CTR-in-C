/*
 * MEMCARD_ChecksumSave @ 0x8003D584  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact match: base score 125, Branch 0, Stack 0, Register 5
 * (register-coloring/scheduling residual only; natural C, no hacks).
 *
 * Synchronous CRC-16 over saveBytes[0..len-3] into g_nMemcardCrc16, two zero-finalizers, then writes the 2-byte big-endian checksum into the last two bytes.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_Checksum.c audited CLEAN vs this asm + Ghidra.
 */
extern unsigned int func_8003D540(unsigned int crc, int b);  /* CRC16 */
int D_8008D8D0 = 0;   /* g_nMemcardCrc16 (gp+0x964) */
void MEMCARD_ChecksumSave(unsigned char *saveBytes, int len)
{
    int uCrc;
    unsigned char *pByte;
    int iLast;
    int iByteIdx;
    uCrc = 0;
    iByteIdx = 0;
    if (0 < len - 2) {
        do {
            pByte = &saveBytes[iByteIdx];
            uCrc = func_8003D540(uCrc, *pByte);
            iByteIdx = iByteIdx + 1;
        } while (iByteIdx < len - 2);
    }
    D_8008D8D0 = uCrc;
    uCrc = func_8003D540(uCrc, 0);
    uCrc = func_8003D540(uCrc, 0);
    saveBytes[iByteIdx] = (unsigned char)(uCrc >> 8);
    iLast = iByteIdx + 1;
    saveBytes[iLast] = (unsigned char)uCrc;
}
