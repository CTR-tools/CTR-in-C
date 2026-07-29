#include "HOWL.h"

// howl_StopAudio @ 0x8002C8A8 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH (perfect).
//
// Stops audio: if bool_DestroyMusic, CseqMusic_StopAll(); then, inside a critical section,
// Channel_DestroyAll_LowLevel(bool_DestroyAllSFX, bool_DestroyMusic==0, 2) — the middle arg keeps music
// alive when DestroyMusic is false (retail emits it as `sltiu a1, bool_DestroyMusic, 1`); type 2 = music.
// If clearBackupStats, clears g_nNumPausedChannels.
//
// GP-rel DEFINED (GP_VALUE=0x8008CF6C): g_nNumPausedChannels=D_8008D790(0x824,int). Callees: func_80029258
// CseqMusic_StopAll(#20), func_8002B4D0 Smart_EnterCriticalSection, func_8002BA90 Channel_DestroyAll_LowLevel,
// func_8002B508 Smart_ExitCriticalSection.

extern void func_80029258(void);                                // CseqMusic_StopAll (#20)
extern void func_8002B4D0(void);                                // Smart_EnterCriticalSection
extern void func_8002BA90(int destroyAllSFX, int keepMusic, int type);  // Channel_DestroyAll_LowLevel
extern void func_8002B508(void);                                // Smart_ExitCriticalSection

int D_8008D790 = 0;   // g_nNumPausedChannels (gp+0x824)

void howl_StopAudio(int clearBackupStats, int bool_DestroyMusic, int bool_DestroyAllSFX)
{
    if (bool_DestroyMusic != 0)
        func_80029258();
    func_8002B4D0();
    func_8002BA90(bool_DestroyAllSFX, bool_DestroyMusic == 0, 2);
    func_8002B508();
    if (clearBackupStats != 0)
        D_8008D790 = 0;
}
