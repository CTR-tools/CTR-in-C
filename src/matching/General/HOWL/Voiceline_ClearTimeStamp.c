#include "HOWL.h"

// Voiceline_ClearTimeStamp @ 0x8002CAA8 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH (perfect).
//
// Zeroes the two 16-entry audio timestamp arrays (timeSet1 @D_80096244, timeSet2 @D_80096284).
// `int index` (not char) so gcc strength-reduces both array walks to walking pointers, matching retail.
//
// ABS extern: timeSet1=D_80096244(int[16]), timeSet2=D_80096284(int[16]).

extern int D_80096244[];   // timeSet1
extern int D_80096284[];   // timeSet2

void Voiceline_ClearTimeStamp(void)
{
    int index;

    for (index = 0; index < 16; index++)
    {
        D_80096244[index] = 0;
        D_80096284[index] = 0;
    }
}
