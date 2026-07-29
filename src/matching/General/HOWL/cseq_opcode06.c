#include "HOWL.h"

// cseq_opcode06 @ 0x8002A3A8 (SCUS_944.26.exe, 926/NTSC-U). (Ghidra: cseq_opcode_Volume)
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// CSEQ "set sequence volume" opcode: stores the note operand into the sequence's current volume
// (vol_Curr = currNote[1]), then runs the shared 06/07 post-processing (cseq_opcode_from06and07,
// #42, func_8002A170) which re-scales every live music channel playing this sequence.
//
// Tail-call form: the vol_Curr store lands in the jal delay slot in retail.

extern void func_8002A170(struct SongSeq* seq);  // cseq_opcode_from06and07 (#42)

void cseq_opcode06(struct SongSeq* songSeq)
{
    songSeq->vol_Curr = songSeq->currNote[1];
    func_8002A170(songSeq);
}
