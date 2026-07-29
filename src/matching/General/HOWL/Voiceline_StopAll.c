#include "HOWL.h"

// Voiceline_StopAll @ 0x8002CB44 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best (natural-code) score = 955, byte-BEHAVIOR-correct. GRANTED
//   (2026-07-28, user; direct-&Voiceline1 form chosen over the v1-pointer form that scores 785 but changes
//   the codegen pattern). The loop structure, the per-iteration `last` capture, and both LIST calls all
//   match retail; the residual is a gcc-2.8.1 build LICM / address-materialization DIVERGENCE with
//   IDENTICAL behavior: for the Voiceline1 address (used once per iteration, as the LIST_AddFront arg)
//   retail hoists only `%hi(D_800961A4)` into a callee-saved register and emits `addiu $a0,$s2,%lo` each
//   iteration, whereas this compiler build recomputes the full `lui+addiu` per iteration. That %hi-only
//   hoist is a compiler-internal choice not expressible in natural C. The frequently-used Voiceline2
//   address (read as `.last` + passed to LIST_RemoveMember, 3 uses/iter) IS captured in a pointer local
//   `v2` so gcc keeps it in a callee-saved reg (matching retail's $s1) — without it, 1930; with it, 955.
//   `last` must be a local: LIST_RemoveMember mutates Voiceline2.last, so both calls use the pre-remove
//   value. Permuter finds only semantically-broken sub-955 forms (uninitialized-variable exploits).
//
// Drains the active voiceline list: while Voiceline2 has a last item, remove it from Voiceline2 and push it
// onto the front of the free list Voiceline1.
//
// ABS extern: Voiceline1=D_800961A4, Voiceline2=D_800961B4 (LinkedList heads). Callees: func_800317E4
// LIST_RemoveMember, func_80031744 LIST_AddFront.

extern void func_800317E4(void* list, void* item);   // LIST_RemoveMember
extern void func_80031744(void* list, void* item);   // LIST_AddFront

extern struct LinkedList D_800961A4;   // Voiceline1
extern struct LinkedList D_800961B4;   // Voiceline2

void Voiceline_StopAll(void)
{
    struct LinkedList* v2 = &D_800961B4;
    void* last;

    while ((last = v2->last) != 0)
    {
        func_800317E4(v2, last);
        func_80031744(&D_800961A4, last);
    }
}
