#include "HOWL.h"

// Garage_Leave @ 0x8003074C (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Tear down the garage SFX pan-states on leaving the garage/character-select: set every one of the 8
// slots to GSP_GONE(3) so Garage_LerpFX fades them all out. gcc strength-reduces the backwards loop
// (i = 7 .. 0) to a pointer walking &pool[7] down by 0xC. pool = D_800962E4 (GarageFX, stride 0xC).
// Verified vs disassembly. KEY: hoist the store value into a local (`int gone = 3`) BEFORE the loop so
// gcc materializes the 3 before the loop counter 7 (matching retail's `li a0,3; li v1,7` order) -> 0.

struct GarageFX { u_char gsp_curr; char gsp_prev; short volume; short LR; int audioPtr; };

extern struct GarageFX D_800962E4[];   // garageSoundPool[8]

void Garage_Leave(void)
{
    int i;
    int gone = 3;

    for (i = 7; i >= 0; i--)
        D_800962E4[i].gsp_curr = gone;
}
