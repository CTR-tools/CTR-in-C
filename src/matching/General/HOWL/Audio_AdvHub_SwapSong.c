#include "HOWL.h"

// Audio_AdvHub_SwapSong @ 0x8002D50C (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH.
// On an Adventure-arena level (levelID 0x19..0x1D) while in audio state 5, swap the active hub song
// via CseqMusic_AdvHubSwap (#18). Verified vs Ghidra decomp + h102.

extern void func_800290CC(int songID, void* songSet, int activeBits);  // CseqMusic_AdvHubSwap (#18)

extern int D_8008D068;   // g_advHubSongSet (passed by address)

short D_8008D7F0 = 0;     // gp+0x884 g_nUnkAudioState

void Audio_AdvHub_SwapSong(int levelID)
{
    if (D_8008D7F0 == 5 && (u_int)(levelID - 0x19) < 5)
        func_800290CC(0, &D_8008D068, 1 << (levelID - 0x19));
}
