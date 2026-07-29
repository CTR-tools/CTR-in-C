#include "HOWL.h"

// cseq_opcode07 @ 0x8002A3D4 (SCUS_944.26.exe, 926/NTSC-U). (Ghidra: cseq_opcode_Pan)
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// CSEQ "set sequence pan" opcode (parallel to cseq_opcode06): stores the note operand into the
// sequence's L/R pan (LR = currNote[1]), then runs the shared 06/07 post-processing
// (cseq_opcode_from06and07, #42, func_8002A170). Tail-call form: the LR store lands in the jal
// delay slot in retail.

extern void func_8002A170(struct SongSeq* seq);  // cseq_opcode_from06and07 (#42)

void cseq_opcode07(struct SongSeq* songSeq)
{
    songSeq->LR = songSeq->currNote[1];
    func_8002A170(songSeq);
}
