#include "HOWL.h"

// Voiceline_SetDefaults @ 0x8002D2B0 (SCUS_944.26.exe, 926/NTSC-U).
// Resets the voiceline/wrong-way audio state to defaults, then chains to func_8002E338
// (Music_SetDefaults per in-repo decomp h99). Reversed from asm (Ghidra flapping).

extern void func_8002E338(void);   // Music_SetDefaults (audio sub-defaults)

extern u_char D_8008D9DC;   // g_bWrongWayDirection
extern int    D_8008D9E4;   // g_nFramesDrivingSameDir

short  D_8008D7F0 = 0;   // gp+0x884  unkAudioState
int    D_8008D7F4 = 0;   // gp+0x888  audioStateWord
short  D_8008D7F2 = 0;   // gp+0x886  desiredXA_RaceIntroIndex
short  D_8008D7FA = 0;   // gp+0x88E  nTropyVoiceCount
u_char D_8008D7FC = 0;   // gp+0x890  boolNeedXASeek

void Voiceline_SetDefaults(void)
{
    D_8008D7F0 = 0;
    D_8008D7F4 = 0;
    D_8008D7F2 = 0;
    D_8008D9DC = 0;
    D_8008D9E4 = 0;
    D_8008D7FA = 0;
    D_8008D7FC = 0;
    func_8002E338();
}
