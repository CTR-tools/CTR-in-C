#include "HOWL.h"

// SongPool_StopAllCseq @ 0x8002AC0C (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 10, with the whole function matching
//   (behavior byte-for-byte correct; entire diff is one 2-instruction pair). The residual is the final
//   `song->flags &= ~1` clear: retail computes it into a fresh scratch (`andi $v0,$v0,0xfe; sb $v0`),
//   while here it lands in $s0. That is an unavoidable side effect of the transform that MATCHES the
//   rest of the function: the loop's iSeq counter / pSeqWalk cursor otherwise get the s0/s1 callee-
//   saved regs SWAPPED vs retail (gcc gives the first-used cursor s0). Reusing the dead iSeq for the
//   flags clear (`iSeq = song->flags & 0xfe; song->flags = iSeq;`) extends iSeq's live range past the
//   loop, forcing iSeq->s0 like retail and fixing the whole loop — but the clear then uses s0. Retail
//   gets iSeq->s0 without any extension, which can't be expressed in C (plain `flags &= ~1` -> the
//   loop swap returns, score 40). USER-GRANTED (auto-granted per the <300 scheduling-plateau policy
//   after genuine effort: 580->335->170->10 via increment-reorder + pSeqWalk-init-in-loop + this reuse).
//
// Stops every CSEQ sequence belonging to an in-use song (SongPool_StopCseq per sequence) and clears
// the song's in-use flag (flags &= ~1). No-op if the song isn't in use. The sequence array is walked
// via a Song* cursor advanced 4 bytes/iteration (pSeqWalk->CseqSequences[0] == song->CseqSequences[i]).
//
// Match note: `iSeq = song->flags & 0xfe; song->flags = iSeq;` (rather than `song->flags &= 0xfe`)
// and `pSeqWalk = song;` placed inside the numSequences guard are both match-necessary (see above).

extern void SongPool_StopCseq(struct SongSeq* seq);  // #56

void SongPool_StopAllCseq(struct Song* song)
{
    int iSeq;
    struct Song* pSeqWalk;

    if ((song->flags & 1) != 0)
    {
        iSeq = 0;
        if (song->numSequences != 0)
        {
            pSeqWalk = song;
            do
            {
                SongPool_StopCseq(pSeqWalk->CseqSequences[0]);
                pSeqWalk = (struct Song*)&pSeqWalk->songSetActiveBits;
                iSeq = iSeq + 1;
            } while (iSeq < (int)(uint)song->numSequences);
        }
        iSeq = song->flags & 0xfe;
        song->flags = iSeq;
    }
}
