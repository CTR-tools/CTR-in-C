#include "HOWL.h"

// Smart_EnterCriticalSection @ 0x8002B4D0 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Nesting-aware critical-section enter for the audio system: increments g_nCriticalSectionCount and,
// on the 0->1 transition, calls the real EnterCriticalSection (disables IRQs). (Binary authoritative:
// the in-repo decomp is a rewrite that only bumps the counter; the 926 binary calls EnterCriticalSection.)
//
// Build note: g_nCriticalSectionCount (gp+0x820) is GP-relative int, DEFINED so gcc emits %gp_rel
// (GP_VALUE=0x8008CF6C). Callee func_80076990 (EnterCriticalSection, PSY-Q libapi).
//
// Match note: `oldCount = count;` (a raw snapshot) BEFORE the in-place `count += 1;` is match-necessary.
// Retail loads the count into $v0, COPIES it to $v1, then increments $v0 in place (`move v1,v0; addiu
// v0,v0,1`); the raw copy + in-place `+=` reproduces that. A plain `store = oldCount + 1` computes the
// new value out-of-place, letting gcc drop the copy (1 insn shorter, score 110).

extern void func_80076990(void);  // EnterCriticalSection

int D_8008D78C = 0;   // g_nCriticalSectionCount (gp+0x820)

void Smart_EnterCriticalSection(void)
{
    int count;
    int oldCount;

    count = D_8008D78C;
    oldCount = count;
    count += 1;
    D_8008D78C = count;
    if (oldCount == 0)
        func_80076990();
}
