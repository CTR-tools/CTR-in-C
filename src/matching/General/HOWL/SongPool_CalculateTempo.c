#include "HOWL.h"

// SongPool_CalculateTempo @ 0x8002A678 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Per-tick timing value for a sequenced song: ((tpqn * bpm) / 60) << 16 / const60. tpqn = ticks per
// quarter note, bpm = beats per minute, const60 = the timing constant. Args are read as shorts
// (sign-extended). Leaf; the / const60 traps (break 0x1c00) on a zero divisor (--expand-div guard);
// the / 60 is folded to a magic multiply by gcc.

int SongPool_CalculateTempo(int const60, int tpqn, int bpm)
{
    return ((uint)((int)(short)bpm * (int)(short)tpqn) / 0x3c << 0x10) / (uint)(int)(short)const60;
}
