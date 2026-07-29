#include "HOWL.h"

// Bank_ResetAllocator @ 0x800292E0 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 match against SCUS_944.26.exe (926/NTSC-U).
//
// Resets the SPU sample-bank allocator: allocation pointer -> 0x202 (start of usable SPU RAM,
// past the 0x1000-byte reverb/capture work area), bank count -> 0, load stage -> 4 (idle).
//
// Build note: all three globals are GP-relative (gp = 0x8008CF6C, 926/NTSC-U). They must be
// DEFINED (not extern) so gcc emits %gp_rel relocations; patch_gprel resolves each from its
// D_<abs_addr> name (score with GP_VALUE=0x8008CF6C). Widths follow the store opcodes:
// D_8008D770 is a word (sw), the other two are bytes (sb).

int    D_8008D770 = 0;   // g_nAudioAllocPtr  (gp+0x804, sw)
u_char D_8008D76C = 0;   // g_nNumAudioBanks  (gp+0x800, sb)
u_char D_8008D774 = 0;   // g_nBankLoadStage  (gp+0x808, sb)

void Bank_ResetAllocator(void)
{
    D_8008D770 = 0x202;
    D_8008D76C = 0;
    D_8008D774 = 4;
}
