#include "HOWL.h"

// PlayWarppadSound @ 0x8002E994 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Thin wrapper: update the warppad's positional SFX (slot D_800962D0, soundID 0x98) for the given
// distance via CalculateVolumeFromDistance (#126, func_8002E84C). Verified vs asm.

extern void func_8002E84C(u_int* slot, int soundID, int distance);   // CalculateVolumeFromDistance

extern u_int D_800962D0;   // warppad SFX slot (live handle)

void PlayWarppadSound(int distance)
{
    func_8002E84C(&D_800962D0, 0x98, distance);
}
