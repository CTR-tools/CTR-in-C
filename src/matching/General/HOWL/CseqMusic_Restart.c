#include "HOWL.h"

// CseqMusic_Restart @ 0x80028F34 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best achievable score = 635. Control flow and behavior
//   are correct; the residue is the same permuter-UNREACHABLE register-lifetime artifact as
//   CseqMusic_ChangeVolume: the SongPool_Volume argument is masked INSIDE the loop
//   (volStepRate & 0xff), so retail keeps the raw songID in a saved register and re-derives the
//   masked compare across the loop, an allocation gcc 2.8.1 will not reproduce from any
//   semantically-correct C. The permuter plateaus (only a semantically-broken variant scores
//   lower, and is not deliverable). User-granted exception. Delivered as the most faithful
//   readable form.
//
// Flags a playing CSEQ song to restart and re-fades its volume from 0. Guards on audio
// enabled + valid header (songID < numSongs). Under a critical section, scans the song
// pool; for the in-use slot matching songID, sets flags bit2 (restart) and calls
// SongPool_Volume(song, 0, volStepRate&0xff, 0) so the volume ramps back up.

extern u_char D_8008D05C;                    // boolAudioEnabled
extern struct CseqHeader* D_8008D7B0;        // g_CseqHeader (->numSongs @0x6)
extern struct Song D_80095D84[];             // g_aSongPool[]

extern void func_8002B4D0(void);             // Smart_EnterCriticalSection
extern void func_8002B508(void);             // Smart_ExitCriticalSection
extern void func_8002A9D8(struct Song*, uint, uint, int); // SongPool_Volume

void CseqMusic_Restart(ushort songID, uint volStepRate)
{
    struct Song* song;
    int iSlot;

    if ((D_8008D05C != 0) && (D_8008D7B0 != 0) && (songID < D_8008D7B0->numSongs))
    {
        iSlot = 0;
        func_8002B4D0();
        song = D_80095D84;
        do
        {
            if (((song->flags & 1) != 0) && (song->id == songID))
            {
                song->flags = song->flags | 4;
                func_8002A9D8(song, 0, volStepRate & 0xff, 0);
            }
            iSlot = iSlot + 1;
            song = song + 1;
        } while (iSlot < 2);
        func_8002B508();
    }
}
