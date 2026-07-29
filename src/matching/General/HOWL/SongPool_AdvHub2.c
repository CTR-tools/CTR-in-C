#include "HOWL.h"

// SongPool_AdvHub2 @ 0x8002AA44 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Applies a SongSet's active-sequence mask to an already-playing song (Adventure-hub cross-fade).
// Looks up the song's CseqSongHeader numSeqs byte; if songSet != NULL it must match (else abort),
// then latches song->songSetActiveBits. For each sequence i in [0,numSeqs): vol =
// (songSet->ptrSongSetBits[i] & song->songSetActiveBits) ? 0xff : 0, then SongPool_AdvHub1(song, i,
// vol, 0) ramps each channel toward on/off.
//
// Build note: g_pCseqSongStartOffset (gp+0x848) / g_pCseqSongData (gp+0x85C) are GP-relative, DEFINED
// so gcc emits %gp_rel (GP_VALUE=0x8008CF6C).
//
// Match note: the per-sequence vol is applied as TWO distinct SongPool_AdvHub1 calls (one with 0xff,
// one with 0) rather than computing a `vol` local and passing it once. gcc cross-jumps the two calls
// into a single `jal` with the differing 0xff/0 set per-branch (retail's `j`-converge shape) — and
// that call structure also drives the song/iVar1 callee-saved register assignment to match. Folding
// them into one call with a `vol` temp collapses the branch and swaps song<->iVar1 (s1/s2), = 280.

extern void SongPool_AdvHub1(struct Song* song, int seqID, int vol, int boolImm);  // #54

int D_8008D7B4 = 0;   // g_pCseqSongStartOffset (gp+0x848)
int D_8008D7C8 = 0;   // g_pCseqSongData        (gp+0x85C)

void SongPool_AdvHub2(struct Song* song, struct SongSet* songSet, int songSetActiveBits)
{
    int seqID;
    int iVar1;

    iVar1 = D_8008D7C8 + *(u_short*)((uint)song->id * 2 + D_8008D7B4);
    if (songSet != 0)
    {
        if (songSet->numSeqs != (uint)*(u_char*)(iVar1 + 1))
        {
            return;
        }
        song->songSetActiveBits = songSetActiveBits;
    }
    seqID = 0;
    if (*(u_char*)(iVar1 + 1) != 0)
    {
        do
        {
            if (((uint)songSet->ptrSongSetBits[seqID] & song->songSetActiveBits) != 0)
            {
                SongPool_AdvHub1(song, seqID, 0xff, 0);
            }
            else
            {
                SongPool_AdvHub1(song, seqID, 0, 0);
            }
            seqID = seqID + 1;
        } while (seqID < (int)(uint)*(u_char*)(iVar1 + 1));

    }
}
