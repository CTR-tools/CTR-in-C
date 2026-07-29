// SCORE 0 — byte-perfect match vs SCUS_944.26.exe (926/NTSC-U), func @ 0x80028B54.
// EngineAudio_Stop — stop the engine-FX loop for soundID: under a critical section,
// Channel_SearchFX_Destroy(0 /*engine type*/, soundID&0xffff, 0xffffffff). No-op if
// audio disabled or soundID >= numEngineFX.

#include "HOWL.h"

extern u_char D_8008D05C;                   // boolAudioEnabled
extern struct HowlHeader* D_8008D7C0;       // ptrHowlHeader (->numEngineFX @0x18)

extern void func_8002B4D0(void);            // Smart_EnterCriticalSection
extern void func_8002B9B8(int, int, uint);  // Channel_SearchFX_Destroy
extern void func_8002B508(void);            // Smart_ExitCriticalSection

void EngineAudio_Stop(uint soundID)
{
    if ((D_8008D05C != 0) && ((soundID & 0xffff) < D_8008D7C0->numEngineFX))
    {
        func_8002B4D0();
        func_8002B9B8(0, soundID & 0xffff, 0xffffffff);
        func_8002B508();
    }
}
