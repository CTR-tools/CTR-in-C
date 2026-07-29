#include "HOWL.h"

// SongPool_ChangeTempo @ 0x8002A6CC (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Re-derives an active song's playback tempo after a BPM delta. Looks up the song's CseqSongHeader at
// g_pCseqSongData[g_pCseqSongStartOffset[song->id]], reads its base bpm (header+2), adds p2 (the BPM
// delta), stores it into song->bpm, and recomputes song->tempo via
// SongPool_CalculateTempo(60, song->tpqn, bpm) (#50).
//
// Build note: g_pCseqSongStartOffset (gp+0x848) and g_pCseqSongData (gp+0x85C) are GP-relative,
// DEFINED so gcc emits %gp_rel (GP_VALUE=0x8008CF6C).

extern int SongPool_CalculateTempo(int const60, int tpqn, int bpm);  // #50 (func @0x8002A678)

int D_8008D7B4 = 0;   // g_pCseqSongStartOffset (gp+0x848, u_short[] per song id)
int D_8008D7C8 = 0;   // g_pCseqSongData        (gp+0x85C, byte buffer of CseqSongHeaders)

void SongPool_ChangeTempo(struct Song* song, short p2)
{
    short bpm;

    bpm = *(u_short*)(D_8008D7C8 + *(u_short*)(D_8008D7B4 + song->id * 2) + 2) + p2;
    song->bpm = bpm;
    song->tempo = SongPool_CalculateTempo(0x3c, song->tpqn, bpm);
}
