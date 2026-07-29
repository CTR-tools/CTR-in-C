#include "HOWL.h"

// Bank_DestroyLast @ 0x80029824 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 match against SCUS_944.26.exe (926/NTSC-U).
//
// Frees the most recently loaded SPU sample bank: if any exist, decrements g_nNumAudioBanks and
// Bank_Destroy's g_aBanks[count] (the now-decremented index). Returns 1 if a bank was freed, else 0.
//
// Build note: D_8008D76C (g_nNumAudioBanks) is GP-relative (gp=0x8008CF6C); DEFINED not extern so
// gcc emits %gp_rel (GP_VALUE=0x8008CF6C). D_8008FC2C is absolute (%hi/%lo).

struct Bank
{
    short    bankID;
    u_short  flags;
    u_short  min;
    u_short  max;
};

extern struct Bank D_8008FC2C[];   // g_aBanks

u_char D_8008D76C = 0;   // g_nNumAudioBanks (gp+0x800, lbu/sb)

extern void func_800296C4(int);    // Bank_Destroy(bank)

int Bank_DestroyLast(void)
{
    if (D_8008D76C != 0)
    {
        D_8008D76C = D_8008D76C - 1;
        func_800296C4((int)&D_8008FC2C[D_8008D76C]);
        return 1;
    }
    return 0;
}
