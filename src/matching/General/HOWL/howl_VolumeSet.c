#include "HOWL.h"

// howl_VolumeSet @ 0x8002B130 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Sets the howl volume for a type (0 = EngineFX, 1 = Music, 2 = OtherFX). If the value actually
// changed, stores it, enters a Smart critical section, updates every channel of that type via the
// matching UpdateChannelVol_*_All, then exits the critical section. No-op if unchanged or a bad type.
//
// Match note: written as an explicit goto-dispatch (checking all three types up front, branching to
// separate labeled blocks, converging on a shared ExitCrit) — the same code-layout lever as #65
// howl_VolumeGet. An if/else-if chain makes gcc INLINE each case body behind a `bne` skip (score 950);
// the goto-dispatch reproduces retail's beq-to-labeled-block structure. `volume` is u_char so the
// change-check compiles to `andi 0xff` (retail hoists that mask into the type-2 branch's delay slot and
// keeps volume raw in $a2 for the sb store); labels ordered engine/music/other to match block addresses.
//
// Build note: g_bVolEngineFX (gp+0x840) / g_bVolMusic (gp+0x84C) / g_bVolOtherFX (gp+0x850) are
// GP-relative, DEFINED so gcc emits %gp_rel (GP_VALUE=0x8008CF6C). Callees: func_8002B4D0
// (Smart_EnterCriticalSection), func_8002B508 (Smart_ExitCriticalSection), func_8002AE64/AF6C/B030
// (UpdateChannelVol_EngineFX_All / _Music_All / _OtherFX_All, #62/#63/#64).

extern void func_8002B4D0(void);  // Smart_EnterCriticalSection
extern void func_8002B508(void);  // Smart_ExitCriticalSection
extern void func_8002AE64(void);  // UpdateChannelVol_EngineFX_All
extern void func_8002AF6C(void);  // UpdateChannelVol_Music_All
extern void func_8002B030(void);  // UpdateChannelVol_OtherFX_All

u_char D_8008D7AC = 0;   // g_bVolEngineFX (gp+0x840)
u_char D_8008D7B8 = 0;   // g_bVolMusic    (gp+0x84C)
u_char D_8008D7BC = 0;   // g_bVolOtherFX  (gp+0x850)

void howl_VolumeSet(int volumeType, u_char volume)
{
    if (volumeType == 1) goto music;
    if (volumeType == 0) goto engine;
    if (volumeType == 2) goto other;
    return;
engine:
    if (D_8008D7AC == volume) return;
    D_8008D7AC = volume;
    func_8002B4D0();
    func_8002AE64();
    goto done;
music:
    if (D_8008D7B8 == volume) return;
    D_8008D7B8 = volume;
    func_8002B4D0();
    func_8002AF6C();
    goto done;
other:
    if (D_8008D7BC == volume) return;
    D_8008D7BC = volume;
    func_8002B4D0();
    func_8002B030();
done:
    func_8002B508();
}
