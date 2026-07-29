#include "HOWL.h"

// SongPool_Volume @ 0x8002A9D8 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Sets a song's volume: always stores song->vol_New (target) and vol_StepRate (fade rate); when
// boolImm != 0 also snaps song->vol_Curr = newVol so the change is immediate rather than ramped.
// Frameless leaf; the vol_StepRate store lands in the jr delay slot.

void SongPool_Volume(struct Song* song, int newVol, int newStep, int boolImm)
{
    if (boolImm != 0)
    {
        song->vol_Curr = (u_char)newVol;
    }
    song->vol_New = (u_char)newVol;
    song->vol_StepRate = (char)newStep;
}
