#include "HOWL.h"

// Garage_PlayFX @ 0x80030404 (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH -- score 25, NOT score-0 (auto-granted register/const-fold tie-break, 2026-07-29).
// Behavior byte-EXACT (45 insns). Play a garage/character-select SFX for character `charId` (only if
// charId < 8 = PINSTRIPE). If soundId == 0xF6 (BIRD_RANDOM), advance the audio LCG g_dwAudioRNG
// (D_8008D058) and pick one of three bird noises 0xF3/0xF4/0xF5 (rng%3). Plays via
// OtherFX_Play_LowLevel(func_800284D0)(soundId&0xFFFF, antiSpam=1, flags) where
// flags = ((u_char)pool[charId].volume << 16) | (u_char)pool[charId].LR | 0x8000(distort 0x80).
// pool = D_800962E4 (GarageFX, stride 0xC). Verified vs Ghidra decomp + in-repo h140.
//
// MATCH NOTE -- residual 25 (5 reg): in the flag word, gcc-2.8.1 canonicalizes `X | const` and folds
// 0x8000 into the volume<<16 term (kept in a2, the arg reg) with LR in a temp; retail seeds a2 with
// (LR | 0x8000) and keeps volume<<16 in a temp (v1), i.e. `or a2, v1, a2`. The two forms are coupled:
// every source grouping that makes (LR|0x8000) the a2 accumulator forces gcc to load LR before volume
// (retail loads volume first), scoring 230; every form with retail's load order folds the const into
// volume, scoring 25-30. gcc-2.8.1 cannot produce volume-first-load + LR-accumulator together. The
// flag VALUE is identical either way -- a pure #34-class register/const-fold tie-break, unforceable
// from C (5 manual variants + ~35 permuter iterations all plateau at 25; permuter finds nothing lower).

struct GarageFX { char gsp_curr; char gsp_prev; short volume; short LR; int audioPtr; };

extern u_int func_800284D0(u_int soundID, int a1, u_int flags);   // OtherFX_Play_LowLevel
extern struct GarageFX D_800962E4[];   // garageSoundPool[8]
extern u_int D_8008D058;               // g_dwAudioRNG (LCG state, absolute)

void Garage_PlayFX(u_int soundId, int charId)
{
    if (charId < 8)
    {
        if (soundId == 0xF6)
        {
            D_8008D058 = ((D_8008D058 >> 3) + D_8008D058 * 0x20000000) * 5 + 1;
            soundId = D_8008D058 % 3 + 0xF3;
        }
        func_800284D0(soundId & 0xFFFF, 1,
                      ((u_char)D_800962E4[charId].LR | 0x8000)
                      | ((u_char)D_800962E4[charId].volume << 16));
    }
}
