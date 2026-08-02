/*
 * RngDeadCoed @ 0x8006C684  (vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact (Branch/Stack 0; residual = 0xDEADC0ED constant-scheduling + register-coloring wall, score 890).
 * The 0xDEADC0ED 64-bit RNG: byte-rotate+add mix of state[0]/state[1], XOR 0xDEADC0ED, writeback.
 *
 * Phase B (saphi game/MixRNG): CLEAN
 */
unsigned int RngDeadCoed(unsigned int *state)
{
    unsigned int state0 = state[0];
    unsigned int state1 = state[1];
    unsigned int shifted1 = state1 >> 8;
    unsigned int shifted0 = (state0 >> 8) | (state1 << 24);
    unsigned int mixed = (shifted1 | ((state0 + shifted1 + (shifted0 >> 8)) << 24)) ^ 0xdeadc0ed;
    state[1] = mixed;
    state[0] = shifted0;
    return mixed;
}
