#include "HOWL.h"

// Level_SoundLoopSet @ 0x8002E9C0 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Drive a level's looping SFX slot to a target volume: volume 0 stops + clears the slot; otherwise start
// the sound fresh (OtherFX_Play_LowLevel) when the slot is empty, or update the running instance
// (OtherFX_Modify). flags = (volume & 0xff) << 16 | 0x8080 (no distortion, centered pan). Verified vs asm.

extern void func_80028808(u_int handle);                        // OtherFX_Stop1
extern u_int func_800284D0(u_int soundID, int a1, u_int flags); // OtherFX_Play_LowLevel
extern void func_80028690(u_int handle, u_int flags);           // OtherFX_Modify

void Level_SoundLoopSet(u_int* slot, int soundID, u_int volume)
{
    if (volume == 0)
    {
        if (*slot != 0)
        {
            func_80028808(*slot);
            *slot = 0;
        }
        return;
    }

    if (*slot == 0)
    {
        *slot = func_800284D0(soundID & 0xffff, 0, (volume & 0xff) << 16 | 0x8080);
    }
    else
    {
        func_80028690(*slot, (volume & 0xff) << 16 | 0x8080);
    }
}
