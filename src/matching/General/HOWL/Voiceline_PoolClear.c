#include "HOWL.h"

// Voiceline_PoolClear @ 0x8002CAE0 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best score = 160 (0 register diffs, 1 reordering, 1 insertion),
//   byte-BEHAVIOR-correct. AUTO-GRANTED (2026-07-28) per the <300 scheduling-plateau policy. Everything
//   matches EXCEPT one delay-slot fill: for the second LIST_Clear (on Voiceline2 @D_800961B4) retail places
//   the `addiu $a0,%lo(D_800961B4)` in the jal's delay slot (`lui $a0,%hi; jal; addiu $a0,%lo`), whereas
//   this compiler build completes the address before the jal and emits a nop in the delay slot. It is a
//   gcc/maspsx delay-slot-scheduling choice, not expressible in C — the permuter floors at 160 over ~300
//   iterations. The flag-clear order was matched by writing them 0x881/0x882/0x880 (they are independent);
//   the direct-address form (no v1Head local) is worse (975) because two absolute `&D_800961A4` expressions
//   don't CSE the way the decomp's shared sdata base did, so the local captures that shared address.
//
// Resets the voiceline subsystem: clears the wrong-way-SFX / can-play-voicelines flags and the voiceline
// cooldown, clears both voiceline lists and rebuilds the free list from the 8-entry voiceline pool, then
// clears the timestamp arrays (Voiceline_ClearTimeStamp, #91).
//
// GP-rel DEFINED (GP_VALUE=0x8008CF6C): boolCanPlayWrongWaySFX=D_8008D7EC(0x880,u8),
// boolCanPlayVoicelines=D_8008D7ED(0x881,u8), voicelineCooldown=D_8008D7EE(0x882,short). ABS extern:
// Voiceline1=D_800961A4, Voiceline2=D_800961B4 (list heads), voicelinePool=D_800961C4 (8 x 0x10). Callees:
// func_80031734 LIST_Clear, func_8003197C LIST_Init, func_8002CAA8 Voiceline_ClearTimeStamp(#91).

extern void func_80031734(void* list);                                   // LIST_Clear
extern void func_8003197C(void* list, void* array, int stride, int count); // LIST_Init
extern void func_8002CAA8(void);                                         // Voiceline_ClearTimeStamp (#91)

extern void* D_800961A4;   // Voiceline1 list head
extern void* D_800961B4;   // Voiceline2 list head
extern int   D_800961C4[]; // voicelinePool (8 x 0x10)

u_char D_8008D7EC = 0;   // boolCanPlayVoicelines  (gp+0x880)  [name corrected per #94]
u_char D_8008D7ED = 0;   // boolCanPlayWrongWaySFX (gp+0x881)  [name corrected per #94]
short  D_8008D7EE = 0;   // voicelineCooldown      (gp+0x882)

void Voiceline_PoolClear(void)
{
    void* v1Head = &D_800961A4;

    D_8008D7ED = 0;
    D_8008D7EE = 0;
    D_8008D7EC = 0;

    func_80031734(v1Head);
    func_80031734(&D_800961B4);
    func_8003197C(v1Head, &D_800961C4[0], 0x10, 8);
    func_8002CAA8();
}
