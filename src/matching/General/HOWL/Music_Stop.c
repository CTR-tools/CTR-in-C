#include "HOWL.h"

// Music_Stop @ 0x8002E4EC (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// If cseq music is playing, stop it and clear the play flag / current-song index. Verified vs Ghidra
// decomp + h115. CseqMusic_Stop = func_800291A0. NOTE: the songID passed is the OLD cseqHighestIndex
// (lhu loads it BEFORE the stores) — the call precedes the two global writes.

extern void func_800291A0(u_short songID);   // CseqMusic_Stop

int D_8008D80C = 0;   // gp+0x8A0 cseqBoolPlay
int D_8008D810 = 0;   // gp+0x8A4 cseqHighestIndex

void Music_Stop(void)
{
    if (D_8008D80C == 0) return;

    func_800291A0(D_8008D810);
    D_8008D80C = 0;
    D_8008D810 = -1;
}
