#include "HOWL.h"

// howl_PauseAudio @ 0x8002C510 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best score = 320 (4 register diffs + 1 insertion + 2 deletions,
//   0 reorderings), byte-BEHAVIOR-correct. GRANTED (2026-07-27, user case-by-case). The whole loop body,
//   the 0x20-byte ChannelStats block-copy, and the full structure match exactly; the ENTIRE residual is
//   TWO load-target register-materialization tie-breaks, both the same unforceable gcc-2.8.1 class:
//     (1) HEAD-LOAD-LUI on the taken-list head D_8009056C (the standing auto-grant class): retail
//         `lui $v0,%hi(D_8009056C); lw $s1,%lo(D_8009056C)($v0)` materializes %hi in a caller-saved
//         scratch; gcc coalesces it as `lui $s1; lw $s1,%lo($s1)`.
//     (2) COUNTER-LOAD reg: retail reads g_nNumPausedChannels into $v0 then copies it to $a3=idx
//         (`lw $v0,0x824($gp); move $a3,$v0; addiu $v0,$v0,1`); gcc reads it directly into $a3=idx and
//         computes the +1 from there (`lw $a3,0x824($gp); addiu $v0,$a3,1`), saving the redundant `move`.
//   Both are "load into scratch then move vs load straight into the final reg" — not forceable from C.
//   Permuter floor confirmed over 115 iterations (only ever worsens 320); no C reformulation of the
//   counter RMW changes gcc's load-target choice.
//
//   KEY STRUCTURAL CRACK (1450 -> 830 -> 320): (a) the counter RMW must sit AFTER both flag ops
//   (`|=1` then `&=~2`), not between them, or gcc hoists the counter load ahead of the `|=1` (830);
//   (b) the `{ int* p; int f = *p; next = item->next; *p = f | 1; }` block below is MATCH-NECESSARY: by
//   reading `next` BETWEEN the flags load and the `|=1` store, gcc slots the `next` load into the
//   flags-load delay slot exactly like retail, which un-hoists everything (0 reorderings). The block still
//   recomputes &g_abChannelUpdateFlags[channelID] for the `&=~2`, so both retail channelID reads remain.
//
// Pauses all audio: requests an XA pause (CDSYS_XAPauseRequest) then, if not already paused
// (g_nNumPausedChannels==0), pauses CSEQ music, enters a critical section, and backs up every taken SPU
// channel's 0x20-byte ChannelStats into the backup array (D_8008FFCC, stride 0x20, index = running
// g_nNumPausedChannels), moving each channel from the taken list to the free list. Per channel it also
// sets bit0 and clears bit1 of g_abChannelUpdateFlags[channelID].
//
// GP-rel DEFINED (GP_VALUE=0x8008CF6C): g_nNumPausedChannels=D_8008D790(0x824,int). ABS extern:
// g_pChannelTakenFirst=D_8009056C, g_pChannelFreeFirst=D_80090578, g_abChannelUpdateFlags=D_8008FC6C(int[]),
// g_aPausedChannelBackup=D_8008FFCC(ChannelStats[]). Callees: func_8001CF98 CDSYS_XAPauseRequest,
// func_80028D64 CseqMusic_Pause, func_8002B4D0 Smart_EnterCriticalSection, func_8002B508
// Smart_ExitCriticalSection, func_800317E4 LIST_RemoveMember, func_80031788 LIST_AddBack.

extern void func_8001CF98(void);                                   // CDSYS_XAPauseRequest
extern void func_80028D64(void);                                   // CseqMusic_Pause
extern void func_8002B4D0(void);                                   // Smart_EnterCriticalSection
extern void func_8002B508(void);                                   // Smart_ExitCriticalSection
extern void func_800317E4(void* list, struct ChannelStats* item);  // LIST_RemoveMember
extern void func_80031788(void* list, struct ChannelStats* item);  // LIST_AddBack

extern struct ChannelStats* D_8009056C;    // g_pChannelTakenFirst
extern struct ChannelStats* D_80090578;    // g_pChannelFreeFirst
extern int                  D_8008FC6C[];  // g_abChannelUpdateFlags (int per channel)
extern struct ChannelStats  D_8008FFCC[];  // g_aPausedChannelBackup (0x20 stride)

int D_8008D790 = 0;   // g_nNumPausedChannels (gp+0x824)

void howl_PauseAudio(void)
{
    struct ChannelStats* item;
    struct ChannelStats* next;
    int idx;

    func_8001CF98();
    if (D_8008D790 == 0)
    {
        func_80028D64();
        func_8002B4D0();
        for (item = D_8009056C; item != 0; item = next)
        {
            // Match-necessary form: reading `next` between the flags load and the |=1 store makes gcc
            // fill the flags-load delay slot with the `next` load (as retail does), which prevents the
            // counter RMW from being hoisted ahead of the flag ops. See MATCHING NOTE above.
            {
                int* p = &D_8008FC6C[item->channelID];
                int f = *p;
                next = item->next;
                *p = f | 1;
            }
            D_8008FC6C[item->channelID] &= ~2;
            idx = D_8008D790;
            D_8008D790 = idx + 1;
            D_8008FFCC[idx] = *item;
            func_800317E4(&D_8009056C, item);
            func_80031788(&D_80090578, item);
        }
        func_8002B508();
    }
}
