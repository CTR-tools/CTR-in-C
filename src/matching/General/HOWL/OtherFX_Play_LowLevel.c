// SCORE 0 — byte-perfect match vs SCUS_944.26.exe (926/NTSC-U), func @ 0x800284D0.
//
// OtherFX_Play_LowLevel — starts a one-shot non-engine SFX and returns its handle
// ((CountSounds()<<16)|id), or 0 if audio is disabled, the id is out of range, the
// SPU slot is unloaded, or (boolAntiSpam==2 and the sound is already playing).
// flags: LR@bits0-7, distort@8-15, vol@16-23, echo/reverb@24-31.
//
// Structure follows the Ghidra decomp (binary-authoritative), NOT the reference
// decomp `h04`: on channel==0 the binary only skips the populate block and then
// returns channel->soundID unconditionally (h04's early `return 0` would add a branch).
//
// Matching notes — three seemingly-redundant copies are REQUIRED to reproduce ND's
// exact gcc-2.8.1 register allocation; each was verified (removing any regresses):
//   * `sid = soundID`      raw copy the target keeps in $s2 across the masks; routing
//                          every `& 0xffff` through it reproduces the mask recomputes
//                          and the bulk of the allocation (score 300 -> 60).
//   * `bas = boolAntiSpam`  fixes the prologue arg-save order — s2(soundID) pair must
//                          save before s7(boolAntiSpam) pair (score 60 -> 30).
//   * `volume` -> `nvol`    the volume copy fixes the volume/LR saved-reg pick s5/s6
//                          (score 30 -> 0).

#include "HOWL.h"

extern u_char D_8008D05C;                   // boolAudioEnabled
extern struct HowlHeader* D_8008D7C0;       // ptrHowlHeader
extern struct OtherFX*    D_8008D7D8;       // howl_metaOtherFX[]
extern struct SpuAddr*    D_8008D7DC;       // howl_spuAddrs[]
extern struct GameTracker* D_8008D2AC;      // gGT

extern void func_8002C424(struct OtherFX*, struct ChannelAttr*, u_int, u_int, u_int); // howl_InitChannelAttr_OtherFX
extern void func_8002B4D0(void);            // Smart_EnterCriticalSection
extern void func_8002B508(void);            // Smart_ExitCriticalSection
extern int  func_8002B5B4(int);             // Channel_FindSound
extern struct ChannelStats* func_8002B608(int, u_char, int, struct ChannelAttr*);     // Channel_AllocSlot_AntiSpam
extern int  func_8002843C(void);            // CountSounds

int OtherFX_Play_LowLevel(u_int soundID, u_char boolAntiSpam, u_int flags)
{
    struct ChannelStats* channel;
    int count;
    struct OtherFX* ptrOtherFX;
    u_int sid;      // raw soundID (see matching notes)
    u_char bas;     // raw boolAntiSpam (see matching notes)
    u_int nvol;     // volume copy (see matching notes)
    u_int LR;
    u_int distortion;
    u_int volume;
    u_int echo;
    struct ChannelAttr channelAttr;

    sid = soundID;
    bas = boolAntiSpam;
    if (D_8008D05C == 0) return 0;
    if ((sid & 0xffff) >= D_8008D7C0->numOtherFX) return 0;

    ptrOtherFX = &D_8008D7D8[sid & 0xffff];
    if (D_8008D7DC[ptrOtherFX->spuIndex].spuAddr == 0) return 0;

    volume = (flags >> 0x10) & 0xff;
    nvol = volume;
    LR = flags & 0xff;
    distortion = (flags >> 8) & 0xff;
    echo = flags >> 0x18;

    func_8002C424(ptrOtherFX, &channelAttr, nvol, LR, distortion);
    channelAttr.reverb = echo;

    func_8002B4D0();
    if ((bas == 2) && (func_8002B5B4(sid & 0xffff) != 0))
    {
        func_8002B508();
        return 0;
    }

    channel = func_8002B608(sid & 0xffff, bas, 0x7c, &channelAttr);
    if (channel != 0)
    {
        if ((ptrOtherFX->flags & 2) != 0)
        {
            channel->flags |= 4;
        }
        channel->type = 1;
        channel->unk2 = 0;
        channel->echo = echo;
        channel->vol = nvol;
        channel->distort = distortion;
        channel->LR = LR;
        channel->timeLeft = ptrOtherFX->duration;
        count = func_8002843C();
        channel->soundID = (count << 0x10) | (sid & 0xffff);
        channel->startFrame = D_8008D2AC->frameTimer_MainFrame_ResetDB;
    }
    func_8002B508();
    return channel->soundID;
}
