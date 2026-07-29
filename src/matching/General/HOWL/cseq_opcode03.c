#include "HOWL.h"

// cseq_opcode03 @ 0x80029F24 (SCUS_944.26.exe, 926/NTSC-U). (Ghidra: howl_StopChannelCseq)
// SCORE-0 MATCH against SCUS_944.26.exe (decomp-permuter base score = 0).
//
// CSEQ opcode (table idx 3): if the sequence's flag bit1 is clear, stops the CSEQ for its song-pool
// entry (SongPool_StopAllCseq(&g_aSongPool[seq->songPoolIndex])); otherwise sets the 'stopping'
// flag bit3 (flags |= 8).
//
// Match note: the `if ((flags & 2) != 0) {flags|=8} else {StopAllCseq}` polarity (flags|=8 inline,
// StopAllCseq out-of-line) matches the target's `beqz v0, <StopAllCseq>`; the reference
// `if ((flags & 2) == 0)` would invert the branch.
//
// Build note: g_aSongPool is absolute (%hi/%lo), left extern. No gp-relative globals here.

extern void func_8002AC0C(struct Song* song);   // SongPool_StopAllCseq
extern struct Song D_80095D84[];                 // g_aSongPool (struct Song, 0x7C each)

void cseq_opcode03(struct SongSeq* seq)
{
    if ((seq->flags & 2) != 0)
    {
        seq->flags |= 8;
    }
    else
    {
        func_8002AC0C(&D_80095D84[seq->songPoolIndex]);
    }
}
