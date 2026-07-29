#include "HOWL.h"

// UpdateChannelVol_EngineFX_All @ 0x8002AE64 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 205, with the WHOLE ~66-instruction
//   function matching (behavior byte-for-byte correct) except one 3-line prologue diff. The entire
//   residual is the IDENTICAL #34/#39/#56-class g_pChannelTakenFirst (D_8009056C) list-head-load
//   tie-break: retail materializes %hi(D_8009056C) into a caller-saved scratch and loads the head via
//   it (`lui $v0,%hi; lw $s0,%lo($v0)`), while gcc materializes it directly into the callee-saved loop
//   register (`lui $s0,%hi; lw $s0,%lo($s0)`). This is a prologue register-materialization tie-break,
//   outside the permuter's body-level transforms and unforceable from C (a do-while restructure mirroring
//   the asm loop shape left it at 205; #39 established it across ~8400 iterations + manual nudges with
//   zero break). USER-GRANTED via the standing rule for this specific g_pChannelTakenFirst head-load-lui
//   residual (auto-grants without asking; previously granted for #39, #56).
//
// Walks the in-use (taken) channel list g_pChannelTakenFirst; for every non-music channel, flags it
// for a hardware volume update (g_abChannelUpdateFlags[channelID] |= 0x40) and recomputes its volume
// via UpdateChannelVol_EngineFX (type 0) or UpdateChannelVol_OtherFX (type 1), passing the channel's
// FX metadata entry (g_pHowlMetaEngineFX/OtherFX + soundID.id * 8) and its live attribute block
// (&g_abChannelAttrNew[channelID]).
//
// Build note: g_pHowlMetaEngineFX (gp+0x864) / g_pHowlMetaOtherFX (gp+0x86C) are GP-relative, DEFINED
// so gcc emits %gp_rel (GP_VALUE=0x8008CF6C). g_pChannelTakenFirst / g_abChannelUpdateFlags /
// g_abChannelAttrNew are absolute. Callees func_8002ACB8 (UpdateChannelVol_EngineFX) / func_8002AD04
// (UpdateChannelVol_OtherFX). soundID read as *(u_short*) to take only the low 16 bits (the sound id)
// via a single lhu, matching retail's `lhu 0x18`.

extern void func_8002ACB8(u_char* meta, struct ChannelAttr* attr, int vol, int LR);  // UpdateChannelVol_EngineFX
extern void func_8002AD04(u_char* meta, struct ChannelAttr* attr, int vol, int LR);  // UpdateChannelVol_OtherFX

extern struct ChannelStats* D_8009056C;  // g_pChannelTakenFirst
extern int                  D_8008FC6C[];// g_abChannelUpdateFlags (one int per channel)
extern struct ChannelAttr   D_8008FCCC[];// g_abChannelAttrNew

int D_8008D7D0 = 0;   // g_pHowlMetaEngineFX (gp+0x864)
int D_8008D7D8 = 0;   // g_pHowlMetaOtherFX  (gp+0x86C)

void UpdateChannelVol_EngineFX_All(void)
{
    struct ChannelStats* channel;

    for (channel = D_8009056C; channel != 0; channel = channel->next)
    {
        if (channel->type == 0)
        {
            D_8008FC6C[channel->channelID] |= 0x40;
            func_8002ACB8((u_char*)(D_8008D7D0 + *(u_short*)&channel->soundID * 8),
                          &D_8008FCCC[channel->channelID], channel->vol, channel->LR);
        }
        else if (channel->type == 1)
        {
            D_8008FC6C[channel->channelID] |= 0x40;
            func_8002AD04((u_char*)(D_8008D7D8 + *(u_short*)&channel->soundID * 8),
                          &D_8008FCCC[channel->channelID], channel->vol, channel->LR);
        }
    }
}
