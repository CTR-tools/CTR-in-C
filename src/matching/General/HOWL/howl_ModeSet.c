#include "HOWL.h"

// howl_ModeSet @ 0x8002B1FC (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Sets the howl audio volume mode (g_bVolumeMode = volumeMode). Leaf.
//
// Build note: g_bVolumeMode (gp+0x860) is GP-relative, DEFINED so gcc emits %gp_rel
// (GP_VALUE=0x8008CF6C). The store is a single sb.

u_char D_8008D7CC = 0;   // g_bVolumeMode (gp+0x860)

void howl_ModeSet(char volumeMode)
{
    D_8008D7CC = volumeMode;
}
