#include "HOWL.h"

// SongPool_StopCseq @ 0x8002AB18 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 365, with the whole ~60-instruction
//   function matching (8-line diff, behavior byte-for-byte correct). The entire residual is the
//   IDENTICAL #34/#39-class g_pChannelTakenFirst list-head-load tie-break already granted an exception
//   in cseq_opcode01_noteoff (#39): retail computes %hi(g_pChannelTakenFirst) into a caller-saved
//   scratch and loads the head via it (`lui $v0; sw $s0; lw $s0,%lo($v0)`), while gcc materializes it
//   directly into the callee-saved destination (`sw $s0; lui $s0; lw $s0,%lo($s0)`); plus retail
//   hoists the LIST_RemoveMember arg's `lui $a0,%hi(g_pChannelTakenFirst)` into the soundID-bne delay
//   slot, which gcc leaves as a nop (+1-instruction branch-offset cascade). Both are prologue/delay-
//   slot scheduling tie-breaks unforceable from C and outside the permuter's body-level transforms
//   (#39 established this across ~8400 iterations + manual nudges, zero break). USER-GRANTED exception
//   (standing rule for this specific g_pChannelTakenFirst head-load-lui residual).
//
// Stops one CSEQ sequence by freeing every hardware channel currently playing it. Walks the taken-
// channel list; for each MUSIC(2)-type channel with soundID == seq->soundID, sets its update flag
// (|=1 OFF, &=~2 clear ON), clears channel->flags bit0, and recycles the node (LIST_RemoveMember from
// taken, LIST_AddBack to free). Finally clears seq->flags bit0 (no longer playing).

extern void func_800317E4(void* list, struct ChannelStats* item);  // LIST_RemoveMember
extern void func_80031788(void* list, struct ChannelStats* item);  // LIST_AddBack

extern struct ChannelStats* D_8009056C;  // g_pChannelTakenFirst
extern struct ChannelStats* D_80090578;  // g_pChannelFreeFirst
extern int                  D_8008FC6C[];// g_abChannelUpdateFlags (one int per channel)

void SongPool_StopCseq(struct SongSeq* seq)
{
    struct ChannelStats* channel;
    struct ChannelStats* next;

    channel = D_8009056C;
    while (channel != 0)
    {
        next = channel->next;
        if ((channel->type == 2) && (channel->soundID == seq->soundID))
        {
            D_8008FC6C[channel->channelID] |= 1;
            D_8008FC6C[channel->channelID] &= 0xfffffffd;
            channel->flags &= 0xfe;
            func_800317E4(&D_8009056C, channel);
            func_80031788(&D_80090578, channel);
        }
        channel = next;
    }
    seq->flags &= 0xfe;
}
