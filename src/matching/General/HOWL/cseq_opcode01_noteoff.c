#include "HOWL.h"

// cseq_opcode01_noteoff @ 0x80029E18 (SCUS_944.26.exe, 926/NTSC-U). (Ghidra: howl_StopSoundChannels)
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 365, USER-GRANTED time-boxed
//   exception (3 fires). Behavior is byte-for-byte correct; the entire residual is 2 gcc-2.8.1
//   instruction-SCHEDULING artifacts that no C form and the permuter (~8000 iterations) can force:
//     (a) the initial `item = g_pChannelTakenFirst` load: retail materializes the %hi into a
//         caller-saved scratch (`lui $v0`) BEFORE the $s0 frame-save and loads via it
//         (`lw $s0,%lo($v0)`); gcc materializes it directly into the callee-saved $s0
//         (`lui $s0; lw $s0,%lo($s0)`). (#34-class prologue scheduling.)
//     (b) retail HOISTS `lui $a0,%hi(g_pChannelTakenFirst)` (the LIST_RemoveMember arg) into the
//         drumIndex-check `bne` delay slot, then completes `addiu $a0` mid-body; gcc leaves that
//         delay slot as a nop and materializes the address late. The extra nop is a single
//         instruction whose position cascade accounts for most of the 365. Manual nudges (hoisting
//         the arg address via a local, extracting the |=1 bit into a temp) were all folded away.
//   NOTE: 365 is higher than the usual scheduling near-match band; it is inflated by (b)'s
//   +1-instruction cascade, not by any behavioral or structural divergence.
//
// CSEQ sound-modify opcode (table index 1). Releases all HOWL channels (ChannelStats) playing
// seq->soundID whose drumIndex_pitchIndex == seq->currNote[1]: clears the channel's
// g_abChannelUpdateFlags entry (|=1 then &=~2) + active bit (flags&=0xfe), then moves it taken->free
// (LIST_RemoveMember g_pChannelTakenFirst / LIST_AddBack g_pChannelFreeFirst).
//
// Build note: all globals are absolute (%hi/%lo), left extern. No gp-relative globals here.

extern void func_800317E4(void* list, struct ChannelStats* item);  // LIST_RemoveMember
extern void func_80031788(void* list, struct ChannelStats* item);  // LIST_AddBack

extern struct ChannelStats* D_8009056C;  // g_pChannelTakenFirst
extern struct ChannelStats* D_80090578;  // g_pChannelFreeFirst
extern int                  D_8008FC6C[];// g_abChannelUpdateFlags (one int per channel)

void cseq_opcode01_noteoff(struct SongSeq* seq)
{
    struct ChannelStats* item;
    struct ChannelStats* next;
    u_char* currNote;

    currNote = seq->currNote;
    item = D_8009056C;
    while (item != 0)
    {
        next = item->next;
        if ((item->type == 2) &&
            (item->soundID == seq->soundID) &&
            (item->drumIndex_pitchIndex == currNote[1]))
        {
            D_8008FC6C[item->channelID] |= 1;
            D_8008FC6C[item->channelID] &= 0xfffffffd;
            item->flags = item->flags & 0xfe;
            func_800317E4(&D_8009056C, item);
            func_80031788(&D_80090578, item);
        }
        item = next;
    }
}
