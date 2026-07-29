#include "HOWL.h"

// Voiceline_StartPlay @ 0x8002CF28 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH (perfect).
// Reverse-engineered entirely from the asm (Ghidra MCP was down, no in-repo decomp) — matched in one pass.
//
// Backs up the requested voiceline node into D_80096194, then picks which voiceline "set" to play: normally
// priorityTable[id], but under a special game-state (gameState[0] < 0) for id 0xA..0xF on channels 8..0xB or
// 0xF it picks a random set index (rand()&3)+4. Looks up g_pVoicelineSets[channel][setIndex]; if that set is
// empty, calls Voiceline_StopAll (#93); otherwise picks a random sound-id from the set (rand() % count) and,
// if it passes func_8001CDB4, plays it via func_8001CD20 and sets the cooldown to duration/5 + 0x1E (else 0x1E).
//
// MATCH NOTES: (1) the cooldown divisor is /5 (retail magic 0x66666667 + `sra 1`), not /10; (2) the empty-set
// Voiceline_StopAll must be OUT-OF-LINE — structuring as `if (count != 0) { play; return; } StopAll();` makes
// the play block the fall-through and StopAll the branched-to tail, matching retail; (3) v->id is read as
// u_short for the range check but signed short for the priorityTable index; (4) the sound-id is read from the
// set for BOTH callees (not cached) — retail re-reads it across func_8001CDB4.
//
// ABS extern: gameState=D_8008D2AC(char*), priorityTable=D_800838DC(u8[]), prngSeed=D_8008D058(int; same
// rotr(seed,3)*5+1 PRNG as #95), g_pVoicelineSets=D_8008389C(VoicelineSet*[] per channel), currentVoiceline=
// D_80096194(Voiceline). GP-rel DEFINED (GP_VALUE=0x8008CF6C): voicelineCooldown=D_8008D7EE(0x882,short).
// Callees: func_8001CDB4 voiceline-check, func_8001CD20 voiceline-play(->duration), func_8002CB44
// Voiceline_StopAll(#93).

extern int  func_8001CDB4(int a, int soundId);   // voiceline check
extern int  func_8001CD20(int a, int soundId);   // voiceline play -> duration
extern void func_8002CB44(void);                 // Voiceline_StopAll (#93)

extern char*  D_8008D2AC;     // gameState ptr
extern u_char D_800838DC[];   // priorityTable
extern int    D_8008D058;     // PRNG seed

extern struct VoicelineSet* D_8008389C[];   // per-channel voiceline-set table
extern struct Voiceline     D_80096194;     // current voiceline backup

short D_8008D7EE = 0;   // voicelineCooldown (gp+0x882)

void Voiceline_StartPlay(struct Voiceline* v)
{
    int a1;
    struct VoicelineSet* set;
    int count;
    int idx;

    D_80096194 = *v;

    if (*(int*)D_8008D2AC < 0
        && (uint)((u_short)v->id - 0xA) < 6
        && ((uint)(v->channel - 8) < 4 || v->channel == 0xF))
    {
        D_8008D058 = (((uint)D_8008D058 >> 3) + (D_8008D058 << 29)) * 5 + 1;
        a1 = (D_8008D058 & 3) + 4;
    }
    else
    {
        a1 = D_800838DC[v->id];
    }

    set = &D_8008389C[v->channel][a1];
    count = set->count;
    if (count != 0)
    {
        D_8008D058 = (((uint)D_8008D058 >> 3) + (D_8008D058 << 29)) * 5 + 1;
        idx = (uint)D_8008D058 % count;
        if (func_8001CDB4(2, set->soundIds[idx]) != 0)
            D_8008D7EE = func_8001CD20(2, set->soundIds[idx]) / 5 + 0x1E;
        else
            D_8008D7EE = 0x1E;
        return;
    }
    func_8002CB44();
}
