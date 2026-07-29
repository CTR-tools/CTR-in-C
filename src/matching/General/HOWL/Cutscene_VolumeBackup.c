#include "HOWL.h"

// Cutscene_VolumeBackup @ 0x8002C18C (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best score = 20, byte-BEHAVIOR-correct; the entire residual is a
//   pure gcc-2.8.1 register-allocation tie-break (4 register-name diffs, no instruction/scheduling
//   differences). AUTO-GRANTED (2026-07-27) per the <300 regalloc-plateau policy. Retail emits
//   `andi $v1,$v0,0xff` (masked value -> $v1) then `li $v0,1` (the constant reuses vol's now-dead $v0);
//   gcc here always emits `andi $v0,$v0,0xff` (masked reuses vol's $v0) + `li $v1,1`. This is
//   deterministically unforceable: gcc's scheduler always moves the andi after the currentVolume store,
//   so vol dies at the andi and the masked value reuses $v0. Verified over 7+ source forms (self-assign
//   is best at 20; separate-local / masked-computed-early both regress to 215 by hoisting the constant;
//   reusing vol for the constant via `vol=1` regresses to 425/625 by deferring the currentVolume store)
//   and 4 permuter windows (up to 116 iters) that all plateau at 20 with no sane OR broken sub-20 form.
//   The BIG fix that got here (415->20): howl_VolumeGet is declared to return `short`, so `int vol=...`
//   sign-extended (sll/sra, +200); the prototype below returns `int` because retail uses the raw return
//   (the sb/andi discard the upper bits), which drops the sign-extension.
//
// Backs up the current FX volume at the start of a cutscene. Under a critical section, reads
// howl_VolumeGet(0) and stores it (byte-masked) into currentVolume (u_char @gp+0x832) and storedVolume
// (u_short @gp+0x830), then sets boolStoringVolume (u_char @gp+0x833) = 1.
//
// GP-rel globals (GP_VALUE=0x8008CF6C; DEFINED so gcc emits %gp_rel, patch_gprel resolves from D_<addr>).
// Callees: func_8002B4D0 Smart_EnterCriticalSection(#70), func_8002B0E0 howl_VolumeGet(#65),
// func_8002B508 Smart_ExitCriticalSection(#71).

extern void  func_8002B4D0(void);   // Smart_EnterCriticalSection
extern int   func_8002B0E0(int);    // howl_VolumeGet (return used raw — no sign-extend)
extern void  func_8002B508(void);   // Smart_ExitCriticalSection

u_short D_8008D79C = 0;   // storedVolume      (gp+0x830)
u_char  D_8008D79E = 0;   // currentVolume     (gp+0x832)
u_char  D_8008D79F = 0;   // boolStoringVolume (gp+0x833)

void Cutscene_VolumeBackup(void)
{
    int vol;

    func_8002B4D0();
    vol = func_8002B0E0(0);
    D_8008D79E = vol;
    vol = vol & 0xff;
    D_8008D79F = 1;
    D_8008D79C = vol;
    func_8002B508();
}
