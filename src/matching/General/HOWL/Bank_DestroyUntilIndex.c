#include "HOWL.h"

// Bank_DestroyUntilIndex @ 0x80029870 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 match against SCUS_944.26.exe (926/NTSC-U).
//
// Frees the most recent SPU banks (Bank_DestroyLast) until the top-of-stack bank's bankID equals
// `index`, or none remain. g_aBanks entries are 8 bytes (bankID@0).
//
// MATCH NOTES:
//  - `index` and Bank.bankID are u_short (not short). Retail compares them via lhu + andi 0xffff
//    (unsigned 16-bit); a signed `short` would emit lh + sll/sra sign-extension. (Same 16-bit-type
//    lesson as Bank_Load's bankID.)
//  - g_nNumAudioBanks is read DIRECTLY (no cached `count` local). Caching it lets gcc CSE the guard
//    read with the loop read (one lbu + a register copy); reading the global fresh at each use
//    reproduces retail's re-read (two lbu) and its exact register allocation.
//
// Build note: D_8008D76C (g_nNumAudioBanks) is GP-relative (gp=0x8008CF6C); DEFINED not extern so
// gcc emits %gp_rel (GP_VALUE=0x8008CF6C). D_8008FC2C is absolute (%hi/%lo).

struct Bank
{
    u_short  bankID;
    u_short  flags;
    u_short  min;
    u_short  max;
};

extern struct Bank D_8008FC2C[];   // g_aBanks

u_char D_8008D76C = 0;   // g_nNumAudioBanks (gp+0x800, lbu)

extern void func_80029824(void);   // Bank_DestroyLast

void Bank_DestroyUntilIndex(u_short index)
{
    if (D_8008D76C != 0)
    {
        do
        {
            if (D_8008FC2C[D_8008D76C - 1].bankID == index)
            {
                return;
            }
            func_80029824();
        } while (D_8008D76C != 0);
    }
}
