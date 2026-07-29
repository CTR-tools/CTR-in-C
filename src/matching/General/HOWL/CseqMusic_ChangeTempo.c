#include "HOWL.h"

// CseqMusic_ChangeTempo @ 0x80029008 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 match against SCUS_944.26.exe (926/NTSC-U).
//
// Changes the tempo of the CSEQ song with the given songID: scans the 2 song-pool slots
// and, for an in-use slot whose id matches, calls SongPool_ChangeTempo(song, tempo) inside a
// critical section. Gated on audio enabled, CSEQ header, songID < numSongs.
//
// MATCH NOTE: `idCopy` is a match-necessary temp. Retail masks songID once (for the bounds
//   check), then keeps that masked value alive in a saved register across the loop while the
//   bounds-check register is reused as the song-pool cursor -- one mask + one register copy.
//   Reading `songID` directly in the loop lets gcc 2.8.1 mask straight into the survivor
//   register and drop the copy (a strictly-better allocation that mismatches retail by one
//   `move`). Copying songID into `idCopy` for the loop compare, while the bounds check reads
//   the raw param, reproduces retail's exact mask-once/copy-to-survivor dataflow.

extern u_char D_8008D05C;                    // boolAudioEnabled
extern struct CseqHeader* D_8008D7B0;        // g_CseqHeader (->numSongs @0x6)
extern struct Song D_80095D84[];             // g_aSongPool[]

extern void func_8002B4D0(void);             // Smart_EnterCriticalSection
extern void func_8002B508(void);             // Smart_ExitCriticalSection
extern void func_8002A6CC(struct Song*, int); // SongPool_ChangeTempo

void CseqMusic_ChangeTempo(ushort songID, int tempo)
{
    struct Song* song;
    int iSlot;
    uint idCopy;

    if (D_8008D05C != 0)
    {
        if ((D_8008D7B0 != 0) && (songID < D_8008D7B0->numSongs))
        {
            iSlot = 0;
            func_8002B4D0();
            idCopy = songID;
            song = D_80095D84;
            do
            {
                if (((song->flags & 1) != 0) && (song->id == idCopy))
                {
                    func_8002A6CC(song, tempo);
                }
                iSlot = iSlot + 1;
                song = song + 1;
            } while (iSlot < 2);
            func_8002B508();
        }
    }
}
