#include "HOWL.h"

// Garage_MoveLR @ 0x80030694 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Reassign the garage SFX pan-states as the menu selection moves to slot `idx`: the current slot becomes
// GSP_CENTER(0), the previous slot (wrap 0->7) GSP_LEFT(1), the next slot (wrap 7->0) GSP_RIGHT(2), all
// others GSP_GONE(3). For a prev/next slot that was previously GONE, seed its LR (0x3C left / 0xC3 right)
// before flipping the state so Garage_LerpFX animates from the correct pan. Lighter sibling of
// Garage_Enter (#139): only touches gsp_curr (+ LR on the GONE->active transition), no volume/audioPtr.
// pool = D_800962E4 (GarageFX, stride 0xC). Verified vs Ghidra + disassembly. KEY: fully-indexed
// D_800962E4[i] (score 0 first compile).

struct GarageFX { u_char gsp_curr; char gsp_prev; short volume; short LR; int audioPtr; };

extern struct GarageFX D_800962E4[];   // garageSoundPool[8]

void Garage_MoveLR(int idx)
{
    int i, prev, next;

    if (idx >= 8) return;
    prev = idx - 1;
    if (prev < 0) prev = 7;
    next = idx + 1;
    if (next >= 8) next = 0;

    for (i = 0; i < 8; i++)
    {
        if (i == idx)
        {
            D_800962E4[i].gsp_curr = 0;
        }
        else if (i == prev)
        {
            if (D_800962E4[i].gsp_curr == 3) D_800962E4[i].LR = 0x3C;
            D_800962E4[i].gsp_curr = 1;
        }
        else if (i == next)
        {
            if (D_800962E4[i].gsp_curr == 3) D_800962E4[i].LR = 0xC3;
            D_800962E4[i].gsp_curr = 2;
        }
        else
        {
            D_800962E4[i].gsp_curr = 3;
        }
    }
}
