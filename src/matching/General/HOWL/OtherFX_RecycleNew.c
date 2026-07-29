#include "HOWL.h"

// OtherFX_RecycleNew @ 0x8002E690 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Maintains a single looping/continuous SFX slot whose live handle is in *soundID_Count. If a different
// sound is playing there ((handle&0xffff)!=newSoundID), stop it (OtherFX_Stop1) and clear the slot.
// Then if newSoundID!=-1: start it fresh via OtherFX_Play_LowLevel (store the handle) when empty, else
// OtherFX_Modify the existing instance. Verified vs Ghidra + h122.

extern void func_80028808(u_int handle);                             // OtherFX_Stop1
extern u_int func_800284D0(u_int soundID, int a1, u_int modifyFlags); // OtherFX_Play_LowLevel
extern void func_80028690(u_int handle, u_int modifyFlags);          // OtherFX_Modify

void OtherFX_RecycleNew(u_int* soundID_Count, u_int newSoundID, u_int modifyFlags)
{
    u_int uVar1;

    uVar1 = *soundID_Count;
    if (uVar1 != 0 && (uVar1 & 0xffff) != newSoundID)
    {
        func_80028808(uVar1);
        *soundID_Count = 0;
    }
    if (newSoundID != 0xffffffff)
    {
        if (*soundID_Count == 0)
        {
            uVar1 = func_800284D0(newSoundID & 0xffff, 0, modifyFlags);
            *soundID_Count = uVar1;
        }
        else
        {
            func_80028690(*soundID_Count, modifyFlags);
        }
    }
}
