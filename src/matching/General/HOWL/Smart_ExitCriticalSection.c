#include "HOWL.h"

// Smart_ExitCriticalSection @ 0x8002B508 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Nesting-aware critical-section leave for the audio system: if the counter is non-zero, decrements it
// and, on the 1->0 transition, calls the real ExitCriticalSection (re-enables IRQs). No-op if already 0.
//
// Build note: g_nCriticalSectionCount (gp+0x820) is GP-relative int, DEFINED so gcc emits %gp_rel
// (GP_VALUE=0x8008CF6C). Callee func_800767D0 (ExitCriticalSection, PSY-Q libapi).

extern void func_800767D0(void);  // ExitCriticalSection

int D_8008D78C = 0;   // g_nCriticalSectionCount (gp+0x820)

void Smart_ExitCriticalSection(void)
{
    int count;

    count = D_8008D78C;
    if (count != 0)
    {
        count = count - 1;
        D_8008D78C = count;
        if (count == 0)
            func_800767D0();
    }
}
