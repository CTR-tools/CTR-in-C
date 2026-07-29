// SCORE 0 — byte-perfect match vs SCUS_944.26.exe (926/NTSC-U), func @ 0x80028494.
// OtherFX_Play_Echo — like OtherFX_Play but sets the reverb/echo bit (0x1000000)
// in the volume flags when echoFlag is non-zero. Wrapper over OtherFX_Play_LowLevel
// (func_800284D0). The echo color is written as the 3rd call arg (ternary) so gcc
// schedules the a0/a1 arg-masks before the beqz and fills its delay slot with andi a1.

#include "HOWL.h"

extern void func_800284D0(uint soundID, u_char antiSpamMode, u_int volumeFlags);

void OtherFX_Play_Echo(uint soundID, u_char antiSpamMode, int echoFlag)
{
    func_800284D0(soundID & 0xffff, antiSpamMode,
                  echoFlag != 0 ? 0x1ff8080 : 0xff8080);
}
