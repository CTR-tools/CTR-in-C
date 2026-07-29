#include "HOWL.h"

// Music_Adjust @ 0x8002E350 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Adjust the currently-playing cseq music: change tempo if same song, stop if different, and (re)start
// a new song. Verified vs Ghidra decomp + h111. CseqMusic_Stop(#19)/ChangeTempo(#17)/Start(#12).
// KEY CRACKS (1065->760->60->0): (1) `u_short songID` param (16-bit song id, matches the per-use
// `& 0xffff` masking exactly — the mask is applied per use, NOT once up front, so gGT keeps the raw arg
// in s0 and re-masks each block); (2) first block inverted to `if (cseqHighestIndex != id) Stop; else
// tempo` so the Stop branch is the inline fall-through and the tempo-check is the beq-to target.

extern void func_800291A0(u_short songID);                          // CseqMusic_Stop
extern void func_80029008(int songID, int tempo);                  // CseqMusic_ChangeTempo
extern void func_80028C78(u_int songID, int tempo, void* set, u_int bits, int loop);  // CseqMusic_Start

int D_8008D80C = 0;   // gp+0x8A0 cseqBoolPlay
int D_8008D810 = 0;   // gp+0x8A4 cseqHighestIndex
int D_8008D814 = 0;   // gp+0x8A8 cseqTempo

void Music_Adjust(u_short songID, int newTempo, void* set, u_int songSetActiveBits)
{
    if (D_8008D80C != 0)
    {
        if (D_8008D810 != (songID & 0xffff))
        {
            func_800291A0(D_8008D810);
        }
        else if (D_8008D814 != newTempo)
        {
            func_80029008(D_8008D810, newTempo);
            D_8008D814 = newTempo;
        }
    }

    if (D_8008D810 != (songID & 0xffff))
    {
        func_80028C78(songID & 0xffff, newTempo, set, songSetActiveBits, 1);
        D_8008D80C = 1;
        D_8008D810 = songID & 0xffff;
        D_8008D814 = newTempo;
    }
}
