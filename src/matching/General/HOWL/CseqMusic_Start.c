// SCORE 0 — byte-perfect match vs SCUS_944.26.exe (926/NTSC-U), func @ 0x80028C78.
// CseqMusic_Start — start a CSEQ song in the first free song-pool slot via SongPool_Start;
// returns 1 on success, 0 if audio disabled / no CSEQ header / songID >= numSongs / no free slot.
//
// Match notes: (1) the header/bounds-fail path uses `return 0` (a literal-0 return, distinct
// from the `return uResult` used by the audio-fail/no-slot paths). (2) `tempo` is declared `int`
// (not the semantic `short`) — a short that is only forwarded to another call would otherwise be
// sign-extended (sll/sra) at the call site; `int` passes the already-extended value as-is and also
// yields the target's saved-register assignment.

#include "HOWL.h"

extern u_char D_8008D05C;                    // boolAudioEnabled
extern struct CseqHeader* D_8008D7B0;        // g_CseqHeader (->numSongs @0x6)
extern struct Song D_80095D84[];             // g_aSongPool[]

extern void func_8002B4D0(void);             // Smart_EnterCriticalSection
extern void func_8002B508(void);             // Smart_ExitCriticalSection
extern void func_8002A730(struct Song*, uint, int, int, void*, int); // SongPool_Start

uint CseqMusic_Start(ushort songID, int tempo, void* songSet, int songSetActiveBits, int boolLoopAtEnd)
{
    int iSlot;
    struct Song* song;
    uint uResult;

    uResult = 0;
    if (D_8008D05C != 0)
    {
        if ((D_8008D7B0 == 0) || (D_8008D7B0->numSongs <= (songID & 0xffff)))
        {
            return 0;
        }
        func_8002B4D0();
        iSlot = 0;
        song = D_80095D84;
        do
        {
            iSlot = iSlot + 1;
            if ((song->flags & 1) == 0)
            {
                func_8002A730(song, songID & 0xffff, tempo, boolLoopAtEnd, songSet, songSetActiveBits);
                uResult = 1;
                break;
            }
            song = song + 1;
        } while (iSlot < 2);
        func_8002B508();
    }
    return uResult;
}
