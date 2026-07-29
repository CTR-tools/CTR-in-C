#include "HOWL.h"

// howl_UnPauseAudio @ 0x8002C784 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best (natural-code) score = 1930, byte-BEHAVIOR-correct. GRANTED
//   (2026-07-27) as an exception. The overall structure matches retail exactly — the guard, EnterCritical,
//   the do/while over the backup array, the 0x20-byte ChannelStats block-copy, the next/prev/channelID
//   re-override, the LIST_RemoveMember/AddBack pair, howl_UnPauseChannel, ExitCritical, CseqMusic_Resume,
//   and clearing the counter. The residual is PURELY a gcc-2.8.1 register-allocation / CSE difference with
//   IDENTICAL behavior (same loads, stores, calls, and results, in the same order):
//     (1) retail does NOT common-subexpression-eliminate the two `stats->next` reads (nextFree and
//         savedNext) and consequently SPILLS savedNext/savedPrev to the stack across the block-copy (a2/a3
//         are reused by the copy); this compiler build reads stats->next once and keeps it in a register,
//         needing no spill. Both are behaviorally identical — it is a compiler-internal choice.
//     (2) an $s2/$s3 register swap (loop counter i vs. the backup pointer).
//     (3) the free-list head-load-lui on g_pChannelFreeFirst (D_80090578) — the standing auto-grant class.
//   Reaching score 0 would require forcing gcc to emit deliberately-redundant reads + stack spills, which
//   is only expressible via unnatural contortions (volatile casts, type-punned on-stack scratch, empty
//   marker blocks) that would never have appeared in the original source. Per project guidance this file
//   keeps the natural, original-like structure (which the Ghidra reference confirms: two stats->next reads
//   into two named locals) rather than such hacks. Permuter (~130 iters) finds only cosmetic or
//   semantically-broken forms.
//
// Restores audio paused by howl_PauseAudio. For each of the g_nNumPausedChannels backed-up channels: pops a
// free channel, restores its ChannelStats from the backup array (block copy, then re-overrides the free
// node's own next/prev/channelID), re-queues it onto the taken list, and re-arms it via howl_UnPauseChannel
// (#87). Then resumes CSEQ music and clears the counter. Guarded by g_nNumPausedChannels != 0.
//
// GP-rel DEFINED (GP_VALUE=0x8008CF6C): g_nNumPausedChannels=D_8008D790(0x824,int). ABS extern:
// g_pChannelTakenFirst=D_8009056C, g_pChannelFreeFirst=D_80090578, g_aPausedChannelBackup=D_8008FFCC
// (ChannelStats[],0x20). Callees: func_8002B4D0 Smart_EnterCriticalSection, func_8002B508
// Smart_ExitCriticalSection, func_80028DE0 CseqMusic_Resume, func_800317E4 LIST_RemoveMember,
// func_80031788 LIST_AddBack, func_8002C64C howl_UnPauseChannel(#87).

extern void func_8002B4D0(void);                                   // Smart_EnterCriticalSection
extern void func_8002B508(void);                                   // Smart_ExitCriticalSection
extern void func_80028DE0(void);                                   // CseqMusic_Resume
extern void func_800317E4(void* list, struct ChannelStats* item);  // LIST_RemoveMember
extern void func_80031788(void* list, struct ChannelStats* item);  // LIST_AddBack
extern void func_8002C64C(struct ChannelStats* stats);             // howl_UnPauseChannel (#87)

extern struct ChannelStats* D_8009056C;   // g_pChannelTakenFirst
extern struct ChannelStats* D_80090578;   // g_pChannelFreeFirst
extern struct ChannelStats  D_8008FFCC[]; // g_aPausedChannelBackup (0x20 stride)

int D_8008D790 = 0;   // g_nNumPausedChannels (gp+0x824)

void howl_UnPauseAudio(void)
{
    struct ChannelStats* stats;
    struct ChannelStats* nextFree;
    struct ChannelStats* savedNext;
    struct ChannelStats* savedPrev;
    struct ChannelStats* backup;
    u_char savedChannelID;
    int i;

    if (D_8008D790 != 0)
    {
        i = 0;
        func_8002B4D0();
        stats = D_80090578;
        if (0 < D_8008D790)
        {
            backup = D_8008FFCC;
            do
            {
                if (stats == 0)
                    break;
                nextFree = stats->next;
                savedChannelID = stats->channelID;
                savedNext = stats->next;
                savedPrev = stats->prev;
                *stats = *backup;
                backup++;
                stats->channelID = savedChannelID;
                stats->next = savedNext;
                stats->prev = savedPrev;
                i++;
                func_800317E4(&D_80090578, stats);
                func_80031788(&D_8009056C, stats);
                func_8002C64C(stats);
                stats = nextFree;
            } while (i < D_8008D790);
        }
        func_8002B508();
        func_80028DE0();
        D_8008D790 = 0;
    }
}
