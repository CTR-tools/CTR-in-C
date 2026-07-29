#include "HOWL.h"

// Audio_SetDefaults @ 0x8002DC4C (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH.
// Zeroes the two SoundFadeInput channels and the audio-defaults gp globals (two seeded to 0x80).
// Verified vs Ghidra decomp + h105. Store order follows the retail asm.

struct SoundFadeInput
{
    int unk;
    int desiredVolume;
    int currentVolume;
    int soundID_soundCount;
};

extern struct SoundFadeInput D_800962C4;   // SoundFadeInput[0]
extern struct SoundFadeInput D_800962D4;   // SoundFadeInput[1]

int    D_8008D818 = 0;   // gp+0x8AC
int    D_8008D81C = 0;   // gp+0x8B0
int    D_8008D820 = 0;   // gp+0x8B4
int    D_8008D824 = 0;   // gp+0x8B8
int    D_8008D828 = 0;   // gp+0x8BC
int    D_8008D82C = 0;   // gp+0x8C0
int    D_8008D830 = 0;   // gp+0x8C4
u_char D_8008D834 = 0;   // gp+0x8C8 (byte)
int    D_8008D838 = 0;   // gp+0x8CC

void Audio_SetDefaults(void)
{
    D_800962C4.unk = 0;
    D_800962C4.desiredVolume = 0;
    D_800962C4.currentVolume = 0;
    D_800962C4.soundID_soundCount = 0;

    D_800962D4.unk = 0;
    D_800962D4.desiredVolume = 0;
    D_800962D4.currentVolume = 0;
    D_800962D4.soundID_soundCount = 0;

    D_8008D830 = 0;
    D_8008D818 = 0;
    D_8008D81C = 0;
    D_8008D820 = 0;
    D_8008D824 = 0;
    D_8008D828 = 0x80;
    D_8008D82C = 0x80;
    D_8008D838 = 0;
    D_8008D834 = 0;
}
