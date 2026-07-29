#include "HOWL.h"

// Channel_AllocSlot @ 0x8002B7D0 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 1030, with the ENTIRE function BODY
//   (everything from the first LIST_RemoveMember onward) byte-for-byte identical. The whole residual is
//   the #34/#39/#56/#73/#74-class head-load-lui on g_pChannelFreeFirst (D_80090578): retail materializes
//   its %hi into a caller-saved scratch ($v0) and REUSES that scratch to build the &free LIST_ arg
//   (`lui $v0,%hi; lw $s0,%lo($v0); addiu $a0,$v0,%lo`); gcc coalesces the %hi into the callee-saved
//   load-dest $s0 (`lui $s0,%hi; lw $s0,%lo($s0)`), so it re-materializes `lui $a0,%hi` for the arg and
//   shuffles the null-check placement. A prologue register-materialization tie-break, unforceable from C
//   (explicit &-pointer, inverted null-check, and params-first were all neutral or worse). USER-GRANTED
//   via the extended standing rule covering the free-head (g_pChannelFreeFirst) head-load-lui even above
//   300 (same class as #74's second head, here standalone).
//
// Takes the first free SPU channel (g_pChannelFreeFirst), moves it to the taken list, flags it playing
// (updateFlags |= 2 | flags), copies the 4-word ChannelAttr into g_abChannelAttrNew, and marks it active
// (flags = 1). Returns the channel, or NULL if none free. (This is Channel_AllocSlot_AntiSpam's #74
// allocate tail as a standalone function.)
//
// Build note: all globals absolute (%hi/%lo). Callees func_800317E4 (LIST_RemoveMember) / func_80031788
// (LIST_AddBack). The ChannelAttr copy is a 4-word struct assignment (retail emits lw/sw a2..t1); the
// update flags use an in-place accumulator so `|= 2 | flags` compiles to ori/or (not a reassociated
// `ori a1,flags,2`).

extern void func_800317E4(void* list, struct ChannelStats* item);  // LIST_RemoveMember
extern void func_80031788(void* list, struct ChannelStats* item);  // LIST_AddBack

extern struct ChannelStats* D_8009056C;   // g_pChannelTakenFirst
extern struct ChannelStats* D_80090578;   // g_pChannelFreeFirst
extern int                  D_8008FC6C[]; // g_abChannelUpdateFlags (one int per channel)
extern struct ChannelAttr   D_8008FCCC[]; // g_abChannelAttrNew

struct ChannelStats* Channel_AllocSlot(uint flags, struct ChannelAttr* attr)
{
    struct ChannelStats* item;

    item = D_80090578;
    if (item == 0)
    {
        return 0;
    }
    func_800317E4(&D_80090578, item);
    func_80031788(&D_8009056C, item);
    {
        int uf = D_8008FC6C[item->channelID];
        uf |= 2;
        uf |= flags;
        D_8008FC6C[item->channelID] = uf;
    }
    D_8008FCCC[item->channelID] = *attr;
    item->flags = 1;
    return item;
}
