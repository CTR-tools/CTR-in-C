#include "HOWL.h"

// Audio_SetState @ 0x8002D2F4 (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH — score 400, NOT score-0. Structure verified against the Ghidra decomp (case ordering,
// shared play tail, case-9 inline-play-then-mod, case-16 xaType logic all identical).
//
// Central audio state-machine dispatcher (jump table on state, 16 cases). Reversed from asm + in-repo
// decomp h100, structure cross-checked vs Ghidra.
//
// MATCH NOTE — the sole residual is case 16's `xaType = MUSIC; if (0x3D < xaIndex) xaType = EXTRA;`
// (Ghidra confirms this exact source): gcc-2.8.1 if-converts it to a `xori` setcc (2 insns) whereas
// retail emits the branch form (`slti; bnez play; move a0,zero (delay); li a0,1`). Pure if-conversion
// codegen tie-break on byte-behavior-correct source. The permuter reaches 15 only via semantically
// BROKEN forms (reusing an uninitialized XA_index as the value / to zero unrelated globals), which the
// natural-code rule forbids; the natural form is delivered as-is.
//
// KEY LEVER: gcc lays switch case-bodies in a specific .text order (NOT numeric) — 1, 5, 2/7, 9, 10,
// 11, 13, 14, 12/15, 16 — so the source `case` blocks are ordered to match (1300 -> 400).
//
// Ghidra names: func_8001CF98=CDSYS_XAPauseRequest, func_8002E4C0=Music_Restart, func_8002C8A8=
// howl_StopAudio(#89), func_80029258=CseqMusic_StopAll(#20), func_8002E350=Music_Adjust, func_8001CDB4=
// CDSYS_XAPlay, func_8002E4EC=Music_Stop, func_8002CBB4=Voiceline_ToggleEnable, func_8002E418=
// Music_LowerVolume, func_8002E46C=Music_RaiseVolume. D_8008D2AC=sdata_gGT (levelID@0x1A10), D_8008D9DC=
// g_bWrongWayDirection, D_8008D9E4=g_nFramesDrivingSameDir, D_8008D068=g_advHubSongSet, D_8008D7F2=
// g_wDesiredXARaceIntroIdx, D_8008D7F8=g_wDesiredXARaceEndIdx, D_8008D7FC=g_bNeedXASeek.

extern void func_8001CF98(void);                    // CDSYS_XAPauseRequest
extern void func_8002E4C0(void);                    // Music_Restart
extern void func_8002C8A8(int, int, int);           // howl_StopAudio (#89)
extern void func_80029258(void);                    // CseqMusic_StopAll (#20)
extern void func_8002E350(int, int, void*, int);    // Music_Adjust
extern int  func_8001CDB4(int type, int index);     // CDSYS_XAPlay
extern void func_8002E4EC(void);                    // Music_Stop
extern void func_8002CBB4(int enable);              // Voiceline_ToggleEnable
extern void func_8002E418(void);                    // Music_LowerVolume
extern void func_8002E46C(void);                    // Music_RaiseVolume

extern char*  D_8008D2AC;   // sdata_gGT (levelID @ +0x1A10)
extern u_char D_8008D9DC;   // g_bWrongWayDirection
extern int    D_8008D9E4;   // g_nFramesDrivingSameDir
extern int    D_8008D068;   // g_advHubSongSet (passed by address)

short  D_8008D7F2 = 0;   // gp+0x886 g_wDesiredXARaceIntroIdx
short  D_8008D7F8 = 0;   // gp+0x88C g_wDesiredXARaceEndIdx
u_char D_8008D7FC = 0;   // gp+0x890 g_bNeedXASeek

void Audio_SetState(u_int state)
{
    int XA_type;
    int XA_index;
    int level;

    switch (state)
    {
    case 1:
        D_8008D7FC = 0;
        func_8001CF98();
        func_8002E4C0();
        func_8002C8A8(1, 0, 1);
        break;
    case 5:
        func_80029258();
        level = *(int*)(D_8008D2AC + 0x1A10);
        if ((u_int)(level - 0x19) < 5)
            func_8002E350(0, 0, &D_8008D068, 1 << (level - 0x19));
        break;
    case 2:
    case 7:
        func_80029258();
        func_8002E350(0, 0, 0, 0);
        break;
    case 9:
        func_8001CDB4(0, D_8008D7F2);
        D_8008D7F2 = (D_8008D7F2 + 1) % 4;
        break;
    case 10:
        func_8002E4EC();
        func_80029258();
        break;
    case 11:
        D_8008D9DC = 0;
        D_8008D9E4 = 0;
        func_8002CBB4(1);
        break;
    case 13:
        D_8008D7FC = 0;
        func_8002E418();
        XA_type = 0;
        XA_index = 6;
        goto play_xa;
    case 14:
        D_8008D9DC = 0;
        D_8008D9E4 = 0;
        func_8002E46C();
        func_8002CBB4(1);
        break;
    case 12:
    case 15:
        func_8002CBB4(0);
        D_8008D7FC = 1;
        break;
    case 16:
        D_8008D7FC = 0;
        func_8002E4C0();
        XA_index = D_8008D7F8;
        XA_type = 0;
        if (61 < XA_index)
            XA_type = 1;
    play_xa:
        func_8001CDB4(XA_type, XA_index);
    }
}
