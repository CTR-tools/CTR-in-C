#include "HOWL.h"

// Bank_DestroyAll @ 0x800298E4 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 match against SCUS_944.26.exe (926/NTSC-U).
//
// Frees all loaded SPU sample banks by repeatedly calling Bank_DestroyLast until none remain.
//
// Build note: D_8008D76C (g_nNumAudioBanks) is GP-relative (gp=0x8008CF6C); DEFINED not extern so
// gcc emits %gp_rel (GP_VALUE=0x8008CF6C).

u_char D_8008D76C = 0;   // g_nNumAudioBanks (gp+0x800, lbu)

extern void func_80029824(void);   // Bank_DestroyLast

void Bank_DestroyAll(void)
{
    while (D_8008D76C != 0)
    {
        func_80029824();
    }
}
