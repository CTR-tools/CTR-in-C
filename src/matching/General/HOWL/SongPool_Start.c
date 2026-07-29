#include "HOWL.h"

// SongPool_Start @ 0x8002A730 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 995 (behavior byte-for-byte correct; all
//   ~170 instructions present with matching opcodes). This is a large 8-callee-saved-register function
//   and the residual is entirely gcc register-ALLOCATION: an iSongData<->sequence-0xff s6/s7 coloring
//   swap (retail keeps iSongData in s7 for the per-iteration seq-count re-read and the seq-vol 0xff in
//   s6; gcc reverses them), plus the vol-default 0xff/0xbe select shape and a vol_StepRate scratch reg.
//   Driven 2330 -> 995 via five match-necessary transforms (kept below): (1) empty `do{}while(0)`
//   between the two seq vol=0xff stores (scheduling barrier; removing it = 1945); (2) `new_var`(=1) as
//   the loop-count read offset `iSongData+new_var` (un-hoists the constant 1 out of prologue-s1;
//   removing it = 1300); (3) CHAINED `iSeqIndex = (song->tempo = SongPool_CalculateTempo(...))` (fixes
//   the song<->iSeqIndex s2/s3 swap so song lands in s2 like retail); (4) INVERTED songSet-active vol
//   polarity `if((songSet!=0)&&(active==0)){vol=0}else{vol=0xff}` (retail lays vol=0 inline). The
//   permuter's only sub-995 forms are unusable: caching the seq-count (structurally drops the retail
//   per-iteration `lbu`) or a variable-reuse that corrupts the loop counter. USER-GRANTED exception.
//
// Initializes a song-pool slot to play CSEQ song `songID` and spawns its sequences. Resolves the
// song's CSEQ data block (g_pCseqSongData + g_pCseqSongStartOffset[songID]); if a songSet is supplied
// its sequence count must match (else abort). Sets tpqn/bpm (header bpm + bpmOffset), computes tempo
// via SongPool_CalculateTempo(60, tpqn, bpm) (#50), default volume 0xff for songs 1/2 else 0xbe (and
// forced 0xbe on the NAUGHTY_DOG_CRATE level). Then for each sequence: grabs a free SongSeq
// (SongPool_FindFreeChannel, #49), sets its flags/volume/pan/note pointers (howl_GetNextNote, #38),
// and links it into song->CseqSequences[].
//
// Build note: g_pCseqSongStartOffset (gp+0x848) and g_pCseqSongData (gp+0x85C) are GP-relative,
// DEFINED so gcc emits %gp_rel (GP_VALUE=0x8008CF6C). gGT (D_8008D2AC) is absolute (%hi/%lo).

extern int              SongPool_CalculateTempo(int const60, int tpqn, int bpm);  // #50
extern struct SongSeq*  SongPool_FindFreeChannel(void);                           // #49
extern u_char*          howl_GetNextNote(u_char* stream, uint* outValue);         // #38

extern struct GameTracker* D_8008D2AC;   // gGT

int D_8008D7B4 = 0;   // g_pCseqSongStartOffset (gp+0x848)
int D_8008D7C8 = 0;   // g_pCseqSongData        (gp+0x85C)

void SongPool_Start(struct Song* song, u_short songID, short bpmOffset, int boolLoopAtEnd,
                    uint* songSet, int songSetActiveBits)
{
    u_char uDefaultVol;
    int iSeqIndex;
    struct SongSeq* pSeq;
    u_short* pSeqOffsetTable;
    short sBpm;
    u_char* pSeqData;
    u_short* pSeqDataBase;
    int iSongData;
    u_char bSeqUnk;
    char cPoolIndex;
    int new_var;

    song->flags = 1;
    iSongData = D_8008D7B4;
    song->id = songID;
    iSongData = D_8008D7C8 + *(u_short*)((uint)songID * 2 + iSongData);
    if (songSet != 0)
    {
        if (*songSet != (uint)*(u_char*)(iSongData + 1))
        {
            return;
        }
        song->songSetActiveBits = songSetActiveBits;
    }
    song->tpqn = *(short*)(iSongData + 4);
    sBpm = *(short*)(iSongData + 2) + bpmOffset;
    song->bpm = sBpm;
    iSeqIndex = (song->tempo = SongPool_CalculateTempo(0x3c, (int)song->tpqn, (int)sBpm));
    song->unk10 = 0;
    song->timeSpentPlaying = 0;
    new_var = 1;
    if ((u_short)(songID - 1) < 2)
    {
        uDefaultVol = 0xff;
    }
    else
    {
        uDefaultVol = 0xbe;
    }
    song->vol_Curr = uDefaultVol;
    song->vol_New = uDefaultVol;
    if (D_8008D2AC->levelID == 0x29)
    {
        song->vol_Curr = 0xbe;
        song->vol_New = 0xbe;
    }
    song->vol_StepRate = 1;
    song->numSequences = 0;
    pSeqOffsetTable = (u_short*)(iSongData + 6);
    pSeqDataBase = pSeqOffsetTable + *(u_char*)(iSongData + 1);
    if (((uint)pSeqDataBase & 1) != 0)
    {
        pSeqDataBase = (u_short*)((int)pSeqDataBase + 1);
    }
    if (((uint)pSeqDataBase & 2) != 0)
    {
        pSeqDataBase = pSeqDataBase + 1;
    }
    iSeqIndex = 0;
    if (*(u_char*)(iSongData + 1) != 0)
    {
        do
        {
            pSeqData = (u_char*)((int)pSeqDataBase + (uint)*pSeqOffsetTable);
            pSeq = SongPool_FindFreeChannel();
            if (pSeq != 0)
            {
                pSeq->flags = 1;
                if ((*pSeqData & 1) != 0)
                {
                    pSeq->flags = 5;
                }
                if (boolLoopAtEnd != 0)
                {
                    pSeq->flags = pSeq->flags | 2;
                }
                bSeqUnk = pSeqData[1];
                pSeq->instrumentID = 0;
                pSeq->reverb = 0;
                pSeq->unk = bSeqUnk;
                if ((songSet != 0) &&
                    (((uint)*(u_char*)(songSet[1] + iSeqIndex) & song->songSetActiveBits) == 0))
                {
                    pSeq->vol_Curr = 0;
                    pSeq->vol_New = 0;
                }
                else
                {
                    pSeq->vol_Curr = 0xff;
                    do { } while (0);
                    pSeq->vol_New = 0xff;
                }
                pSeq->vol_StepRate = 1;
                pSeq->distort = 0x80;
                pSeq->LR = 0x80;
                pSeq->unk0A = 0;
                cPoolIndex = song->songPoolIndex;
                pSeq->NoteLength = 0;
                pSeq->NoteTimeElapsed = 0;
                pSeq->firstNote = (char*)(pSeqData + 2);
                pSeq->songPoolIndex = cPoolIndex;
                pSeqData = howl_GetNextNote(pSeqData + 2, (uint*)&pSeq->NoteLength);
                pSeq->currNote = pSeqData;
                song->CseqSequences[(u_char)song->numSequences] = pSeq;
                song->numSequences = song->numSequences + 1;
            }
            iSeqIndex = iSeqIndex + 1;
            pSeqOffsetTable = pSeqOffsetTable + 1;
        } while (iSeqIndex < (int)(uint)*(u_char*)(iSongData + new_var));
    }
}
