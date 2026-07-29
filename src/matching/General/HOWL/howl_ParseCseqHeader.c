#include "HOWL.h"

// howl_ParseCseqHeader @ 0x80029AB4 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 MATCH against SCUS_944.26.exe (decomp-permuter base score = 0).
//
// CSEQ-header sibling of howl_ParseHeader. Given the loaded CSEQ music-bank header, derives and
// caches all section base pointers by byte-address accumulation (data begins at header+8):
//   longSamples     = header + 8;                    (0xc-byte long-sample instruments)
//   shortSamples    = + numLongSamples*0xc;          (8-byte short-sample/drum entries)
//   songStartOffset = + numShortSamples*8;           (numSongs short offset table)
//   songData        = + numSongs*2, rounded up to a 4-byte boundary.
//
// Match notes:
//   * The header-pointer store (D_8008D7B0 = cseqHeader) must be the FIRST statement; retail emits
//     `sw $a0,0x844(gp)` right after computing the header+8 accumulator.
//   * The final align-to-4 rounds via two independent checks. The first (`if (p & 1)`) stores
//     `p + 1` WITHOUT reassigning p, so gcc computes the +1 into a scratch reg and leaves the
//     accumulator alone (retail's `addiu $v0,$a1,1`) — writing `p = p + 1` instead does the add
//     in-place (`addiu $a1,$a1,1`) and regresses to 10. The second check reads the GLOBAL directly
//     (D_8008D7C8, not p) to reproduce retail's re-read (`lw $v1,0x85C(gp)`) after the merge.
//
// Build note: the 5 cached pointers are GP-relative (gp=0x8008CF6C); DEFINED (not extern) so gcc
// emits %gp_rel (GP_VALUE=0x8008CF6C). Stored as int (byte addresses).

int D_8008D7B0 = 0;   // g_pCseqHeader          (gp+0x844)
int D_8008D7B4 = 0;   // g_pCseqSongStartOffset (gp+0x848)
int D_8008D7C4 = 0;   // g_pCseqShortSamples    (gp+0x858)
int D_8008D7C8 = 0;   // g_pCseqSongData        (gp+0x85C)
int D_8008D7E8 = 0;   // g_pCseqLongSamples     (gp+0x87C)

void howl_ParseCseqHeader(struct CseqHeader* cseqHeader)
{
    int p;

    D_8008D7B0 = (int)cseqHeader;
    p = (int)cseqHeader + 8;
    D_8008D7E8 = p;
    p = p + cseqHeader->numLongSamples * 0xc;
    D_8008D7C4 = p;
    p = p + cseqHeader->numShortSamples * 8;
    D_8008D7B4 = p;
    p = p + cseqHeader->numSongs * 2;
    D_8008D7C8 = p;
    if (p & 1)
    {
        D_8008D7C8 = p + 1;
    }
    if (D_8008D7C8 & 2)
    {
        D_8008D7C8 = D_8008D7C8 + 2;
    }
}
