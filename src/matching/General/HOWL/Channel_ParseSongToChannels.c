#include "HOWL.h"

// Channel_ParseSongToChannels @ 0x8002BBAC (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 580 (from a first-compile 2355), the whole
//   ~188-instruction function byte-for-behavior correct. NO head-load-lui — this is pure gcc register
//   allocation. Four structural fixes landed the bulk: (1) unk10 u16-truncation as a RE-READ
//   (`song->unk10 = *(u_short*)&song->unk10`, lhu) not `(u_short)local` (andi); (2) vol_StepRate read
//   INLINE per add/sub branch, not hoisted; (3) volStepped computed TWO-STEP (`volStepped=volCurr;
//   volStepped ±= vol_StepRate`) to reproduce retail's move copies; (4) the seq-fade `new_var = volNew`
//   copy (permuter-found) used asymmetrically to fix the seq vol_New allocation. The residual ~28
//   register diffs are a SYSTEMATIC gcc register-REUSE-ORDER preference in the SONG fade only: retail
//   reuses $v1 across flags -> unk10-accumulator -> vol_New (reading vol_New late into the dead tempo
//   register), while gcc reads them into $v0/$a0. Every C lever to force this (song-fade new_var2,
//   flags/nflags temps, load reorders) was neutral or worse, and the permuter (which DID crack the
//   seq-fade transform) plateaus at 580 for the song fade. USER-GRANTED (2026-07-26) case-by-case
//   exception after 4+ fires of manual + permuter grinding.
//
// Per-frame CSEQ music sequencer tick. For each of the 2 g_aSongPool songs that is playing (flags bit0)
// and not paused (bit1): accumulate tempo into the play-time (yielding `ticks` whole steps), fade
// vol_Curr toward vol_New by vol_StepRate, and on reaching the target with a pending restart (bit2) do
// SongPool_StopAllCseq + Music_End + clear the bit. Then for each CseqSequences[]: fade the sequence
// volume, and if active advance NoteTimeElapsed by `ticks`; while it passes NoteLength, dispatch the
// current note opcode via g_apCseqOpcodeHandlers[op<11], advance currNote by g_anCseqOpcodeLengths[op]
// (or loop to firstNote on bit3), and decode the next note (howl_GetNextNote). If anything changed,
// UpdateChannelVol_Music_All. (Structure verified vs decomp h79 + binary.)

extern void    func_8002AC0C(struct Song* song);           // SongPool_StopAllCseq
extern void    func_8002E53C(void);                        // Music_End
extern void    func_8002AF6C(void);                        // UpdateChannelVol_Music_All
extern u_char* func_80029DCC(u_char* notePtr, int* pNoteLength);  // howl_GetNextNote

extern struct Song D_80095D84[];                // g_aSongPool
extern void (*D_80083004[])(struct SongSeq*);   // g_apCseqOpcodeHandlers
extern int  D_80083030[];                       // g_anCseqOpcodeLengths

u_char D_8008D05C = 0;   // g_bHowlInitialized (gp+0xF0)
int    D_8008D7B0 = 0;   // g_pCseqHeader      (gp+0x844)

void Channel_ParseSongToChannels(void)
{
    struct Song* song;
    int songIdx;
    int changed;

    if (D_8008D05C == 0)
        return;
    if (D_8008D7B0 == 0)
        return;

    changed = 0;
    songIdx = 0;
    do {
        song = &D_80095D84[songIdx];
        if ((song->flags & 1) != 0 && (song->flags & 2) == 0) {
            uint unk10_total;
            uint ticks;
            int volCurr, volNew, volStepped, boolFinalStep;
            int seqIdx;

            unk10_total = song->unk10 + song->tempo;
            ticks = unk10_total >> 0x10;
            song->unk10 = unk10_total;
            song->timeSpentPlaying = song->timeSpentPlaying + ticks;
            song->unk10 = *(u_short*)&song->unk10;

            volCurr = song->vol_Curr;
            volNew = song->vol_New;
            if (volCurr != volNew) {
                volStepped = volCurr;
                if (volCurr < volNew) {
                    volStepped += song->vol_StepRate;
                    boolFinalStep = volStepped > volNew;
                } else {
                    volStepped -= song->vol_StepRate;
                    boolFinalStep = volStepped < volNew;
                }
                if (boolFinalStep) {
                    volStepped = volNew;
                    if ((song->flags & 4) != 0) {
                        func_8002AC0C(song);
                        func_8002E53C();
                        song->flags = song->flags & 0xfb;
                    }
                }
                song->vol_Curr = volStepped;
                changed = 1;
            }

            if (song->numSequences != 0) {
                seqIdx = 0;
                do {
                    struct SongSeq* seq = song->CseqSequences[seqIdx];

                    volCurr = seq->vol_Curr;
                    volNew = seq->vol_New;
                    if (volCurr != volNew) {
                        int new_var = volNew;
                        volStepped = volCurr;
                        if (volCurr < volNew) {
                            volStepped += seq->vol_StepRate;
                            boolFinalStep = volStepped > new_var;
                        } else {
                            volStepped -= seq->vol_StepRate;
                            boolFinalStep = volStepped < volNew;
                        }
                        if (boolFinalStep)
                            volStepped = new_var;
                        seq->vol_Curr = volStepped;
                        changed = 1;
                    }

                    if ((seq->flags & 1) != 0) {
                        seq->NoteTimeElapsed = seq->NoteTimeElapsed + ticks;
                        while ((uint)seq->NoteLength <= (uint)seq->NoteTimeElapsed) {
                            int opcode;

                            if ((seq->flags & 1) == 0)
                                break;
                            seq->NoteTimeElapsed = seq->NoteTimeElapsed - seq->NoteLength;
                            opcode = *seq->currNote;
                            if ((uint)opcode < 0xb) {
                                int nflags;
                                D_80083004[opcode](seq);
                                nflags = seq->flags;
                                if ((nflags & 1) != 0) {
                                    u_char* notePtr;
                                    if ((nflags & 8) != 0) {
                                        notePtr = (u_char*)seq->firstNote;
                                        seq->flags = nflags & 0xf7;
                                    } else {
                                        notePtr = seq->currNote + D_80083030[opcode];
                                        seq->currNote = notePtr;
                                    }
                                    seq->currNote = func_80029DCC(notePtr, &seq->NoteLength);
                                }
                            }
                        }
                    }

                    seqIdx = seqIdx + 1;
                } while (seqIdx < (int)(uint)song->numSequences);
            }
        }
        songIdx = songIdx + 1;
    } while (songIdx < 2);

    if (changed)
        func_8002AF6C();
}
