/*
 * MixRNG_GetValue @ 0x8003EAAC  (vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact: score 0 (Branch/Stack/Register 0).
 * Stateless 16-bit LCG step: (seed*0x6255 + 0x3619) & 0xFFFF.
 *
 * Phase B (saphi game/MixRNG): CLEAN
 */
unsigned int MixRNG_GetValue(int seed)
{
    return seed * 0x6255 + 0x3619 & 0xffff;
}
