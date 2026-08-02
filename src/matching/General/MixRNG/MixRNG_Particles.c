/*
 * MixRNG_Particles @ 0x8003EA6C  (vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact: score 0 (Branch/Stack/Register 0).
 * rand in [0,range): (RngDeadCoed(&gGT->deadcoed_struct) & 0xFFFF) * range >> 16.
 *
 * Phase B (saphi game/MixRNG): CLEAN
 */
extern unsigned int func_8006C684(void *rng);   /* RNG_Random */
extern char *D_8008D2AC;   /* gGT */
int MixRNG_Particles(int range)
{
    unsigned int rngVal;
    rngVal = func_8006C684(D_8008D2AC + 0x252C);
    return (int)((rngVal & 0xffff) * range) >> 0x10;
}
