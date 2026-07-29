#include "HOWL.h"

// CseqMusic_ChangeVolume @ 0x80028E5C (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best achievable score = 635. Control flow and behavior
//   are correct; the residue is a permuter-UNREACHABLE register-lifetime artifact of the
//   songID-scan under high register pressure. Because the call arguments are masked INSIDE the
//   loop (newVol & 0xff, newStep & 0xff), retail keeps the raw songID in a saved register and
//   re-derives the masked compare value across the loop, an allocation gcc 2.8.1 will not
//   reproduce from any semantically-correct C. Six distinct C forms (uint+mask, ushort+mask,
//   copy, implicit-truncate, separate-bound, per-iteration remask) all scored 635; the permuter
//   only ever reached 310 via a SEMANTICALLY-BROKEN variant (iSlot = songID), which is not
//   deliverable. User-granted exception. Delivered as the most faithful readable form.
//
// Sets the volume of the CSEQ song matching songID across the 2 song-pool slots
// (SongPool_Volume), inside a critical section. Gated on audio enabled, CSEQ header,
// songID < numSongs.

extern u_char D_8008D05C;                    // boolAudioEnabled
extern struct CseqHeader* D_8008D7B0;        // g_CseqHeader (->numSongs @0x6)
extern struct Song D_80095D84[];             // g_aSongPool[]

extern void func_8002B4D0(void);             // Smart_EnterCriticalSection
extern void func_8002B508(void);             // Smart_ExitCriticalSection
extern void func_8002A9D8(struct Song*, uint, uint, int); // SongPool_Volume

void CseqMusic_ChangeVolume(ushort songID, uint newVol, uint newStep)
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
                func_8002A9D8(song, newVol & 0xff, newStep & 0xff, 0);
            }
            iSlot = iSlot + 1;
            song = song + 1;
        } while (iSlot < 2);
        func_8002B508();
    }
}
