#include "HOWL.h"

// cseq_opcode08 @ 0x8002A400 (SCUS_944.26.exe, 926/NTSC-U). (Ghidra: howl_SetSoundReverb)
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 10, with 37/37 INSTRUCTION PARITY and a
//   byte-EXACT loop body (the ENTIRE function differs by exactly one 2-instruction pair). Behavior is
//   byte-for-byte correct. The whole residual is the initial list-head load: retail computes the
//   %hi(g_pChannelTakenFirst) into a caller-saved scratch (`lui $v0`) and loads the head via it
//   (`lw $a1,%lo(..)($v0)`); gcc COALESCES the %hi-temp and the load destination into one register and
//   emits `lui $a1,%hi; lw $a1,%lo(..)($a1)`. This is a pure register-COALESCING tie-break — the
//   IDENTICAL #34/#39/#42-class residual on the IDENTICAL global (g_pChannelTakenFirst) that was
//   unforceable in cseq_opcode01_noteoff (#39, 365) and cseq_opcode_from06and07 (#42, 205). Here there
//   is no stack frame and no cascade, so it is isolated to a single 2-instruction pair = score 10.
//   Outside the permuter's body-level transforms (~450 iterations, no break — it never cracked this
//   residual in #39/#42 either) and unforceable from C (do-while form + head-first source order both
//   stay 10; the coalescing decision is made in the register allocator, not from statement structure).
//
// CSEQ "set sound reverb" opcode (table index 8): walks the taken-channel list; for every MUSIC(2)-
// type channel currently playing this sequence's soundID, writes the note operand into that channel's
// reverb (g_abChannelAttrNew[channelID].reverb = currNote[1]) and sets its update flag
// (g_abChannelUpdateFlags[channelID] |= 0x20). Frameless leaf (calls nothing). Sibling of the
// pitch/pan opcodes.

extern struct ChannelStats* D_8009056C;   // g_pChannelTakenFirst
extern struct ChannelAttr   D_8008FCCC[];  // g_abChannelAttrNew (0x10 bytes each)
extern int                  D_8008FC6C[];  // g_abChannelUpdateFlags (one int per channel)

void cseq_opcode08(struct SongSeq* seq)
{
    struct ChannelStats* channel;
    u_char* currNote;

    currNote = seq->currNote;
    for (channel = D_8009056C; channel != 0; channel = channel->next)
    {
        if ((channel->type == 2) && (channel->soundID == seq->soundID))
        {
            D_8008FCCC[channel->channelID].reverb = currNote[1];
            D_8008FC6C[channel->channelID] |= 0x20;
        }
    }
}
