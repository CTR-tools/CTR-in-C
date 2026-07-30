// CTR_unknownMaybeThunk3 @ 0x80021ea8  (vs SCUS_944.26.exe)
//
// SCORE 0 exact match.
//
// Word-wise OR-composite: OR (byteCount >> 2) words from src into dst.
// dst=a0, src=a1, byteCount=a2. wordCount = byteCount >> 2 (signed sra), then
// a guarded `while(wordCount != 0){ *out++ |= *in++; wordCount--; }`. The
// 32-bit sibling of Thunk2's byte-wise OR merge (e.g. compositing an aligned
// bit-layer / visibility mask onto a buffer a word at a time).

#include "CTR.h"

void CTR_unknownMaybeThunk3(void *dst, void *src, int byteCount)
{
    u32 *out = (u32 *)dst;
    u32 *in = (u32 *)src;
    int wordCount = byteCount >> 2;

    while (wordCount != 0)
    {
        *out++ |= *in++;
        wordCount--;
    }
}
