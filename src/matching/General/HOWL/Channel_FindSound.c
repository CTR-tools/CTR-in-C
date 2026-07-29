#include "HOWL.h"

// Channel_FindSound @ 0x8002B5B4 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 10, with the WHOLE ~21-instruction
//   function matching (behavior byte-for-byte correct) except one 2-instruction prologue diff. The
//   entire residual is the IDENTICAL #34/#39/#56/#62/#63-class g_pChannelTakenFirst (D_8009056C) list-
//   head-load tie-break: retail materializes %hi(D_8009056C) into a caller-saved scratch and loads the
//   head via it (`lui $v0,%hi; lw $v1,%lo($v0)`), while gcc coalesces the %hi into the load-dest register
//   (`lui $v1,%hi; lw $v1,%lo($v1)`). A prologue register-materialization tie-break, unforceable from C
//   and outside the permuter's body-level transforms (3 structural forms 645/1095/10 + a permuter run
//   whose mutations only worsened it). USER-GRANTED via the standing rule for this specific
//   g_pChannelTakenFirst head-load-lui residual (auto-grants; here the dest is caller-saved $v1 rather
//   than a saved reg, but the mechanism is identical).
//
// Returns 1 if any in-use channel is an OtherFX (type == 1) currently playing soundID, else 0. Walks
// the taken-channel list g_pChannelTakenFirst.
//
// Match note: single walking pointer with a shared `goto ret0` for the head-null-check and the loop-
// exit (both converge on retail's one return-0 block; separate returns cost +1085). g_pChannelTakenFirst
// (D_8009056C) is absolute (%hi/%lo). soundID and the channel's soundID are compared as 16-bit
// (andi/lhu); channel->soundID is read as *(u_short*) for the low 16.

extern struct ChannelStats* D_8009056C;  // g_pChannelTakenFirst

uint Channel_FindSound(short soundID)
{
    struct ChannelStats* channel;
    struct ChannelStats* next;

    channel = D_8009056C;
    if (channel == 0)
        goto ret0;
    do {
        next = channel->next;
        if (channel->type == 1 && *(u_short*)&channel->soundID == (u_short)soundID)
            return 1;
        channel = next;
    } while (channel != 0);
ret0:
    return 0;
}
