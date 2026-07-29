#include "HOWL.h"

// howl_Disable @ 0x8002AC94 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Consumes/queries the howl-initialized flag: if g_bHowlInitialized is set, clears it (disabling
// audio) and returns 1, else returns 0. Leaf.
//
// Match note: written with an INVERTED early-return (`if (g==0) return 0; g=0; return 1;`) rather than
// `if (g!=0){g=0;return 1;} return 0;`. Retail defaults the return value to 1 in the beq delay slot
// (fall-through = return 1) and clears it to 0 on the g==0 branch; the inverted form makes return-1 the
// fall-through so gcc schedules `li v0,1` into that delay slot. (Natural form scores 220.)
//
// Build note: g_bHowlInitialized is GP-relative (gp+0xF0), DEFINED so gcc emits %gp_rel
// (GP_VALUE=0x8008CF6C).

u_char D_8008D05C = 0;   // g_bHowlInitialized (gp+0xF0)

uint howl_Disable(void)
{
    if (D_8008D05C == 0)
    {
        return 0;
    }
    D_8008D05C = 0;
    return 1;
}
