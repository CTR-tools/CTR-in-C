#include "HOWL.h"

// Voiceline_PoolInit @ 0x8002C918 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best (natural-code) score = 1050, byte-BEHAVIOR-correct. GRANTED
//   (2026-07-27, user) as an exception. The structure, all three init loops, every store, and the SPU
//   calls match retail; the residual is a gcc-2.8.1 build LICM / register-allocation DIVERGENCE with
//   IDENTICAL behavior (same stores/values, same order): retail hoists the constants 0x80FF and 0x1FC2
//   into callee-saved registers and materializes -1 / 0xFFFF PER-ITERATION, whereas this compiler build
//   hoists -1 into a callee-saved register instead (and materializes 0x80FF per-use). It is NOT a use-count
//   decision (retail hoists the 2-use 0x80FF over the 3-use -1), so it is a compiler-internal choice this
//   build makes differently and which no natural C expresses. The only C lever that touches it — making
//   ChannelStats.ad a u_short so 0x80FF is a single hoistable constant — just makes gcc try to hoist all
//   three constants and spill a walking pointer to a caller-saved reg (worse, 2140), so ChannelStats.ad is
//   kept `short` here.
//
//   Two NATURAL cracks got it from 5670 -> 1050: (1) `int index` (not `char`) so gcc's induction-variable
//   optimization strength-reduces the three array walks to walking pointers instead of per-access multiplies
//   (5670 -> 1200); (2) reusing `&D_80090578` via a local across LIST_Clear + LIST_Init (retail keeps it in
//   one register), 1200 -> 1050.
//
// Resets the whole audio pool at boot: clears the critical-section counter / paused-channel count / cseq
// header ptr, resets the bank allocator, sets audio defaults, clears the free+taken channel lists and
// rebuilds the free list from the 24-entry ChannelStats array, then initializes 24 channels (ChannelStats +
// ChannelAttr + SPU voice ADSR), 2 song-pool slots, and 24 song sequences.
//
// GP-rel DEFINED (GP_VALUE=0x8008CF6C): criticalSectionCount=D_8008D78C(0x820,int),
// numBackup_ChannelStats=D_8008D790(0x824,int), ptrCseqHeader=D_8008D7B0(0x844,int). ABS extern:
// channelFree=D_80090578, channelTaken=D_8009056C (list heads), channelStatsPrev=D_80095E94(ChannelStats[]),
// channelAttrCur=D_8008FE4C(ChannelAttr[]), ChannelUpdateFlags=D_8008FC6C(int[]), songPool=D_80095D84(Song[]),
// songSeq=D_800902CC(SongSeq[]). Callees: func_800292E0 Bank_ResetAllocator(#21), func_8002DC4C
// Audio_SetDefaults, func_80031734 LIST_Clear, func_8003197C LIST_Init, func_80076840 SpuSetReverbVoice,
// func_80076420 SpuSetVoiceADSRAttr.

extern void func_800292E0(void);   // Bank_ResetAllocator (#21)
extern void func_8002DC4C(void);   // Audio_SetDefaults
extern void func_80031734(void* list);                                   // LIST_Clear
extern void func_8003197C(void* list, void* array, int stride, int count); // LIST_Init
extern void func_80076840(int voice, int reverbMask);                    // SpuSetReverbVoice
extern void func_80076420(int voice, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8); // SpuSetVoiceADSRAttr

extern struct ChannelStats* D_80090578;   // channelFree  (list head)
extern struct ChannelStats* D_8009056C;   // channelTaken (list head)
extern struct ChannelStats  D_80095E94[];  // channelStatsPrev (24 x 0x20)
extern struct ChannelAttr   D_8008FE4C[];  // channelAttrCur   (24 x 0x10)
extern int                  D_8008FC6C[];  // ChannelUpdateFlags
extern struct Song          D_80095D84[];  // songPool (2 x 0x7C)
extern struct SongSeq       D_800902CC[];  // songSeq (24 x 0x1C)

int D_8008D78C = 0;   // criticalSectionCount   (gp+0x820)
int D_8008D790 = 0;   // numBackup_ChannelStats (gp+0x824)
int D_8008D7B0 = 0;   // ptrCseqHeader          (gp+0x844)

void Voiceline_PoolInit(void)
{
    int index;

    D_8008D78C = 0;
    D_8008D790 = 0;
    D_8008D7B0 = 0;

    func_800292E0();
    func_8002DC4C();

    {
        void* freeHead = &D_80090578;
        func_80031734(freeHead);
        func_80031734(&D_8009056C);
        func_8003197C(freeHead, &D_80095E94[0], 0x20, 0x18);
    }

    func_80076840(0, 0xffffff);

    for (index = 0; index < 24; index++)
    {
        struct ChannelStats* stats = &D_80095E94[index];
        D_8008FC6C[index] = 0;

        func_80076420(index, 0, 0xf, 0x7f, 2, 0xf, 5, 1, 3);

        stats->flags = 0;
        stats->channelID = index;
        stats->ad = 0x80ff;
        stats->sr = 0x1fc2;

        {
            struct ChannelAttr* curr = &D_8008FE4C[index];
            curr->spuStartAddr = (void*)-1;
            curr->ad = 0x80ff;
            curr->sr = 0x1fc2;
            curr->pitch = 0xffff;
            curr->reverb = 0xffff;
            curr->audioL = -1;
            curr->audioR = -1;
        }
    }

    for (index = 0; index < 2; index++)
    {
        struct Song* pool = &D_80095D84[index];
        pool->flags = 0;
        pool->songPoolIndex = index;
    }

    for (index = 0; index < 24; index++)
    {
        struct SongSeq* seq = &D_800902CC[index];
        seq->flags = 0;
        seq->soundID = index;
    }
}
