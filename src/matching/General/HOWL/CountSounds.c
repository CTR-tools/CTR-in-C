// SCORE 0 — byte-perfect match vs SCUS_944.26.exe (926/NTSC-U), func @ 0x8002843C.
// Build note: reads/writes the GP-relative global D_8008D75C @ gp(0x8008CF6C)+0x7F0,
// so score with GP_VALUE=0x8008CF6C (patch_gprel resolves the %gp_rel relocation).
//
// CountSounds — rolling sound-instance id: increments the global, skipping 0
// (reserved), so every played sound gets a unique-ish tag it can be stopped by.
// The pre-branch `andi 0xFFFF` in the target proves the u16 (unsigned) compare.

#include "HOWL.h"

u16 D_8008D75C = 0;

int CountSounds(void)
{
    D_8008D75C += 1;
    if (D_8008D75C == 0)
    {
        D_8008D75C = 1;
    }
    return D_8008D75C;
}
