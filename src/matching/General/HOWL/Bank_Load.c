#include "HOWL.h"

// Bank_Load @ 0x800297A0 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 match against SCUS_944.26.exe (926/NTSC-U).
//
// Registers and starts loading an SPU sample bank: if fewer than 8 banks are loaded, stores
// bankID into g_aBanks[count], and if that slot is free (flags&3==0) calls Bank_Alloc; on
// success writes the new bank index to *outBankIndex and increments g_nNumAudioBanks. Returns 1
// on success, 0 otherwise. g_aBanks entries are 8 bytes: bankID@0, flags@2, min@4, max@6.
//
// MATCH NOTES:
//  - `bankID` is `u_short`, not `int`. Ghidra guesses `int` (a 32-bit register param), but retail
//    treats it as 16-bit (it masks with andi 0xffff and stores via sh). Declaring the real 16-bit
//    type makes gcc mask the param on entry, which reproduces retail's prologue register moves
//    (saving bankID into a2 and masking from there) -- the last residual that `int` could not
//    produce. (Contrast Bank_ClearInRange, where widening the param to uint DROPPED a needed mask;
//    here narrowing to u_short ADDS the mask retail actually has.)
//  - The success path avoids re-reading g_nNumAudioBanks: capture `oldCount` first, then store the
//    incremented count and *outBankIndex. Writing `*outBankIndex = count; count = count + 1;`
//    directly makes gcc re-load the global (it can't prove outBankIndex != &g_nNumAudioBanks).
//
// Build note: D_8008D76C (g_nNumAudioBanks) is GP-relative (gp=0x8008CF6C); DEFINED not extern
// so gcc emits %gp_rel (GP_VALUE=0x8008CF6C). D_8008FC2C is absolute (%hi/%lo).

struct Bank
{
    short    bankID;
    u_short  flags;
    u_short  min;
    u_short  max;
};

extern struct Bank D_8008FC2C[];   // g_aBanks

u_char D_8008D76C = 0;   // g_nNumAudioBanks (gp+0x800, lbu/sb)

extern uint func_800292FC(int, int);   // Bank_Alloc(bankID, ptrBank)

uint Bank_Load(u_short bankID, byte* outBankIndex)
{
    uint idx;
    struct Bank* bank;
    u_char oldCount;

    if (D_8008D76C < 8)
    {
        idx = D_8008D76C;
        bank = &D_8008FC2C[idx];
        bank->bankID = (short)bankID;
        if (((bank->flags & 3) == 0) &&
            (func_800292FC(bankID & 0xffff, (int)bank) != 0))
        {
            oldCount = D_8008D76C;
            D_8008D76C = oldCount + 1;
            *outBankIndex = oldCount;
            return 1;
        }
    }
    return 0;
}
