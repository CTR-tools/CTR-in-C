// SCORE 0 — byte-perfect match vs SCUS_944.26.exe (926/NTSC-U), func @ 0x80028468.
// OtherFX_Play — plays a non-engine sound effect by id at the given anti-spam
// mode, with a default neutral volume color 0xFF8080. Thin wrapper over
// OtherFX_Play_LowLevel (func_800284D0 @ 0x800284D0; kept as its address-name so
// the jal symbol matches the target object).

#include "HOWL.h"

extern void func_800284D0(uint soundID, u_char antiSpamMode, int volumeColor);

void OtherFX_Play(u_short soundID, u_int antiSpamMode)
{
    func_800284D0((uint)soundID, (u_char)antiSpamMode, 0xff8080);
}
