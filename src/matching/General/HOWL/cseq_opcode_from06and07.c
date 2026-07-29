#include "HOWL.h"

// cseq_opcode_from06and07 @ 0x8002A170 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 205, with 71/71 INSTRUCTION PARITY and
//   a byte-EXACT structure (diff = only 3 lines). Behavior is byte-for-byte correct. The ENTIRE
//   residual is the prologue scheduling of the initial `channel = g_pChannelTakenFirst` load: retail
//   computes the %hi into a caller-saved scratch (`lui $v0`) and hoists it AHEAD of the `sw $s0`
//   frame-save, then loads via it (`lw $s0,%lo($v0)`); gcc materializes it directly into the
//   callee-saved $s0 (`lui $s0; lw $s0,%lo($s0)`), which cannot be scheduled before $s0's own store.
//   This is the identical #34/#39-class prologue register-save scheduling tie-break — the SAME global
//   (g_pChannelTakenFirst) that was unforceable in cseq_opcode01_noteoff (#39). It is outside the
//   permuter's body-level transforms (~1200 iterations, no break) and unforceable from C (pointer-to-
//   pointer, while-loop, and every form yield the same). The permuter DID crack the harder s3/s4
//   songPool/attrNew register swap sanely (via the `vol` temp, below).
//
// Shared CSEQ "set channel volume" opcode handler (opcodes 06 and 07). Walks the taken-channel list
// (g_pChannelTakenFirst); for each MUSIC(2)-type channel currently playing this sequence's soundID,
// recomputes its volume = (g_bVolMusic * g_aSongPool[seq->songPoolIndex].vol_Curr * seq->vol_Curr *
// channel->vol) >> 0x12 and applies it via Channel_SetVolume(&g_abChannelAttrNew[chId], vol,
// seq->LR), then sets the channel's vol-update flag (g_abChannelUpdateFlags[chId] |= 0x40).
//
// Match note: `vol` is a match-necessary temp for the `>> 0x12` volume — computing it into its own
// local fixes retail's s3/s4 register assignment for the g_aSongPool / g_abChannelAttrNew bases
// (235 -> 205); inlining the volume expression into the Channel_SetVolume arg swaps s3<->s4.
//
// Build note: g_bVolMusic is GP-relative (gp=0x8008CF6C), DEFINED so gcc emits %gp_rel
// (GP_VALUE=0x8008CF6C). The list head / attr / song-pool / flag arrays are absolute (%hi/%lo).

extern void func_8002B540(struct ChannelAttr* attr, uint vol, uint pan);  // Channel_SetVolume

extern struct ChannelStats* D_8009056C;  // g_pChannelTakenFirst
extern struct ChannelAttr   D_8008FCCC[];// g_abChannelAttrNew (0x10 bytes each)
extern struct Song          D_80095D84[];// g_aSongPool (0x7C bytes each)
extern int                  D_8008FC6C[];// g_abChannelUpdateFlags (one int per channel)

u_char D_8008D7B8 = 0;   // g_bVolMusic (gp+0x84C)

void cseq_opcode_from06and07(struct SongSeq* seq)
{
    struct ChannelStats* channel;
    uint vol;

    for (channel = D_8009056C; channel != 0; channel = channel->next)
    {
        if ((channel->type == 2) && (channel->soundID == seq->soundID))
        {
            vol = (int)((uint)D_8008D7B8 * (uint)D_80095D84[seq->songPoolIndex].vol_Curr
                        * (uint)seq->vol_Curr * (uint)channel->vol) >> 0x12;
            func_8002B540(&D_8008FCCC[channel->channelID], vol, (uint)seq->LR);
            D_8008FC6C[channel->channelID] |= 0x40;
        }
    }
}
