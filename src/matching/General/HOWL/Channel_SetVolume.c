#include "HOWL.h"

// Channel_SetVolume @ 0x8002B540 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Sets a channel's L/R SPU volumes from a combined volume + pan position (lr), clamped to 0x3FFF. In
// stereo (g_bVolumeMode == 1) the pan is applied via g_abStereoPanTable (L = table[0xFF - lr],
// R = table[lr], each scaled *volume >> 8); in mono both channels get the full clamped volume.
//
// Build note: g_bVolumeMode (gp+0x860) is GP-relative u_char, DEFINED so gcc emits %gp_rel
// (GP_VALUE=0x8008CF6C). g_abStereoPanTable (D_80082DAC, u_char[256]) is absolute. Leaf.

extern u_char D_80082DAC[];  // g_abStereoPanTable

u_char D_8008D7CC = 0;   // g_bVolumeMode (gp+0x860)

void Channel_SetVolume(struct ChannelAttr* channelAttr, uint volume, int lr)
{
    if (0x3fff < volume)
        volume = 0x3fff;
    if (D_8008D7CC == 1)
    {
        channelAttr->audioL = (short)(volume * D_80082DAC[0xff - lr] >> 8);
        channelAttr->audioR = (short)(volume * D_80082DAC[lr] >> 8);
    }
    else
    {
        channelAttr->audioL = (short)volume;
        channelAttr->audioR = (short)volume;
    }
}
