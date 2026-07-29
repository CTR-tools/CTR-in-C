#include "HOWL.h"

// Cutscene_VolumeRestore @ 0x8002C1D0 (SCUS_944.26.exe, 926/NTSC-U).
//
// Score-0 match against SCUS_944.26.exe (matched on first compile).
//
// Restores the FX volume backed up by Cutscene_VolumeBackup. Under a critical section, clears
// boolStoringVolume (u_char @gp+0x833) and re-applies the saved volume via howl_VolumeSet(0, currentVolume).
// NOTE: the binary reads currentVolume (u_char @gp+0x832, lbu) for the arg, not the decomp's storedVolume.
//
// GP-rel globals (GP_VALUE=0x8008CF6C; DEFINED so gcc emits %gp_rel, patch_gprel resolves from D_<addr>).
// Callees: func_8002B4D0 Smart_EnterCriticalSection(#70), func_8002B130 howl_VolumeSet(#66),
// func_8002B508 Smart_ExitCriticalSection(#71).

extern void func_8002B4D0(void);          // Smart_EnterCriticalSection
extern void func_8002B130(int, u_char);   // howl_VolumeSet
extern void func_8002B508(void);          // Smart_ExitCriticalSection

u_char D_8008D79E = 0;   // currentVolume     (gp+0x832)
u_char D_8008D79F = 0;   // boolStoringVolume (gp+0x833)

void Cutscene_VolumeRestore(void)
{
    func_8002B4D0();
    D_8008D79F = 0;
    func_8002B130(0, D_8008D79E);
    func_8002B508();
}
