#include "HOWL.h"

// cseq_opcode09 @ 0x8002A494 (SCUS_944.26.exe, 926/NTSC-U). (Ghidra: howl_SetSoundInstrument)
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// CSEQ "set sound instrument" opcode (table index 9): stores the note operand into the sequence's
// instrument selector (instrumentID = currNote[1]) — the long-sample index later consumed by the
// pitch opcode via g_pCseqLongSamples[instrumentID]. Frameless leaf; store lands in the jr delay slot.

void cseq_opcode09(struct SongSeq* seq)
{
    seq->instrumentID = seq->currNote[1];
}
