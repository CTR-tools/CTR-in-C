#include "HOWL.h"

// Channel_AllocSlot_AntiSpam @ 0x8002B608 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 995, with the WHOLE ~114-instruction
//   function matching (behavior byte-for-byte correct). The ENTIRE residual is the #34/#39/#56/#62/#63/
//   #73-class head-load-lui, here on BOTH linked-list heads: g_pChannelTakenFirst (D_8009056C, anti-spam
//   walk) AND g_pChannelFreeFirst (D_80090578, allocation). Retail materializes each list head's
//   %hi into a caller-saved scratch and loads via it (`lui $v0,%hi; lw $s0,%lo($v0)`), which lets it
//   REUSE that scratch to build the LIST_ arg address (`addiu $a0,$v0,%lo` in a delay slot); gcc
//   coalesces the %hi into the callee-saved load-dest ($s0) (`lui $s0,%hi; lw $s0,%lo($s0)`), so it must
//   re-materialize `lui $a0,%hi` for each &list arg. A prologue register-materialization tie-break,
//   unforceable from C (reordering the head/gGT loads and explicit &-pointers were both neutral) and
//   outside the permuter's body-level transforms (its mutations only worsened 995). USER-GRANTED
//   (2026-07-26) and the standing head-load-lui rule was extended to cover BOTH channel-list heads
//   (taken + free) when they are the only residual, even above 300.
//
// Allocates an SPU channel for a sound. If boolUseAntiSpam, first stops any OtherFX channel (type==1)
// playing the same soundID that was started within the last ~10 frames (anti channel-overflow on rapid
// re-triggers): clears its update flags (|=1, &=~2), clears channel->flags bit0, and recycles it
// (LIST_RemoveMember from taken, LIST_AddBack to free). Then takes a slot from the free list, moves it
// to taken, sets its update flags (|= 2 | flags), copies the 4-word ChannelAttr into g_abChannelAttrNew,
// and marks it playing (flags = 1). Returns the channel, or NULL if none free.
//
// Build note: all globals absolute (%hi/%lo). Callees func_800317E4 (LIST_RemoveMember) / func_80031788
// (LIST_AddBack). soundID kept raw and masked per-use (andi 0xffff); channel->soundID read as u16 (lhu).
// The ChannelAttr copy is a 4-word struct assignment (retail emits lw/sw t0..t3).

extern void func_800317E4(void* list, struct ChannelStats* item);  // LIST_RemoveMember
extern void func_80031788(void* list, struct ChannelStats* item);  // LIST_AddBack

extern struct ChannelStats* D_8009056C;   // g_pChannelTakenFirst
extern struct ChannelStats* D_80090578;   // g_pChannelFreeFirst
extern int                  D_8008FC6C[]; // g_abChannelUpdateFlags (one int per channel)
extern struct ChannelAttr   D_8008FCCC[]; // g_abChannelAttrNew
extern struct GameTracker*  D_8008D2AC;   // sdata_gGT

struct ChannelStats* Channel_AllocSlot_AntiSpam(short soundID, char boolUseAntiSpam, uint flags, void* attr)
{
    struct ChannelStats* channel;

    if (boolUseAntiSpam == 1)
    {
        int iNowFrame;
        struct ChannelStats* item;

        iNowFrame = D_8008D2AC->frameTimer_MainFrame_ResetDB;
        item = D_8009056C;
        while (item != 0)
        {
            struct ChannelStats* next = item->next;
            if (item->type == 1 && *(u_short*)&item->soundID == (u_short)soundID
                && (uint)(iNowFrame - item->startFrame) < 10)
            {
                D_8008FC6C[item->channelID] |= 1;
                D_8008FC6C[item->channelID] &= 0xfffffffd;
                item->flags &= 0xfe;
                func_800317E4(&D_8009056C, item);
                func_80031788(&D_80090578, item);
            }
            item = next;
        }
    }

    channel = D_80090578;
    if (channel == 0)
    {
        return 0;
    }
    func_800317E4(&D_80090578, channel);
    func_80031788(&D_8009056C, channel);
    {
        int uf = D_8008FC6C[channel->channelID];
        uf |= 2;
        uf |= flags;
        D_8008FC6C[channel->channelID] = uf;
    }
    D_8008FCCC[channel->channelID] = *(struct ChannelAttr*)attr;
    channel->flags = 1;
    return channel;
}
