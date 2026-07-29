#include "HOWL.h"

// OtherFX_RecycleMute @ 0x8002E724 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Safely stop the SFX instance recorded in *param_1 (if any) and clear the slot. Verified vs Ghidra + h123.
// OtherFX_Stop1 = func_80028808. (Ghidra DB name "OtherFX_Stop_Safe"; syms926 name OtherFX_RecycleMute.)

extern void func_80028808(int handle);   // OtherFX_Stop1

void OtherFX_RecycleMute(int* param_1)
{
    if (*param_1 != 0)
    {
        func_80028808(*param_1);
        *param_1 = 0;
    }
}
