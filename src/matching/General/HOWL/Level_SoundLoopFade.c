#include "HOWL.h"

// Level_SoundLoopFade @ 0x8002EA44 (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH — score 110, NOT score-0 (auto-granted <300 register-allocation plateau, 2026-07-29).
// Behavior byte-EXACT; do-while-0-free, no hacks. Step a level looping-SFX object's volume one `step`
// toward `target` (clamping on overshoot), record the new target, and push the updated volume to the slot
// via Level_SoundLoopSet (#128, func_8002E9C0). Returns early if already at target. Verified vs asm.
// Fields: targetVol@0x4, curVol@0x8, slot@0xC.
//
// MATCH NOTE — residual 110 (2 reg-diff + 1 delete): retail loads curVol into v0, tests it, then COPIES
// it to v1 (`move v1,v0`, filling the beq delay slot) and does the fade arithmetic in v1; my gcc loads
// curVol straight into v1 and skips the redundant copy (strictly more efficient). Pure gcc register-
// allocation tie-break the original build made differently — the copy is unforceable from natural C
// (the permuter plateaus at 110; its only lower forms need the forbidden variable-reuse hack).

struct SoundLoop
{
    int   unk0;         // 0x0
    int   targetVol;    // 0x4
    int   curVol;       // 0x8
    u_int slot;         // 0xC
};

extern void func_8002E9C0(u_int* slot, int soundID, u_int volume);   // Level_SoundLoopSet

void Level_SoundLoopFade(struct SoundLoop* obj, int soundID, int target, int step)
{
    int cur = obj->curVol;

    if (cur == target) return;

    obj->targetVol = target;
    if (cur < target)
    {
        int n = cur + step;
        obj->curVol = n;
        if (target < n) obj->curVol = target;
    }
    else if (target < cur)
    {
        int n = cur - step;
        obj->curVol = n;
        if (n < target) obj->curVol = target;
    }
    func_8002E9C0(&obj->slot, soundID, obj->curVol);
}
