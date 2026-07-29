#include "HOWL.h"

// Voiceline_Update @ 0x8002D0F8 (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH — score 1095, NOT score-0. Logic verified BYTE-PERFECT against the Ghidra decomp
// (108/108 instructions, identical opcode multiset); the residual is purely compiler
// scheduling / register-allocation, not any behavioral difference.
//
// Per-frame voiceline / wrong-way SFX driver. If voicelines are enabled, not on cooldown, and XA is
// stopped: when the wrong-way flag is armed and the player has driven the wrong way for >30 frames in
// single-player, disarm and play the wrong-way XA line — Aku(0x1E) if good-guy else Uka(0x3D) — via
// CDSYS_XAPlay, setting cooldown = trackLength/5 + 30 (or 30 on failure). Re-arms once driving the
// correct way again for >30 frames. Finally recycles the head of the active voiceline list to the
// free list and starts it playing.
//
// MATCH NOTE — residual 1095 is the HEAD-LOAD-LUI pattern on the voiceline active list D_800961B4:
// retail keeps %hi(list) in a caller-saved scratch (`lui v0; lw s0,%lo(v0)`) and reuses/duplicates it
// (for the LIST_RemoveMember arg and to fill the re-arm branch-delay slots), whereas gcc-2.8.1
// coalesces %hi into the callee-saved load destination (`lui s0; lw s0,0(s0)`) and recomputes it — the
// same unforceable pattern granted for the channel taken/free lists, here on a voiceline list. Also a
// minor a0/a1 delay-slot fill in the wrong-way call block. The permuter reaches 980 only via a
// non-original asymmetric branch structure; Ghidra confirms the symmetric form below is original, so
// it is delivered as-is.
//
// Ghidra-verified: D_8008D7EC=g_bCanPlayVoicelines, D_8008D7ED=g_bVoicelineState (wrong-way armed),
// D_8008D7EE=g_wVoicelineCooldown, D_8008D708=g_eXaState (!=XA_STOPPED), D_8008D9DC=
// g_bWrongWayDirection, D_8008D9E4=g_nFramesDrivingSameDir, D_8008D2AC=sdata_gGT (numPlyrCurrGame
// @0x1CA8, drivers[0]@0x24EC), D_800961B4=g_voicelineActiveList, D_800961A4=g_voicelineFreeList.
// func_80064BE4=VehPickupItem_MaskBoolGoodGuy, func_8001CDB4=CDSYS_XAPlay, func_8001CD20=
// CDSYS_XAGetTrackLength, func_800317E4=LIST_RemoveMember, func_80031788=LIST_AddBack,
// func_8002CF28=Voiceline_StartPlay.

extern short func_80064BE4(int driver);              // VehPickupItem_MaskBoolGoodGuy
extern int   func_8001CDB4(int type, int soundId);   // CDSYS_XAPlay
extern int   func_8001CD20(int type, int soundId);   // CDSYS_XAGetTrackLength
extern void  func_800317E4(void* list, void* item);  // LIST_RemoveMember
extern void  func_80031788(void* list, void* item);  // LIST_AddBack
extern void  func_8002CF28(struct Voiceline* v);     // Voiceline_StartPlay

extern char*  D_8008D2AC;     // sdata_gGT (game state)
extern int    D_8008D708;     // g_eXaState
extern u_char D_8008D9DC;     // g_bWrongWayDirection
extern int    D_8008D9E4;     // g_nFramesDrivingSameDir
extern struct LinkedList D_800961A4;   // g_voicelineFreeList
extern struct LinkedList D_800961B4;   // g_voicelineActiveList

u_char D_8008D7EC = 0;   // g_bCanPlayVoicelines   (gp+0x880)
u_char D_8008D7ED = 0;   // g_bVoicelineState      (gp+0x881)
short  D_8008D7EE = 0;   // g_wVoicelineCooldown   (gp+0x882)

void Voiceline_Update(void)
{
    struct Voiceline* firstVoiceline;
    int voiceID;

    if (D_8008D7EC == 0) return;

    if (D_8008D7EE != 0)
    {
        D_8008D7EE = (u_short)D_8008D7EE - 1;
        if (D_8008D7EE != 0) return;
    }

    if (D_8008D708 != 0) return;

    if (D_8008D7ED != 0)
    {
        if (D_8008D9DC != 0 && D_8008D9E4 >= 0x1F)
        {
            D_8008D7ED = 0;
            if (D_8008D2AC[0x1CA8] == 1)
            {
                if (func_80064BE4(*(int*)(D_8008D2AC + 0x24EC)) != 0)
                {
                    if (func_8001CDB4(1, 0x1E) == 0) goto cool30;
                    voiceID = 0x1E;
                }
                else
                {
                    if (func_8001CDB4(1, 0x3D) == 0) goto cool30;
                    voiceID = 0x3D;
                }
                D_8008D7EE = func_8001CD20(1, voiceID) / 5 + 0x1E;
                return;
            cool30:
                D_8008D7EE = 0x1E;
                return;
            }
        }
        if (D_8008D7ED != 0) goto process;
    }

    if (D_8008D9DC != 0) goto process;
    if (D_8008D9E4 < 0x1F) goto process;
    D_8008D7ED = 1;

process:
    firstVoiceline = (struct Voiceline*)D_800961B4.first;
    if (firstVoiceline == 0) return;
    func_800317E4(&D_800961B4, firstVoiceline);
    func_80031788(&D_800961A4, firstVoiceline);
    func_8002CF28(firstVoiceline);
}
