#include "HOWL.h"

// SongPool_FindFreeChannel @ 0x8002A63C (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Returns the first free slot in the SongSeq pool (g_aSongSeq, 24 entries of 0x1C bytes) — i.e. the
// first whose flags bit 0 (playing) is clear — or NULL if all 24 are in use. Frameless leaf.
//
// Match note: written as `if((flags&1)!=0){seq++;}else{return seq;}` (INVERTED) rather than the
// natural `if((flags&1)==0) return seq; seq++;`. Retail lays out the `return seq` INLINE (fall-through,
// `bnez` on flags&1 skips to the loop-continue out-of-line); the natural form makes gcc branch the
// return out-of-line (`beqz`) instead, which mismatches the block layout (scores 320).

extern struct SongSeq D_800902CC[];   // g_aSongSeq (24 entries, 0x1C bytes each)

struct SongSeq* SongPool_FindFreeChannel(void)
{
    struct SongSeq* seq;
    int i;

    i = 0;
    seq = D_800902CC;
    do
    {
        i = i + 1;
        if ((seq->flags & 1) != 0)
        {
            seq = seq + 1;
        }
        else
        {
            return seq;
        }
    } while (i < 0x18);

    return 0;
}
