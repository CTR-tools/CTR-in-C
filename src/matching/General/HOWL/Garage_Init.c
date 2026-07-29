#include "HOWL.h"

// Garage_Init @ 0x80030208 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Reset the garage SFX pool: stop all cseq audio, then clear the 8 GarageFX slots to GSP_GONE (3),
// zero volume/LR, and null audioPtr. Verified vs Ghidra decomp + h138. howl_StopAudio = func_8002C8A8.

struct GarageFX
{
    char  gsp_curr;   // 0x0
    char  gsp_prev;   // 0x1
    short volume;     // 0x2
    short LR;         // 0x4
    int   audioPtr;   // 0x8
};

extern void func_8002C8A8(int a0, int a1, int a2);   // howl_StopAudio

extern struct GarageFX D_800962E4[];   // garageSoundPool[8]

void Garage_Init(void)
{
    struct GarageFX* g;
    int i;

    func_8002C8A8(1, 0, 1);

    for (i = 0; i < 8; i++)
    {
        g = &D_800962E4[i];
        g->gsp_curr = 3;
        g->gsp_prev = 3;
        g->volume = 0;
        g->LR = 0;
        g->audioPtr = 0;
    }
}
