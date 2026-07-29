// SCORE 0 — byte-perfect match vs SCUS_944.26.exe (926/NTSC-U), func @ 0x80028D64.
// CseqMusic_Pause — pauses all active CSEQ songs: for each of the 2 song-pool slots in use
// (flags&1), set the pause bit (flags|=2), inside a critical section. Gated on audio enabled
// + CSEQ header present.

#include "HOWL.h"

extern u_char D_8008D05C;                    // boolAudioEnabled
extern struct CseqHeader* D_8008D7B0;        // g_CseqHeader
extern struct Song D_80095D84[];             // g_aSongPool[]

extern void func_8002B4D0(void);             // Smart_EnterCriticalSection
extern void func_8002B508(void);             // Smart_ExitCriticalSection

void CseqMusic_Pause(void)
{
    struct Song* song;
    int i;

    if ((D_8008D05C != 0) && (D_8008D7B0 != 0))
    {
        func_8002B4D0();
        i = 0;
        song = D_80095D84;
        do
        {
            if ((song->flags & 1) != 0)
            {
                song->flags = song->flags | 2;
            }
            i = i + 1;
            song = song + 1;
        } while (i < 2);
        func_8002B508();
    }
}
