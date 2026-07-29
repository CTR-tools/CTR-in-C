#include "HOWL.h"

// Channel_SearchFX_Destroy @ 0x8002B9B8 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 410 (435 -> 410); behavior byte-for-byte
//   correct. The residual is the #34/#39/#56/#74-class g_pChannelTakenFirst (D_8009056C) head-load-lui:
//   retail materializes its %hi into a caller-saved scratch $v0 and loads the head via it (`lui $v0,%hi;
//   lw $s0,%lo($v0)`), reusing $v0 for the updateFlags base and hoisting the `lui $a0,%hi(D_8009056C)`
//   for the LIST_RemoveMember &taken arg into a delay slot; gcc coalesces the %hi into the callee-saved
//   load-dest $s0 (`lui $s0,%hi; lw $s0,%lo($s0)`). The remaining register-allocation cascade in the
//   recycle path (the second updateFlags RMW takes $a1 vs $v1) is tied to that same prologue choice —
//   the permuter only worsens 410 and could not reduce it independently. USER-GRANTED via the standing
//   g_pChannelTakenFirst head-load-lui rule (auto-grants even above 300; same class as #74).
//
// Finds the first taken channel of the given type whose masked soundID matches
// ((item->soundID & mask) == (soundID & mask)), stops it (updateFlags |= 1 then &= ~2, clear flags
// bit0) and recycles it to the free list. Returns it (NULL if none). Used by OtherFX_Stop1/Stop2.
//
// Build note: all globals absolute (%hi/%lo). Callees func_800317E4 (LIST_RemoveMember) / func_80031788
// (LIST_AddBack). Unlike sibling #76 (a leaf), the type mask is re-masked per iteration (retail keeps
// type raw in $a0 and masks into $v1 in the advance delay slot), so it is NOT precomputed here.

extern void func_800317E4(void* list, struct ChannelStats* item);  // LIST_RemoveMember
extern void func_80031788(void* list, struct ChannelStats* item);  // LIST_AddBack

extern struct ChannelStats* D_8009056C;   // g_pChannelTakenFirst
extern struct ChannelStats* D_80090578;   // g_pChannelFreeFirst
extern int                  D_8008FC6C[]; // g_abChannelUpdateFlags (one int per channel)

struct ChannelStats* Channel_SearchFX_Destroy(char type, uint soundID, uint mask)
{
    struct ChannelStats* item;
    struct ChannelStats* next;
    int* uf;

    item = D_8009056C;
    if (item == 0)
        goto ret0;
    uf = D_8008FC6C;
loop:
    next = item->next;
    if (item->type != type)
        goto advance;
    if ((item->soundID & mask) != (soundID & mask))
        goto advance;
    uf[item->channelID] |= 1;
    uf[item->channelID] &= 0xfffffffd;
    item->flags &= 0xfe;
    func_800317E4(&D_8009056C, item);
    func_80031788(&D_80090578, item);
    return item;
advance:
    item = next;
    if (item != 0)
        goto loop;
ret0:
    return 0;
}
