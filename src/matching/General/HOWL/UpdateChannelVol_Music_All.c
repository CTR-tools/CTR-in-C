#include "HOWL.h"

// UpdateChannelVol_Music_All @ 0x8002AF6C (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 205, with the WHOLE ~49-instruction
//   function matching (behavior byte-for-byte correct) except one 3-line prologue diff. The entire
//   residual is the IDENTICAL #34/#39/#56/#62-class g_pChannelTakenFirst (D_8009056C) list-head-load
//   tie-break: retail materializes %hi(D_8009056C) into a caller-saved scratch and loads the head via
//   it (`lui $v0,%hi; lw $s0,%lo($v0)`), while gcc materializes it directly into the callee-saved loop
//   register (`lui $s0,%hi; lw $s0,%lo($s0)`). Prologue register-materialization tie-break, unforceable
//   from C (#39 established it across ~8400 iterations). USER-GRANTED via the standing rule for this
//   specific g_pChannelTakenFirst head-load-lui residual (auto-grants; previously granted #39, #56, #62).
//
// Match note: the `attr` temp (computing &g_abChannelAttrNew[channelID] BEFORE the songSeq arg) is
// MATCH-NECESSARY — it makes the attr-array base (D_8008FCCC) appear in source before the songSeq-array
// base (D_800902CC), so gcc allocates attr->$s2 / songSeq->$s1 like retail. Without it the two bases land
// in swapped saved regs, cascading through the call-arg setup (670 vs 205).
//
// Walks the in-use (taken) channel list g_pChannelTakenFirst; for every MUSIC channel (type == 2),
// flags it for a hardware volume update (g_abChannelUpdateFlags[channelID] |= 0x40) and recomputes its
// volume via UpdateChannelVol_Music, passing the channel's song-sequence (&g_aSongSeq[soundID]), its
// live attribute block (&g_abChannelAttrNew[channelID]), the drum/pitch index, and the channel volume.
//
// Build note: all globals are absolute (%hi/%lo) — g_pChannelTakenFirst, g_abChannelUpdateFlags,
// g_abChannelAttrNew, and g_aSongSeq (D_800902CC, the SongSeq array, stride 0x1C). Callee func_8002AD70
// (UpdateChannelVol_Music, #61). soundID read as a full lw (32-bit) index into g_aSongSeq.

extern void func_8002AD70(u_char* songSeq, struct ChannelAttr* attr, int index, int vol);  // UpdateChannelVol_Music

extern struct ChannelStats* D_8009056C;  // g_pChannelTakenFirst
extern int                  D_8008FC6C[];// g_abChannelUpdateFlags (one int per channel)
extern struct ChannelAttr   D_8008FCCC[];// g_abChannelAttrNew
extern struct SongSeq       D_800902CC[];// g_aSongSeq (SongSeq array, stride 0x1C)

void UpdateChannelVol_Music_All(void)
{
    struct ChannelStats* channel;

    for (channel = D_8009056C; channel != 0; channel = channel->next)
    {
        if (channel->type == 2)
        {
            struct ChannelAttr* attr;
            D_8008FC6C[channel->channelID] |= 0x40;
            attr = &D_8008FCCC[channel->channelID];
            func_8002AD70((u_char*)&D_800902CC[channel->soundID],
                          attr,
                          channel->drumIndex_pitchIndex, channel->vol);
        }
    }
}
