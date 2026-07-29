#include "HOWL.h"

// CalculateVolumeFromDistance @ 0x8002E84C (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH — score 120, NOT score-0 (auto-granted <300 scheduling plateau, 2026-07-29). Behavior
// byte-EXACT; PERFECT register allocation (0 reg diffs); do-while-0-free, no hacks.
// Distance-attenuated positional-SFX manager for the slot *slot (a live handle). If too far (>=6000):
// stop + clear. Otherwise stop a stale different sound, compute a volume (max 0xFF below 301, else a
// 300..6000 -> 0xFF..0 lerp via func_80058F9C), and for a live soundID: start it fresh when the slot is
// empty (OtherFX_Play_LowLevel) or modify the running instance (OtherFX_Modify). soundID 0x89 is the
// special engine sound: its pan byte oscillates from a gGT field. Verified vs asm (Ghidra was down).
//
// CRACKS (920 -> 120): (1) cache `cur = *slot` for the final block so the reload lands in a0 (the
// Modify arg) — matches retail's single `lw a0`; (2) per-path `flags = low<<16; flags |= ...; flags |= ..`
// step-splits so the OR chain stays in a1; (3) invert the volume `if` (`if (0x12C < distance) lerp; else
// 0xFF`) so the max block lays out after the lerp with a `j` over it, matching retail's block order;
// (4) hoist `flags = low<<16` above the pan and extract `field>>2` into a temp so the sll schedules near
// the field load. RESIDUAL 120 (2 reorderings only): the `sll a1,a1,16` (low<<16) fills the field-load
// delay slot in retail but my gcc parks it in the bgez delay slot (a nop then swaps in). Pure gcc
// list-scheduler delay-slot tie-break — the ONLY sub-120 form the permuter finds reuses `low` for the
// field shift (a forbidden variable-reuse hack); every clean variant floors at 120.

extern void func_80028808(u_int handle);                             // OtherFX_Stop1
extern u_int func_800284D0(u_int soundID, int a1, u_int flags);      // OtherFX_Play_LowLevel
extern void func_80028690(u_int handle, u_int flags);               // OtherFX_Modify
extern int func_80058F9C(int val, int inMin, int inMax, int outMax, int a4);  // volume lerp

extern char* D_8008D2AC;   // sdata->gGT

void CalculateVolumeFromDistance(u_int* slot, int soundID, int distance)
{
    int volume;
    u_int flags, cur, low;

    if (distance >= 0x1770)
    {
        if (*slot != 0) { func_80028808(*slot); *slot = 0; }
        return;
    }
    if (*slot != 0 && (*slot & 0xffff) != soundID) { func_80028808(*slot); *slot = 0; }
    if (0x12C < distance)
        volume = func_80058F9C(distance, 0x12C, 0x1770, 0xFF, 0);
    else
        volume = 0xFF;
    if (soundID == -1) return;
    cur = *slot;
    if (cur == 0)
    {
        *slot = func_800284D0(soundID & 0xffff, 0, (volume & 0xff) << 16 | 0x8080);
    }
    else
    {
        low = volume & 0xff;
        if (soundID == 0x89)
        {
            u_int shifted = *(u_int*)(D_8008D2AC + 0x1CE4) >> 2;
            int pan = (shifted & 0x7F) - 0x40;
            flags = low << 16;
            if (pan < 0) pan = -pan;
            flags = flags | ((pan + 0x64) & 0xff) << 8;
            flags = flags | 0x80;
        }
        else
        {
            flags = low << 16;
            flags = flags | 0x8080;
        }
        func_80028690(cur, flags);
    }
}
