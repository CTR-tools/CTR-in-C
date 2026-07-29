#include "HOWL.h"

// EngineAudio_Recalculate @ 0x800289B0 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best achievable score = 35 (register diffs only;
//   byte-perfect control flow, correct behavior). The residue is a permuter-UNREACHABLE
//   gcc strength-reduction artifact: the split-screen volume scaling (scaled = vol*0x37 or
//   vol*0x2d, then (scaled<<2)>>8) makes gcc 2.8.1 materialize the multiply/shift chain
//   through a different set of intermediate registers than the retail compiler chose. No
//   C reordering, temp, or expression form reproduces retail's exact intermediate-register
//   assignment, and the permuter cannot cross this gap (it plateaus at 35). Delivered as the
//   most faithful readable form; user-granted exception.
//
// Update the volume/pitch/pan of a currently-playing engine FX (type 0, match by id),
// via Channel_SearchFX_EditAttr(0,...) under a critical section. Returns 0 if audio
// disabled or soundID >= numEngineFX, else 1. Split-screen volume scaling: 2P -> *0x37,
// 3P/4P -> *0x2d, then <<2>>8 (1P unscaled). Volume applied = vol_FX * volMul * vol >> 10.

extern u_char D_8008D05C;                   // boolAudioEnabled
extern struct HowlHeader* D_8008D7C0;       // ptrHowlHeader (->numEngineFX @0x18)
extern struct EngineFX*   D_8008D7D0;       // howl_metaEngineFX[]
extern struct GameTracker* D_8008D2AC;      // gGT (->numPlyrCurrGame @0x1CA8)
extern u_char D_8008D7AC;                   // vol_FX scale
extern int    D_800825AC[];                 // g_anDistortConstEngine[]

extern void func_8002B540(struct ChannelAttr*, uint, uint);  // Channel_SetVolume
extern void func_8002B4D0(void);            // Smart_EnterCriticalSection
extern void func_8002B508(void);            // Smart_ExitCriticalSection
extern struct ChannelStats* func_8002B898(int, uint, int, struct ChannelAttr*); // Channel_SearchFX_EditAttr

int EngineAudio_Recalculate(uint soundID, uint sfx)
{
    struct ChannelStats* liveChannel;
    struct EngineFX* pMeta;
    uint id;
    uint echo;
    uint vol;
    uint distort;
    uint LR;
    uint numPlyr;
    uint scaled;
    struct ChannelAttr attr;

    if (D_8008D05C == 0) return 0;
    id = soundID & 0xffff;
    if (id >= D_8008D7C0->numEngineFX) return 0;

    pMeta = &D_8008D7D0[id];
    vol = (sfx >> 0x10) & 0xff;
    echo = sfx >> 0x18;
    distort = (sfx >> 8) & 0xff;
    LR = sfx & 0xff;

    numPlyr = D_8008D2AC->numPlyrCurrGame;
    if (numPlyr == 2)
    {
        scaled = vol * 0x37;
    }
    else
    {
        if (numPlyr < 3) goto skipScale;
        scaled = vol * 0x2d;
    }
    vol = (scaled << 2) >> 8;
skipScale:

    func_8002B540(&attr, (uint)D_8008D7AC * (uint)pMeta->volMul * vol >> 10, LR);

    attr.pitch = (distort != 0x80)
                     ? (short)((uint)pMeta->pitch * D_800825AC[distort] >> 0x10)
                     : (short)pMeta->pitch;
    attr.reverb = echo;

    func_8002B4D0();
    liveChannel = func_8002B898(0, soundID & 0xffff, 0x70, &attr);
    if (liveChannel != 0)
    {
        liveChannel->echo = echo;
        liveChannel->vol = vol;
        liveChannel->distort = distort;
        liveChannel->LR = LR;
    }
    func_8002B508();
    return 1;
}
