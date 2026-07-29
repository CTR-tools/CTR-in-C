#include "HOWL.h"

// Audio_SetReverbMode @ 0x8002DCAC (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH — score 100, NOT score-0. Behavior byte-correct; residual is a single delay-slot nop.
// If audio enabled: pick a reverb mode from reverbModeBossID[bossID] (boss race) or reverbMode[levelID]
// (levelID<30 else 4), then SetReverbMode (#11). Verified vs Ghidra decomp + h106.
//
// MATCH NOTE — residual 100: retail leaves the boss-branch `beqz v0,exit` (bossID>=6 return) delay slot
// as `nop`; my gcc fills it with the reverbModeBossID table `lui`. Pure delay-slot scheduling tie-break
// (the nonboss levelID branch DOES fill its delay identically). The permuter reaches 0 ONLY via a
// `volatile int` return-type hack (forbidden); natural variants (bossID>=6, u_int isBossRace) all 100.
// Auto-granted as a <300 clean-scheduling plateau.

extern void func_80028BBC(int mode);   // SetReverbMode (#11)

extern u_char D_8008D05C;     // boolAudioEnabled
extern u_char D_8008D070[];   // reverbModeBossID
extern u_char D_80083914[];   // reverbMode (data.reverbMode)

void Audio_SetReverbMode(int levelID, int isBossRace, int bossID)
{
    int reverb;

    if (D_8008D05C == 0) return;
    if (isBossRace != 0)
    {
        if (5 < bossID) return;
        reverb = D_8008D070[bossID];
    }
    else
    {
        if (levelID < 0x1E)
            reverb = D_80083914[levelID];
        else
            reverb = 4;
    }
    func_80028BBC(reverb);
}
