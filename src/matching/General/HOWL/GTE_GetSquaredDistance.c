#include "HOWL.h"

// GTE_GetSquaredDistance @ 0x8002E7BC (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH — NOT score-0 (USER-GRANTED exception, 2026-07-29). Behavior byte-EXACT. Delivered as the
// clean natural in-order form (score 1280); the permuter's 785 floor needs an artificial scheduling
// temp (compute dz before dy) that is not meaningfully more natural. Root blocker is the same either way.
// Squared distance between two 3-vectors a and b: d = a - b (componentwise), then squared length of d
// via IR1/IR2/IR3 + gte_sqr0, returning MAC1+MAC2+MAC3. Companion to GTE_GetSquaredLength (#121); the
// diff local doubles as the MAC output buffer (round-tripped through memory). Verified vs asm.
//
// MATCH NOTE — two COMPOUNDING blockers, neither forceable via natural C:
//  (1) TOOLCHAIN GAP: retail has a `nop` after each `mfc2` (R3000 cop2-move delay — the GPR written by
//      mfc2 is not ready for the next instruction). maspsx never emits it: `mfc2`/`cfc2` are absent from
//      its `load_mnemonics` (same class as the gp-rel load-delay nop fix). My mfc2-then-store therefore
//      lacks the retail delay nops, and the permuter cannot add them (toolchain-level, not C-level).
//  (2) REGISTER/CODEGEN: retail reuses ONE reg (a3) for all three mfc2 and reloads d[0]/d[1] from the
//      stack; my gcc uses three regs (v0/a0/v1) and sums directly. A `"memory"` clobber reproduces the
//      reload but is semantically false for mfc2 (a forbidden hack) and still mis-colors (v0 vs a3 -> 885).

int GTE_GetSquaredDistance(short* a, short* b)
{
    int d[3];
    int* p1;
    int* p2;

    d[0] = a[0] - b[0];
    d[1] = a[1] - b[1];
    d[2] = a[2] - b[2];
    gte_ldIR1(d);
    p1 = &d[1];
    gte_ldIR2(p1);
    p2 = &d[2];
    gte_ldIR3(p2);
    gte_sqr0();
    gte_stMAC1(d[0]);
    gte_stMAC2(d[1]);
    gte_stMAC3(d[2]);
    return d[0] + d[1] + d[2];
}
