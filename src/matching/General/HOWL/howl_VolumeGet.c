#include "HOWL.h"

// howl_VolumeGet @ 0x8002B0E0 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Returns the current howl volume byte for a volume type: 0 = EngineFX (g_bVolEngineFX), 1 = Music
// (g_bVolMusic), 2 = OtherFX (g_bVolOtherFX); 0 for any other type. Leaf.
//
// Match note: written as an explicit goto-dispatch (not if/else-if or switch). Retail lays each case out
// as a SEPARATE labeled block reached by a forward `beq`, with the default (return 0) routed via `j` to a
// shared trailing `jr ra`. A nested-if form makes gcc INLINE the first return (score 1130); a `switch`
// adds an `slti` range check (465); the goto form with `return 0` placed BEFORE the labels inlines the
// default's `jr` (260). Placing the default as the LAST block (`goto def; ... def: return 0;`) is
// match-necessary — it forces the trailing shared return block retail emits. Case labels are ordered
// engine/music/other to match retail's block addresses.
//
// Build note: g_bVolEngineFX (gp+0x840) / g_bVolMusic (gp+0x84C) / g_bVolOtherFX (gp+0x850) are
// GP-relative, DEFINED so gcc emits %gp_rel (GP_VALUE=0x8008CF6C). Each is a u_char loaded with lbu.

u_char D_8008D7AC = 0;   // g_bVolEngineFX (gp+0x840)
u_char D_8008D7B8 = 0;   // g_bVolMusic    (gp+0x84C)
u_char D_8008D7BC = 0;   // g_bVolOtherFX  (gp+0x850)

short howl_VolumeGet(int volumeType)
{
    if (volumeType == 1) goto music;
    if (volumeType == 0) goto engine;
    if (volumeType == 2) goto other;
    goto def;
engine:
    return (short)D_8008D7AC;
music:
    return (short)D_8008D7B8;
other:
    return (short)D_8008D7BC;
def:
    return 0;
}
