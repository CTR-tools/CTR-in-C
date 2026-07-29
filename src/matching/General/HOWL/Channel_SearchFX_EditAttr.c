#include "HOWL.h"

// Channel_SearchFX_EditAttr @ 0x8002B898 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 180 (2400 -> 180). The residual is
//   scheduling/register tie-breaks only (behavior byte-for-byte correct): (a) the #34/#39/#56/#73-class
//   g_pChannelTakenFirst (D_8009056C) head-load-lui — retail materializes its %hi into a caller-saved
//   scratch $v0 and loads the head via it (`lui $v0,%hi; lw $t0,%lo($v0)`), reusing $v0 as a rotating
//   scratch for the other globals and scheduling the head-null `beqz` + type-mask `andi` right after the
//   load; gcc coalesces the %hi into the load-dest $t0 (`lui $t0,%hi; lw $t0,%lo($t0)`), which shuffles
//   the beqz/andi placement; plus (b) one commutative addu operand-order reg-diff on the attrNew base
//   (`addu $v1,$v0,$t2` vs `$t2,$v0`). Both are unforceable from C / permuter-territory (the permuter
//   only worsened 180). USER-GRANTED via the standing g_pChannelTakenFirst head-load-lui rule (<300).
//
// The hard structural work IS solved (kept below): base-array pointers hoisted before the loop (uf/an),
// the type mask precomputed once (`int mtype`), a two-`if goto advance` loop with the edit as the match
// fall-through and a shared `goto ret0`, and the 0x40 audioL/R copy done via __builtin_memcpy of 4 bytes
// to reproduce retail's unaligned lwl/lwr/swl/swr (a plain `*(int*)` copy compiled to aligned lw/sw).
//
// Finds the taken channel of the given type playing soundID and selectively updates its ChannelAttr
// fields per the editMask bits (0x4 spuStartAddr, 0x8 ad/sr, 0x10 pitch, 0x20 reverb, 0x40 audioL/R),
// also OR-ing editMask into the channel's update flags. Returns the channel (0 if not found). Leaf.
//
// Build note: all globals absolute (%hi/%lo). Leaf, so the walk/base pointers use caller-saved $t0-$t2.
// The 0x40 case copies the audioL/audioR word via an unaligned int access (`*(int*)&na->audioL`, whose
// address is only 2-aligned as a short*, and a char*-derived source) → retail's lwl/lwr/swl/swr.

extern struct ChannelStats* D_8009056C;   // g_pChannelTakenFirst
extern int                  D_8008FC6C[]; // g_abChannelUpdateFlags (one int per channel)
extern struct ChannelAttr   D_8008FCCC[]; // g_abChannelAttrNew

int Channel_SearchFX_EditAttr(char type, int soundID, uint editMask, void* newAttr)
{
    struct ChannelStats* channel;
    int mtype = (u_char)type;
    int* uf = D_8008FC6C;
    struct ChannelAttr* an = D_8008FCCC;
    struct ChannelAttr* na;

    channel = D_8009056C;
    if (channel == 0)
        goto ret0;
loop:
    if (channel->type != mtype)
        goto advance;
    if (channel->soundID != soundID)
        goto advance;
    uf[channel->channelID] |= editMask;
    na = &an[channel->channelID];
    if (editMask & 4)
        na->spuStartAddr = *(void**)newAttr;
    if (editMask & 8)
    {
        na->ad = *(u_short*)((char*)newAttr + 4);
        na->sr = *(u_short*)((char*)newAttr + 6);
    }
    if (editMask & 0x10)
        na->pitch = *(u_short*)((char*)newAttr + 8);
    if (editMask & 0x20)
        na->reverb = *(u_short*)((char*)newAttr + 0xA);
    if (editMask & 0x40)
        __builtin_memcpy((char*)na + 0xc, (char*)newAttr + 0xc, 4);
    return (int)channel;
advance:
    channel = channel->next;
    if (channel != 0)
        goto loop;
ret0:
    return 0;
}
