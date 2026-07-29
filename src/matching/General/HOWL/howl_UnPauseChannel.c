#include "HOWL.h"

// howl_UnPauseChannel @ 0x8002C64C (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH (perfect).
//
// Rebuilds and re-arms the SPU attributes for one backed-up channel when audio resumes. Dispatches on
// stats->type: 1=OtherFX, 0=EngineFX, 2=Music; each builds a temp ChannelAttr via the matching
// howl_InitChannelAttr_* (#85/#84/#41), passing vol/LR/distort (or drumIndex/vol for Music) from the saved
// ChannelStats. Then g_abChannelUpdateFlags[channelID] |= 0x7e and copies the temp ChannelAttr into
// g_abChannelAttrNew[channelID]. Unknown type returns early.
//
// MATCH NOTES: (1) `type` must be a signed `int` (not u_char) so the type<2 test emits `slti` not `sltiu`.
// (2) The dispatch MUST be a `switch` (not an if/else-if chain): gcc's switch codegen for this {0,1,2}
// case set places all three bodies out-of-line after a self-contained decision tree that jumps to a common
// tail, exactly as retail; an if/else-if chain inlines the type==1 body as fall-through (score 1340).
// (3) soundID (ChannelStats @0x18, int): OtherFX/EngineFX index the meta tables by (u_short)soundID (lhu),
// Music indexes g_aSongSeq by the full int soundID (lw).
//
// GP-rel DEFINED (GP_VALUE=0x8008CF6C): g_pHowlMetaEngineFX=D_8008D7D0(0x864,ptr),
// g_pHowlMetaOtherFX=D_8008D7D8(0x86C,ptr). ABS extern: g_aSongSeq=D_800902CC(SongSeq[],0x1C stride),
// g_abChannelUpdateFlags=D_8008FC6C(int[]), g_abChannelAttrNew=D_8008FCCC(ChannelAttr[],0x10 stride).
// Callees: func_8002C424 howl_InitChannelAttr_OtherFX(#85), func_8002C34C howl_InitChannelAttr_EngineFX(#84),
// func_80029F80 howl_InitChannelAttr_Music(#41).

extern void func_8002C424(struct OtherFX* meta, struct ChannelAttr* attr, int vol, int LR, int distort);
extern void func_8002C34C(struct EngineFX* meta, struct ChannelAttr* attr, int vol, int LR, int distort);
extern void func_80029F80(struct SongSeq* seq, struct ChannelAttr* attr, int index, int channelVol);

extern struct SongSeq    D_800902CC[];   // g_aSongSeq
extern int               D_8008FC6C[];   // g_abChannelUpdateFlags
extern struct ChannelAttr D_8008FCCC[];  // g_abChannelAttrNew (0x10 stride)

struct EngineFX* D_8008D7D0 = 0;   // g_pHowlMetaEngineFX (gp+0x864)
struct OtherFX*  D_8008D7D8 = 0;   // g_pHowlMetaOtherFX  (gp+0x86C)

void howl_UnPauseChannel(struct ChannelStats* stats)
{
    struct ChannelAttr attr;
    int type;

    type = stats->type;
    switch (type)
    {
    case 1:
        func_8002C424(&D_8008D7D8[(u_short)stats->soundID], &attr,
                      stats->vol, stats->LR, stats->distort);
        break;
    case 0:
        func_8002C34C(&D_8008D7D0[(u_short)stats->soundID], &attr,
                      stats->vol, stats->LR, stats->distort);
        break;
    case 2:
        func_80029F80(&D_800902CC[stats->soundID], &attr,
                      stats->drumIndex_pitchIndex, stats->vol);
        break;
    default:
        return;
    }
    D_8008FC6C[stats->channelID] |= 0x7e;
    D_8008FCCC[stats->channelID] = attr;
}
