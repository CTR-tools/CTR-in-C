#include "HOWL.h"

// howl_ModeGet @ 0x8002B1F0 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Returns the howl audio volume mode (g_bVolumeMode). Leaf.
//
// Build note: g_bVolumeMode (gp+0x860) is GP-relative, DEFINED so gcc emits %gp_rel
// (GP_VALUE=0x8008CF6C). It is a u_char, loaded with lbu.

u_char D_8008D7CC = 0;   // g_bVolumeMode (gp+0x860)

char howl_ModeGet(void)
{
    return D_8008D7CC;
}
