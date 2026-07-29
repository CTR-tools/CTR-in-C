#include "HOWL.h"

// UpdateChannelVol_OtherFX_All @ 0x8002B030 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 205, with the WHOLE ~44-instruction
//   function matching (behavior byte-for-byte correct) except one 3-line prologue diff. The entire
//   residual is the IDENTICAL #34/#39/#56/#62/#63-class g_pChannelTakenFirst (D_8009056C) list-head-load
//   tie-break: retail materializes %hi(D_8009056C) into a caller-saved scratch and loads the head via it
//   (`lui $v0,%hi; lw $s0,%lo($v0)`), while gcc materializes it directly into the callee-saved loop
//   register (`lui $s0,%hi; lw $s0,%lo($s0)`). Prologue register-materialization tie-break, unforceable
//   from C. USER-GRANTED via the standing rule for this specific g_pChannelTakenFirst head-load-lui
//   residual (auto-grants; previously granted #39, #56, #62, #63).
//
// Match note: the `attr` temp (computing &g_abChannelAttrNew[channelID] BEFORE the meta/soundID arg) is
// MATCH-NECESSARY — it makes gcc evaluate the attr/channelID arg before the meta/soundID arg (channelID
// load + $a1<<4 first), matching retail's arg-evaluation order. Without it the two arg computations are
// reordered (295 vs 205).
//
// Walks the in-use (taken) channel list g_pChannelTakenFirst; for every OTHER-FX channel (type == 1),
// flags it for a hardware volume update (g_abChannelUpdateFlags[channelID] |= 0x40) and recomputes its
// volume via UpdateChannelVol_OtherFX, passing the channel's FX metadata entry
// (g_pHowlMetaOtherFX + id*8), its live attribute block (&g_abChannelAttrNew[channelID]), vol, and LR.
//
// Build note: g_pHowlMetaOtherFX (gp+0x86C) is GP-relative, DEFINED so gcc emits %gp_rel
// (GP_VALUE=0x8008CF6C) and is reloaded per iteration (not hoisted). g_pChannelTakenFirst /
// g_abChannelUpdateFlags / g_abChannelAttrNew are absolute. Callee func_8002AD04
// (UpdateChannelVol_OtherFX, #60). soundID read as *(u_short*) to take the low 16 bits (id) via lhu.

extern void func_8002AD04(u_char* meta, struct ChannelAttr* attr, int vol, int LR);  // UpdateChannelVol_OtherFX

extern struct ChannelStats* D_8009056C;  // g_pChannelTakenFirst
extern int                  D_8008FC6C[];// g_abChannelUpdateFlags (one int per channel)
extern struct ChannelAttr   D_8008FCCC[];// g_abChannelAttrNew

int D_8008D7D8 = 0;   // g_pHowlMetaOtherFX (gp+0x86C)

void UpdateChannelVol_OtherFX_All(void)
{
    struct ChannelStats* channel;

    for (channel = D_8009056C; channel != 0; channel = channel->next)
    {
        if (channel->type == 1)
        {
            struct ChannelAttr* attr;
            D_8008FC6C[channel->channelID] |= 0x40;
            attr = &D_8008FCCC[channel->channelID];
            func_8002AD04((u_char*)(D_8008D7D8 + *(u_short*)&channel->soundID * 8),
                          attr, channel->vol, channel->LR);
        }
    }
}
