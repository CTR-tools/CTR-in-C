// SCORE 0 — byte-perfect match vs SCUS_944.26.exe (926/NTSC-U), func @ 0x80028BBC.
// SetReverbMode — switch the SPU reverb preset, skipping the work if already in that mode
// (D_8008D760). mode 0..4: mute depth, SpuSetReverb(1), load the preset's SpuReverbAttr
// (SpuSetReverbModeParam) and its L/R depth (SpuSetReverbModeDepth). mode>=5: mute depth +
// SpuSetReverb(0) (reverb off). Updates the mode global.
//
// Build note: reads/writes the GP-relative global D_8008D760 @ gp(0x8008CF6C)+0x7F4;
// score with GP_VALUE=0x8008CF6C. The global must be DEFINED (not extern) so gcc treats it
// as small-data and emits a %gp_rel reloc (extern → %hi/%lo absolute, which does NOT match).

#include "HOWL.h"

u_short D_8008D760 = 0;                        // g_nHowlReverbMode (GP-rel @ gp+0x7F4)
extern struct HowlReverbParam D_80082548[];   // g_aHowlReverbModeParams[]

extern void func_800761B0(int, int);          // SpuSetReverbModeDepth
extern void func_800761E0(int);               // SpuSetReverb
extern void func_80075CD0(void*);             // SpuSetReverbModeParam

void SetReverbMode(uint mode)
{
    uint uVar1;
    struct HowlReverbParam* p;

    uVar1 = mode & 0xffff;
    if (uVar1 >= 5)
    {
        if (D_8008D760 != 5)
        {
            func_800761B0(0, 0);
            func_800761E0(0);
            D_8008D760 = 5;
        }
    }
    else
    {
        if (D_8008D760 != uVar1)
        {
            func_800761B0(0, 0);
            func_800761E0(1);
            p = &D_80082548[uVar1];
            func_80075CD0(p);
            func_800761B0(p->depthLeft, p->depthRight);
            D_8008D760 = mode;
        }
    }
}
