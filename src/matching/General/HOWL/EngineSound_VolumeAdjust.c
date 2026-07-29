#include "HOWL.h"

// EngineSound_VolumeAdjust @ 0x8002FC28 (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH — score 5, NOT score-0 (auto-granted <300 single-register tie-break). Behavior byte-EXACT.
// Rate-limit `current` to within `step` of `target`: if |current-target| > step, return target +/- step
// (same side as current); otherwise return current unchanged. Also used for talking-mask volumes. Verified vs asm.
//
// MATCH NOTE — residual 5 (1 register diff): retail computes abs(diff) as `move v0,v1; negu v0,v0`
// (copy diff, then self-negate the copy); my gcc emits `move v0,v1; negu v0,v1` (negates the original
// diff, still live in v1 for the later `diff > 0` test). Both compute -diff identically. gcc value-numbers
// mag==diff and negates the canonical v1; the self-negate form is unforceable from natural C (every abs
// idiom tried — if/else, ternary, `0-mag`, `mag<0` cond — yields negu v0,v1; the permuter plateaus at 5,
// and `current>target` for the sign adds an slt = 470). Pure peephole tie-break.

int EngineSound_VolumeAdjust(int current, int target, int step)
{
    int diff = current - target;
    int mag;

    mag = diff;
    if (diff < 0) mag = -mag;

    if (step < mag)
    {
        if (diff > 0) return target + step;
        return target - step;
    }
    return current;
}
