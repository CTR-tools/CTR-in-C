/*
 * MixRNG_Scramble @ 0x8003EA28  (vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact: score 0 (Branch/Stack/Register 0).
 * Advances + returns the global 16-bit LCG g_nRandomNumber = (n*0x6255 + 0x3619) & 0xFFFF.
 *
 * Phase B (saphi game/MixRNG): CLEAN
 */
int D_8008D424 = 0;   /* g_nRandomNumber (gp+0x4B8) */
int MixRNG_Scramble(void)
{
    D_8008D424 = D_8008D424 * 0x6255 + 0x3619 & 0xffff;
    return D_8008D424;
}
