#include "HOWL.h"

// howl_InitGlobals @ 0x80029988 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 120. USER-GRANTED time-boxed
//   exception. The behavior is byte-for-byte correct and NEARLY byte-identical: all 10 GP-relative
//   global stores, all 10 function calls, and the dual-return are matched; the entire residual is
//   gcc-2.8.1 INSTRUCTION SCHEDULING that no C form reproduces:
//     * retail hoists the guard read `lbu 0xF0($gp)` to the very first instruction, ahead of the
//       stack-frame setup; gcc emits the frame setup first.
//     * retail loads the constant 1 (`li v1,1`) as the first instruction inside the if-body and
//       holds it in a saved register for the two `=1` stores. `one = 1` before the guard (kept
//       here) drops the residual 615->120 but loads it one instruction too early (before the lbu);
//       moving it inside the if makes gcc load it lazily (back to 615).
//   The permuter only scores below 120 (110) via a non-retail register reuse (sharing the `1`
//   between a store and the SpuSetCommonCDMix(1) argument), which is not retail's arrangement and
//   is not deliverable. `one` is kept because it is the closest legitimate form.
//
// One-time init of the howl audio engine (skips if already initialized). Sets default volumes and
// flags, songLoadStage=3, marks initialized; then SsUtReverbOff / SpuInit-family / master volume /
// reverb mode / CD reverb-mix-vol / voiceline pool, and loads the howl header. Returns
// (howl_LoadHeader() != 0), or 0 if already initialized.
//
// Build note: all globals are GP-relative (gp=0x8008CF6C); DEFINED (not extern) so gcc emits
// %gp_rel (GP_VALUE=0x8008CF6C). patch_gprel derives each gp offset from its D_<addr> name.

u_char D_8008D05C = 0;   // g_bHowlInitialized     (gp+0xF0)
int    D_8008D794 = 0;   // (gp+0x828)
int    D_8008D798 = 0;   // (gp+0x82C)
u_char D_8008D79F = 0;   // g_bBoolStoringVolume   (gp+0x833)
int    D_8008D7A0 = 0;   // (gp+0x834)
u_char D_8008D7A4 = 0;   // g_nSongLoadStage       (gp+0x838)
u_char D_8008D7AC = 0;   // g_bVolEngineFX         (gp+0x840)
u_char D_8008D7B8 = 0;   // g_bVolMusic            (gp+0x84C)
u_char D_8008D7BC = 0;   // g_bVolOtherFX          (gp+0x850)
u_char D_8008D7CC = 0;   // g_bVolumeMode          (gp+0x860)

extern void func_80076870(void);        // SsUtReverbOff
extern void func_8007193C(int);         // SpuSetTransferMode
extern void func_800767E0(int, int);    // SpuSetCommonMasterVolume
extern void func_80028BBC(int);         // Howl_SetReverbMode (= SetReverbMode, #11)
extern void func_80076800(int);         // SpuSetCommonCDReverb
extern void func_800765D0(int);         // SpuSetCommonCDMix
extern void func_80072C98(int, int);    // SpuSetCommonCDVolume
extern void func_8002C918(void);        // Voiceline_PoolInit
extern void func_8002D2B0(void);        // Voiceline_SetDefaults
extern int  func_80029B2C(char*);       // howl_LoadHeader

int howl_InitGlobals(char* filename)
{
    int result;
    int one;

    one = 1;
    if (D_8008D05C == 0)
    {
        D_8008D7AC = 0xd7;
        D_8008D7B8 = 0xaf;
        D_8008D7BC = 0xff;
        D_8008D794 = 0;
        D_8008D798 = 0;
        D_8008D7A0 = 0;
        D_8008D7CC = one;
        D_8008D05C = one;
        D_8008D79F = 0;
        D_8008D7A4 = 3;
        func_80076870();
        func_8007193C(0);
        func_800767E0(0x3fff, 0x3fff);
        func_80028BBC(5);
        func_80076800(0);
        func_800765D0(1);
        func_80072C98(0, 0);
        func_8002C918();
        func_8002D2B0();
        result = (func_80029B2C(filename) != 0);
    }
    else
    {
        result = 0;
    }
    return result;
}
