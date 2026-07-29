// SCORE 0 — byte-perfect match vs SCUS_944.26.exe (926/NTSC-U), func @ 0x80028880.
// EngineAudio_InitOnce — allocate and start a continuous engine-FX channel once (type 0).
// Returns 0 if audio disabled, soundID >= numEngineFX, or the sample's SPU slot is
// unloaded. Builds a ChannelAttr via howl_InitChannelAttr_EngineFX, then under a critical
// section Channel_AllocSlot(0x7c,&attr) and populates the live ChannelStats (flags|=4 = engine/loop).
//
// Note: the masked id goes in its own `id` var (NOT by reassigning the soundID param);
// that lets gcc mask a0 straight into $s1 (andi s1,a0,0xffff) rather than emitting a
// param-preservation copy (move s1,a0; andi s1,s1). Verified: reassigning regresses.

#include "HOWL.h"

extern u_char D_8008D05C;                   // boolAudioEnabled
extern struct HowlHeader* D_8008D7C0;       // ptrHowlHeader (->numEngineFX @0x18)
extern struct EngineFX*   D_8008D7D0;       // howl_metaEngineFX[]
extern struct SpuAddr*    D_8008D7DC;       // howl_spuAddrs[]

extern void func_8002C34C(struct EngineFX*, struct ChannelAttr*, uint, uint, uint); // howl_InitChannelAttr_EngineFX
extern void func_8002B4D0(void);            // Smart_EnterCriticalSection
extern void func_8002B508(void);            // Smart_ExitCriticalSection
extern struct ChannelStats* func_8002B7D0(int, struct ChannelAttr*);                 // Channel_AllocSlot

int EngineAudio_InitOnce(uint soundID, uint flags)
{
    struct ChannelStats* channel;
    struct EngineFX* pMeta;
    uint id;
    uint echo;
    uint vol;
    uint distort;
    uint LR;
    struct ChannelAttr attr;

    if (D_8008D05C == 0) return 0;
    id = soundID & 0xffff;
    if (id >= D_8008D7C0->numEngineFX) return 0;
    pMeta = &D_8008D7D0[id];
    if (D_8008D7DC[pMeta->spuIndex].spuAddr == 0) return 0;

    echo = flags >> 0x18;
    vol = (flags >> 0x10) & 0xff;
    distort = (flags >> 8) & 0xff;
    LR = flags & 0xff;
    func_8002C34C(pMeta, &attr, vol, LR, distort);
    attr.reverb = echo;

    func_8002B4D0();
    channel = func_8002B7D0(0x7c, &attr);
    if (channel != 0)
    {
        channel->type = 0;
        channel->unk2 = 0;
        channel->echo = echo;
        channel->vol = vol;
        channel->distort = distort;
        channel->LR = LR;
        channel->timeLeft = 0;
        channel->soundID = id;
        channel->flags |= 4;
    }
    func_8002B508();
    return channel != 0;
}
