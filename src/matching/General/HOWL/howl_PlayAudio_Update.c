#include "HOWL.h"

// howl_PlayAudio_Update @ 0x8002C208 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best score = 370 (FIRST compile — the whole ~85-instruction
//   function matched, behavior byte-for-byte correct). The ENTIRE residual is the standing g_pChannelTakenFirst
//   (D_8009056C) head-load-lui class (auto-granted for #39/#56/#73-78): retail materializes %hi(D_8009056C)
//   into a caller-saved scratch $v0 (in both the fade-skip branch's delay slot AND after the fade) and loads
//   the list head via it (`lui $v0,%hi; lw $s0,%lo($v0)`), and hoists the `lui $a0,%hi(D_8009056C)` for the
//   LIST_RemoveMember &taken arg into the timer-expiry bgtz delay slot; gcc coalesces the %hi into the
//   callee-saved $s0 (`lui $s0,%hi; lw $s0,%lo($s0)`) and leaves the delay slots as nops. Prologue
//   register-materialization tie-break, unforceable from C. AUTO-GRANTED (2026-07-27) per the standing rule.
//
// Per-frame audio tick. If audio is enabled: (a) if a cutscene volume backup is active, fade the backed-up
// volume down by 2 (clamp at 0) and re-apply it via howl_VolumeSet under a guard flag; (b) walk the taken
// channel list and, for each timed sound whose timer expires (timeLeft -= 5 <= 0), flag its voice off
// (updateFlags |= 1, &= ~2), clear its active bit, and recycle it from the taken list to the free list;
// (c) tick the CSEQ sequencer (Channel_ParseSongToChannels). Always commits SPU voices (Channel_UpdateChannels).
//
// GP-rel globals DEFINED (GP_VALUE=0x8008CF6C, patch_gprel via D_<addr>). List heads + update-flags array
// are absolute. Callees: func_8002B130 howl_VolumeSet(#66), func_800317E4 LIST_RemoveMember,
// func_80031788 LIST_AddBack, func_8002BBAC Channel_ParseSongToChannels(#79), func_8002BE9C
// Channel_UpdateChannels(#80).

extern void func_8002B130(int, u_char);                            // howl_VolumeSet
extern void func_800317E4(void* list, struct ChannelStats* item);  // LIST_RemoveMember
extern void func_80031788(void* list, struct ChannelStats* item);  // LIST_AddBack
extern void func_8002BBAC(void);                                   // Channel_ParseSongToChannels
extern void func_8002BE9C(void);                                   // Channel_UpdateChannels

extern struct ChannelStats* D_8009056C;    // g_pChannelTakenFirst
extern struct ChannelStats* D_80090578;    // g_pChannelFreeFirst
extern int                  D_8008FC6C[];  // g_abChannelUpdateFlags

u_char  D_8008D05C = 0;   // boolAudioEnabled  (gp+0xF0)
int     D_8008D78C = 0;   // volume-set guard  (gp+0x820)
u_short D_8008D79C = 0;   // fade/stored volume (gp+0x830)
u_char  D_8008D79F = 0;   // boolStoringVolume (gp+0x833)

void howl_PlayAudio_Update(void)
{
    struct ChannelStats* curr;
    struct ChannelStats* backupNext;

    if (D_8008D05C != 0)
    {
        if (D_8008D79F != 0)
        {
            int v = D_8008D79C - 2;
            D_8008D79C = v;
            if ((short)v < 0)
                D_8008D79C = 0;
            D_8008D78C = 1;
            func_8002B130(0, D_8008D79C);
            D_8008D78C = 0;
        }

        for (curr = D_8009056C; curr != 0; curr = backupNext)
        {
            int t;
            backupNext = curr->next;
            if ((curr->flags & 4) != 0)
                continue;
            t = *(u_short*)&curr->timeLeft - 5;
            curr->timeLeft = t;
            if ((short)t > 0)
                continue;
            D_8008FC6C[curr->channelID] |= 1;
            D_8008FC6C[curr->channelID] &= 0xfffffffd;
            curr->flags = curr->flags & 0xfe;
            func_800317E4(&D_8009056C, curr);
            func_80031788(&D_80090578, curr);
        }

        func_8002BBAC();
    }

    func_8002BE9C();
}
