#include "HOWL.h"

// Audio_SetState_Safe @ 0x8002D4CC (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH — score 305, NOT score-0. Structure verified against the Ghidra decomp; the residual is
// a pure gcc scheduling/register tie-break, not any behavioral difference.
//
// Sets the high-level audio state only if it changed: on change, calls Voiceline_EmptyFunc (#98, an
// empty stub), Audio_SetState (#100), and records the new state in g_nUnkAudioState.
//
// MATCH NOTE — residual 305: gcc-2.8.1 hoists the `lh g_nUnkAudioState` load to the top of the function
// into a caller-saved scratch ($v0), so no load-delay nop is needed; retail loads it LATE into $a0
// (freed after `move s0,a0`) right before the `beq`, requiring a load-delay `nop`. Pure load-position /
// register scheduling tie-break; behavior is identical. Permuter plateaued at 305 (84 iters); manual
// local-copy / early-return / uint forms all 305. Granted as a scheduling exception.

extern void func_8002D2A8(void);        // Voiceline_EmptyFunc (#98)
extern void func_8002D2F4(int state);   // Audio_SetState (#100)

short D_8008D7F0 = 0;   // gp+0x884 g_nUnkAudioState

void Audio_SetState_Safe(int state)
{
    if (state != (int)D_8008D7F0)
    {
        func_8002D2A8();
        func_8002D2F4(state);
        D_8008D7F0 = (short)state;
    }
}
