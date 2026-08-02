/*
 * MEMCARD_CRC16 @ 0x8003D540  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact match: base score 230, Branch 0, Stack 0, Register 14
 * (register-coloring/scheduling residual only; natural C, no hacks).
 *
 * CCITT CRC-16 step (poly 0x11021), folds nextByte's 8 bits into crc MSB-first.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_Checksum.c audited CLEAN vs this asm + Ghidra.
 */
typedef unsigned int uint;
uint MEMCARD_CRC16(uint crc, int nextByte)
{
    uint i;
    uint bitCheck;
    i = 7;
    do {
        bitCheck = crc << 1;
        crc = bitCheck | (nextByte >> i & 1U);
        if ((bitCheck & 0x10000) != 0)
            crc = crc ^ 0x11021;
        i = i - 1;
    } while (-1 < (int)i);
    return crc;
}
