#include "HOWL.h"

// OtherFX_DriverCrashing @ 0x8002E760 (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH — score 485, NOT score-0 (USER-GRANTED exception, 2026-07-28). Behavior byte-EXACT.
// Play a vehicle-collision SFX; pick a crash sound by volume: id 10 if volume>=221, id 12 if
// volume>160, else id 11. flags = boolEcho<<24 | (volume&0xff)<<16 | 0x8080 (no distortion, centered
// pan). Verified vs Ghidra + h124. OtherFX_Play_LowLevel = func_800284D0.
//
// MATCH NOTE — residual 485 (5 reg-diff, 1 reorder, 1 insert, 3 delete; SAME instruction count):
// pure gcc ARGUMENT-EVALUATION ORDER. Retail evaluates the call args a0->a1->a2, so it sets a1=0
// EARLY (at the soundID convergence, before the flags math) and therefore must preserve volume by
// copying it to v1 at entry (`move v1,a1`) + fills the first branch's delay slot with the boolEcho
// save (`move a2,a0`). My gcc computes the a2 flags expression FIRST (volume still in a1), sets a1=0
// LATE (right before the jal), and saves boolEcho before the branch. Both are byte-equivalent; the
// choice is a pure scheduling/coloring tie-break the permuter can't reach (needs a coordinated
// volume->v1 + early-a1=0 mutation). Permuter plateaued at 485 over 41 iters; 6 manual structural
// variants (invert/non-invert, flags-early, flags-reorder, char-arg, signed-params, vol-local) all >=485.

extern u_int func_800284D0(u_int soundID, int a1, u_int flags);   // OtherFX_Play_LowLevel

void OtherFX_DriverCrashing(u_int boolEcho, u_int volume)
{
    u_int soundID;

    if (0xdc < volume) {
        soundID = 10;
    } else {
        soundID = 0xb;
        if (0xa0 < volume) soundID = 0xc;
    }
    func_800284D0(soundID, 0, boolEcho << 0x18 | (volume & 0xff) << 0x10 | 0x8080);
}
