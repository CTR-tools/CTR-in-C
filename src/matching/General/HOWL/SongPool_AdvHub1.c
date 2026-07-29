#include "HOWL.h"

// SongPool_AdvHub1 @ 0x8002A9F0 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Sets the target volume of one CSEQ sequence within an active song (used by the AdvHub cross-fade).
// Bounds-checks seqID against the song's CseqSongHeader numSeqs byte; if in range, fetches
// seq = song->CseqSequences[seqID] and sets seq->vol_New = vol, plus seq->vol_Curr = vol immediately
// when boolImm != 0 (snap rather than ramp). Leaf.
//
// Build note: g_pCseqSongStartOffset (gp+0x848) and g_pCseqSongData (gp+0x85C) are GP-relative,
// DEFINED so gcc emits %gp_rel (GP_VALUE=0x8008CF6C).

int D_8008D7B4 = 0;   // g_pCseqSongStartOffset (gp+0x848)
int D_8008D7C8 = 0;   // g_pCseqSongData        (gp+0x85C)

void SongPool_AdvHub1(struct Song* song, int seqID, int vol, int boolImm)
{
    struct SongSeq* seq;

    if (seqID < (int)(uint)*(u_char*)(D_8008D7C8 + *(u_short*)((uint)song->id * 2 + D_8008D7B4) + 1))
    {
        seq = song->CseqSequences[seqID];
        if (boolImm != 0)
        {
            seq->vol_Curr = (u_char)vol;
        }
        seq->vol_New = (u_char)vol;
    }
}
