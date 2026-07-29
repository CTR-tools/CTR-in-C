#include "HOWL.h"

// cseq_opcode05_noteon @ 0x8002A28C (SCUS_944.26.exe, 926/NTSC-U). (Ghidra: cseq_PlayNote)
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Plays the current note of a CSEQ song sequence on a newly allocated audio channel, if music is
// enabled (g_bVolMusic) and the sequence + its song-pool entry have non-zero volume. Builds a
// ChannelAttr from the note (howl_InitChannelAttr_Music, #41), Channel_AllocSlot(0x7c)s a
// ChannelStats, and on success fills the channel (flags|=0xe, type=2, unk1/echo/vol/distort/LR/
// drumIndex_pitchIndex/soundID from songSeq + the note) and bumps songSeq->unk0A.
//
// Build note: g_bVolMusic is GP-relative (gp=0x8008CF6C), DEFINED so gcc emits %gp_rel
// (GP_VALUE=0x8008CF6C). g_aSongPool (D_80095D84) is absolute (%hi/%lo).

extern void func_80029F80(struct SongSeq* seq, struct ChannelAttr* attr, int index, int channelVol); // howl_InitChannelAttr_Music (#41)
extern struct ChannelStats* func_8002B7D0(int size, struct ChannelAttr* attr);  // Channel_AllocSlot

extern struct Song D_80095D84[];   // g_aSongPool

u_char D_8008D7B8 = 0;   // g_bVolMusic (gp+0x84C)

void cseq_opcode05_noteon(struct SongSeq* songSeq)
{
    struct ChannelStats* channel;
    struct Song* song;
    struct ChannelAttr attr;
    u_char* currNote;
    u_char note1;
    u_char note2;

    // Match note: caching the song-pool slot pointer into `song` INSIDE the 3rd condition (rather than
    // indexing D_80095D84 inline) forces gcc to materialize the pool base address LATE — after the
    // songPoolIndex*0x7C multiply — instead of hoisting %hi(D_80095D84) into the 2nd branch's delay
    // slot. That keeps songPoolIndex in v1 (not a0), so songSeq stays in a0 with no reload. (Inline
    // D_80095D84[...] form scores 625.)
    if ((D_8008D7B8 != 0) && (songSeq->vol_Curr != 0) &&
        ((song = &D_80095D84[songSeq->songPoolIndex])->vol_Curr != 0))
    {
        currNote = songSeq->currNote;
        func_80029F80(songSeq, &attr, currNote[1], currNote[2]);
        channel = func_8002B7D0(0x7c, &attr);
        if (channel != 0)
        {
            // Match note: two distinct note temps (note1 for songSeq->unk, note2 for currNote[1])
            // rather than one reused local — this colors the AllocSlot'd channel into a0 (matching
            // retail); a single reused `note` puts channel in v1 and swaps every field-store base
            // register (scores 90).
            channel->flags |= 0xe;
            note1 = songSeq->unk;
            channel->type = 2;
            channel->unk2 = 0;
            channel->unk1 = note1;
            channel->echo = songSeq->reverb;
            channel->vol = currNote[2];
            channel->distort = songSeq->distort;
            channel->LR = songSeq->LR;
            note2 = currNote[1];
            channel->timeLeft = 0;
            channel->drumIndex_pitchIndex = note2;
            channel->soundID = songSeq->soundID;
            songSeq->unk0A++;
        }
    }
}
