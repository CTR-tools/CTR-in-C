#include "HOWL.h"

// Bank_Destroy @ 0x800296C4 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 match against SCUS_944.26.exe (926/NTSC-U).
//
// Frees one SPU sample bank: clears its SPU-address references via Bank_ClearInRange(min, max),
// and if the bank isn't permanent (flags bit0 == 0) rewinds the SPU allocator to the bank's min
// address (valid because only the last bank is ever destroyed). Then clears the bank's loaded bit
// (flags &= ~2). Bank fields: flags@+2, min@+4, max@+6.
//
// MATCH NOTES:
//  - The `do { ... } while (0)` around the call + rewind is match-necessary: it makes gcc load
//    `flags` into its saved register FIRST (retail's order: flags, min, max), instead of last.
//    Same technique as OtherFX_Modify. Behaviorally a no-op scope.
//  - This match REQUIRES the maspsx load-delay fix: `D_8008D770 = *(u_short*)(bank + 4)` compiles
//    to `lhu $v0,4($s1); sw $v0,0x804($gp)`, and retail (aspsx) inserts an R3000 load-delay `nop`
//    between them. Stock maspsx dropped that nop for gp-relative stores (it assumed the store
//    would $at-expand); the fix makes maspsx emit the nop when a load is followed by a store that
//    becomes a single gp-relative op. Without that fix this function floors at 100.
//
// Build note: D_8008D770 (g_nAudioAllocPtr) is GP-relative (gp=0x8008CF6C); DEFINED not extern so
// gcc emits %gp_rel (GP_VALUE=0x8008CF6C).

extern u_char D_8008D05C;    // boolAudioEnabled

int D_8008D770 = 0;          // g_nAudioAllocPtr (gp+0x804, sw)

extern void func_80029730(int, int);   // Bank_ClearInRange(min, max)

void Bank_Destroy(int bank)
{
    u_short flags;

    if (D_8008D05C != 0)
    {
        flags = *(u_short*)(bank + 2);
        do
        {
            func_80029730(*(u_short*)(bank + 4), *(u_short*)(bank + 6));
            if ((flags & 1) == 0)
            {
                D_8008D770 = *(u_short*)(bank + 4);
            }
        } while (0);
        *(u_short*)(bank + 2) = *(u_short*)(bank + 2) & 0xfffd;
    }
}
