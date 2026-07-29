#include "HOWL.h"

// Level_RandomFX @ 0x8002EAB8 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Random-interval level ambience trigger: counts *counter down each call; when it hits 0, plays a
// random-volume one-shot (OtherFX_Play_LowLevel) and re-arms *counter to base + rand%range. Uses the
// inlined LCG on g_rngSeed (D_8008D058): seed = ror(seed,3)*5 + 1, expressed as ((seed>>3)+(seed<<29))*5+1.
// Flags = (((seed%100 + 100)*volume >> 8) & 0xff) << 16 | 0x01008080. Verified vs asm.
// KEY: operate on the global D_8008D058 IN PLACE (the inlined Random() idiom) — that reproduces the
// retail register allocation exactly (reading into a local `seed` first floors at 85 on a v0/a0 coloring
// tie-break). `%100` is gcc's magic-multiply; `%range` is a divu (maspsx --expand-div).

extern u_int func_800284D0(u_int soundID, int a1, u_int flags);   // OtherFX_Play_LowLevel

extern u_int D_8008D058;   // g_rngSeed (LCG state)

void Level_RandomFX(int* counter, int soundID, int base, int range, int volume)
{
    if (0 < *counter) (*counter)--;
    if (*counter != 0) return;

    D_8008D058 = ((D_8008D058 >> 3) + (D_8008D058 << 29)) * 5 + 1;
    func_800284D0(soundID & 0xffff, 0,
                  ((u_int)(((int)(D_8008D058 % 100) + 100) * volume) >> 8 & 0xff) << 16 | 0x01008080);

    D_8008D058 = ((D_8008D058 >> 3) + (D_8008D058 << 29)) * 5 + 1;
    *counter = D_8008D058 % range + base;
}
