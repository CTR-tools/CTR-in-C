#include "HOWL.h"

// Channel_DestroyAll_LowLevel @ 0x8002BA90 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 365 (FIRST compile — the whole ~71-
//   instruction function matched, behavior byte-for-byte correct). The ENTIRE residual is the IDENTICAL
//   #34/#39/#56-class g_pChannelTakenFirst (D_8009056C) head-load-lui + its delay-slot lui-hoist for the
//   LIST_RemoveMember &taken arg: retail materializes %hi(D_8009056C) into a caller-saved scratch $v0
//   and loads the head via it (`lui $v0,%hi; lw $s0,%lo($v0)`), then hoists the `lui $a0,%hi(D_8009056C)`
//   for the &taken arg into the first-RMW `lw` delay slot; gcc coalesces the %hi into the callee-saved
//   $s0 (`lui $s0,%hi; lw $s0,%lo($s0)`) and leaves the arg lui unhoisted. Prologue register-
//   materialization tie-break, unforceable from C. USER-GRANTED via the standing g_pChannelTakenFirst
//   head-load-lui rule (auto-grants; exactly the #56 residual, same 365).
//
// Stops/recycles SPU channels. opt1=1 forces all FX off (opt1=0 lets a menu OtherFX with soundID>5
// ring out); boolKeepMusic=1 keeps channels of `type` (music). For each channel that should be stopped
// it flags it OFF (updateFlags |= 1 then &= ~2), clears its playing bit, and moves it from the taken
// list to the free list. Walks the whole taken list.
//
// Build note: all globals absolute (%hi/%lo). Callees func_800317E4 (LIST_RemoveMember) / func_80031788
// (LIST_AddBack). Type mask is per-iteration (retail re-masks `andi v0,s3,0xff` in the boolKeepMusic
// branch's delay slot each pass), so not precomputed. soundID read as *(u_short*) (lhu).

extern void func_800317E4(void* list, struct ChannelStats* item);  // LIST_RemoveMember
extern void func_80031788(void* list, struct ChannelStats* item);  // LIST_AddBack

extern struct ChannelStats* D_8009056C;   // g_pChannelTakenFirst
extern struct ChannelStats* D_80090578;   // g_pChannelFreeFirst
extern int                  D_8008FC6C[]; // g_abChannelUpdateFlags (one int per channel)

void Channel_DestroyAll_LowLevel(int opt1, int boolKeepMusic, char type)
{
    struct ChannelStats* item;
    struct ChannelStats* next;
    int* uf;

    item = D_8009056C;
    if (item == 0)
        return;
    uf = D_8008FC6C;
    do {
        next = item->next;
        if ((boolKeepMusic == 0 || item->type != type)
            && (opt1 != 0 || item->type != 1 || *(u_short*)&item->soundID > 5))
        {
            uf[item->channelID] |= 1;
            uf[item->channelID] &= 0xfffffffd;
            item->flags &= 0xfe;
            func_800317E4(&D_8009056C, item);
            func_80031788(&D_80090578, item);
        }
        item = next;
    } while (item != 0);
}
