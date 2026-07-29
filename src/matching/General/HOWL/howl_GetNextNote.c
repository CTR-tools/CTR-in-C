#include "HOWL.h"

// howl_GetNextNote @ 0x80029DCC (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 MATCH against SCUS_944.26.exe (decomp-permuter base score = 0).
//
// Decodes a MIDI-style variable-length quantity from `stream` into *outValue (7 bits per byte,
// high bit = continue) and returns the pointer past the consumed bytes. Used to read CSEQ
// delta-times / values. Pure leaf (no globals, no calls).
//
// Match note: `pCursor` is seeded with `stream` and read/incremented through (uValue = *pCursor;
// pCursor = pCursor + 1) rather than `uValue = *stream; pCursor = stream + 1`. Retail copies stream
// into the cursor register first (`move a2,a0`) and walks it; the stream+1 form drops that copy
// (16 insns vs 17) and scores 145.

byte* howl_GetNextNote(byte* stream, uint* outValue)
{
    uint uValue;
    byte* pCursor;
    byte bByte;

    pCursor = stream;
    uValue = *pCursor;
    pCursor = pCursor + 1;
    if ((uValue & 0x80) != 0)
    {
        uValue = uValue & 0x7f;
        do
        {
            bByte = *pCursor;
            pCursor = pCursor + 1;
            uValue = uValue * 0x80 + (bByte & 0x7f);
        } while ((bByte & 0x80) != 0);
    }
    *outValue = uValue;
    return pCursor;
}
