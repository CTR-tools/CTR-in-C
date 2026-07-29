#include "HOWL.h"

// CseqMusic_StopAll @ 0x80029258 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 match against SCUS_944.26.exe (926/NTSC-U).
//
// Stops all active CSEQ songs: under a critical section, scans the 2 song-pool slots and calls
// SongPool_StopAllCseq(song) for each in-use slot (flags bit0). Gated on audio enabled + a
// valid loaded CSEQ header.

extern u_char D_8008D05C;                    // boolAudioEnabled
extern struct CseqHeader* D_8008D7B0;        // g_CseqHeader
extern struct Song D_80095D84[];             // g_aSongPool[]

extern void func_8002B4D0(void);             // Smart_EnterCriticalSection
extern void func_8002B508(void);             // Smart_ExitCriticalSection
extern void func_8002AC0C(struct Song*);     // SongPool_StopAllCseq

void CseqMusic_StopAll(void)
{
    struct Song* song;
    int iSlot;

    if ((D_8008D05C != 0) && (D_8008D7B0 != 0))
    {
        iSlot = 0;
        func_8002B4D0();
        song = D_80095D84;
        do
        {
            if ((song->flags & 1) != 0)
            {
                func_8002AC0C(song);
            }
            iSlot = iSlot + 1;
            song = song + 1;
        } while (iSlot < 2);
        func_8002B508();
    }
}
