// SCORE 0 — byte-perfect match vs SCUS_944.26.exe (926/NTSC-U), func @ 0x80028690.
//
// OtherFX_Modify — live-edits a currently-playing OtherFX instance: recomputes
// scaled volume (vol_FX, or vol_Voice when OtherFX.flags&4) and distorted pitch
// (g_anDistortConstOtherFX[distort], 0x80 = none), then Channel_SearchFX_EditAttr(1,
// soundId, 0x70, &attr) under a critical section and writes echo/vol/distort/LR back
// to the live ChannelStats (@0xE/0xF/0x10/0x11). Returns 0 if audio disabled or
// soundId out of range, else 1.  Structure per the Ghidra decomp (binary-authoritative).
//
// Matching notes — the two do{}while(0) scopes and the lrTmp/volMulTmp/searchType
// temps are REQUIRED to reproduce ND's gcc-2.8.1 register allocation (the echo/vol/LR
// saved-register pick). Verified match-necessary: dropping the scopes regresses 0->110.

#include "HOWL.h"

extern u_char D_8008D05C;                   // boolAudioEnabled
extern struct HowlHeader* D_8008D7C0;       // ptrHowlHeader (->numOtherFX @0x14)
extern struct OtherFX*    D_8008D7D8;       // howl_metaOtherFX[]
extern u_char D_8008D7AC;                   // vol_FX scale
extern u_char D_8008D7BC;                   // vol_Voice scale (when OtherFX.flags & 4)
extern int    D_800829AC[];                 // g_anDistortConstOtherFX[]

extern void func_8002B540(struct ChannelAttr*, uint, uint);  // Channel_SetVolume
extern void func_8002B4D0(void);            // Smart_EnterCriticalSection
extern void func_8002B508(void);            // Smart_ExitCriticalSection
extern struct ChannelStats* func_8002B898(int, uint, int, struct ChannelAttr*); // Channel_SearchFX_EditAttr

int OtherFX_Modify(uint soundId, uint flags)
{
    struct ChannelStats* liveChannel;
    struct OtherFX* pMeta;
    short sPitch;
    uint echo;
    uint vol;
    uint distort;
    int  lrTmp;         // int LR-alias (regalloc; see matching notes)
    uint LR;
    struct ChannelAttr attr;
    u_char bVolScale;
    uint volMulTmp;     // regalloc temp
    int  searchType;    // regalloc temp (holds the 0x70 arg)
    uint volArg;

    if ((D_8008D05C == 0) || (D_8008D7C0->numOtherFX <= (soundId & 0xffff)))
        return 0;

    pMeta = &D_8008D7D8[soundId & 0xffff];
    echo = flags >> 0x18;
    do
    {
        vol = (flags >> 0x10) & 0xff;
        distort = (flags >> 8) & 0xff;
        lrTmp = flags & 0xff;
        LR = lrTmp;
        if ((pMeta->flags & 4) != 0)
            bVolScale = D_8008D7BC;
        else
            bVolScale = D_8008D7AC;
        volMulTmp = pMeta->volMul;
        volArg = ((uint)bVolScale * volMulTmp * vol) >> 10;
    } while (0);

    if (distort != 0x80)
        sPitch = (short)((uint)pMeta->pitch * D_800829AC[distort] >> 0x10);
    else
        sPitch = pMeta->pitch;

    attr.reverb = echo;
    func_8002B540(&attr, volArg, LR);
    searchType = 0x70;
    do
    {
        attr.pitch = sPitch;
        attr.reverb = echo;
        func_8002B4D0();
        liveChannel = func_8002B898(1, soundId, searchType, &attr);
        if (liveChannel != 0)
        {
            liveChannel->echo = echo;
            liveChannel->vol = vol;
            liveChannel->distort = distort;
            liveChannel->LR = LR;
        }
        func_8002B508();
        return 1;
    } while (0);
}
