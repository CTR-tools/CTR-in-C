#include "HOWL.h"

// UpdateChannelVol_EngineFX @ 0x8002ACB8 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Sets one SPU channel's volume for an engine-FX sound: vol = (g_bVolEngineFX * engineFX->volume *
// param_3) >> 10, applied via Channel_SetVolume(attr, vol, pan). param_1 is the EngineFX pointer (its
// volume byte is at +1). Leaf wrapper.
//
// Build note: g_bVolEngineFX is GP-relative (gp+0x840), DEFINED so gcc emits %gp_rel
// (GP_VALUE=0x8008CF6C). Callee Channel_SetVolume (func_8002B540).

extern void func_8002B540(struct ChannelAttr* attr, uint vol, uint pan);  // Channel_SetVolume

u_char D_8008D7AC = 0;   // g_bVolEngineFX (gp+0x840)

void UpdateChannelVol_EngineFX(int param_1, struct ChannelAttr* param_2, int param_3, int param_4)
{
    func_8002B540(param_2,
                  (uint)D_8008D7AC * (uint)*(u_char*)(param_1 + 1) * param_3 >> 10,
                  param_4);
}
