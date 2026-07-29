#include "HOWL.h"

// OptionsMenu_TestSound @ 0x8002B208 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 byte-exact match against SCUS_944.26.exe (gcc 2.8.1-psx -O2, decomp-permuter verified).
//
// Match notes (all match-necessary; ~178 instructions, 5635 -> 0):
//  * The two duplicated "stop-old" dispatches on oldRow are written as `switch (oldRow)` with cases in
//    value order 0,1,2 — gcc emits retail's beq/slti comparison TREE and lays the case bodies out in
//    source order. (An if/else-if inlined the first case; extends the #65/#66 dispatch lever.)
//  * Block (2) is an explicit goto-dispatch (labels play_fx / music_start / stop_old / store_state) to
//    reproduce retail's branch-forward block layout; an if/else-if chain inlined the first case.
//  * music_start uses TWO cross-jumping CseqMusic_Start calls (songID 2 vs 1, identical otherwise): gcc
//    tail-merges the jal while duplicating the `sw newRow` stack-arg store into both ternary paths, as
//    retail does. The comparison is `== newRow` (register s1), not the constant 1, to match `bne v0,s1`.
//  * Block (3) uses TWO cross-jumping OtherFX_Play calls (soundID +0x1c vs +0x2c): this computes the
//    sound id directly into $a0 per branch and merges the jal, matching retail (a plain shared call left
//    the value in $v0 -> 555). The character lookup is duplicated per branch, as retail does.
//  * Frame-cadence divisions use `frameCount / 25` and `frameCount / 50` on a `uint` (retail's unsigned
//    magic-multiply by 0x51EB851F). g_optSlider_soundID stores the OtherFX_Play RETURN (Ghidra mis-typed
//    it as the character pointer).
//
// Options-screen sound preview state machine. newRow/newBoolPlay select which test sound is playing.
// (1) If a sound is currently playing (oldBoolPlay != 0) and the row changed, stop the old sound based
//     on oldRow (0=FX, 1=music, 2=voice). (2) If the selection changed, start the new sound (or stop,
//     when newBoolPlay==0) and store the new row/boolPlay. (3) When the voice row (2) is active, every
//     25th frame play the followed driver's character voice line (offset +0x1c on 50-frame ticks,
//     +0x2c on the odd 25-frame ticks) and remember its handle.
//
// Build note: oldBoolPlay (gp+0x828) / oldRow (gp+0x82C) / g_optSlider_soundID (gp+0x834) are
// GP-relative ints, DEFINED so gcc emits %gp_rel (GP_VALUE=0x8008CF6C). sdata_gGT (D_8008D2AC,
// GameTracker*) and g_awCharacterIDs (D_80086E84, u_short[]) are absolute. Callees: func_8002E550
// (Music_GetHighestSongPlayIndex), func_800291A0 (CseqMusic_Stop), func_80028844 (OtherFX_Stop2),
// func_80028808 (OtherFX_Stop1), func_80028468 (OtherFX_Play, returns the sound handle), func_80028C78
// (CseqMusic_Start). Verified vs in-repo decomp h69_OptionsMenu_TestSound.c (note: the binary stores
// the OtherFX_Play RETURN into g_optSlider_soundID, which Ghidra mis-typed as the character pointer).

extern int  func_8002E550(void);                     // Music_GetHighestSongPlayIndex
extern void func_800291A0(u_short songID);           // CseqMusic_Stop
extern void func_80028844(int soundID_count);        // OtherFX_Stop2
extern void func_80028808(int soundID_count);        // OtherFX_Stop1
extern int  func_80028468(u_short soundID, u_int antiSpamMode);                              // OtherFX_Play
extern uint func_80028C78(u_short songID, int tempo, void* songSet, int activeBits, int loop); // CseqMusic_Start

extern struct GameTracker* D_8008D2AC;   // sdata_gGT
extern u_short             D_80086E84[];  // g_awCharacterIDs

int D_8008D794 = 0;   // oldBoolPlay          (gp+0x828)
int D_8008D798 = 0;   // oldRow               (gp+0x82C)
int D_8008D7A0 = 0;   // g_optSlider_soundID  (gp+0x834) — last OtherFX_Play handle

void OptionsMenu_TestSound(int newRow, int newBoolPlay)
{
    // (1) Stop the currently-playing preview when switching rows.
    if (D_8008D794 != 0 && newRow != D_8008D798)
    {
        switch (D_8008D798)
        {
        case 0:
            func_80028844(0x48);
            break;
        case 1:
            if (func_8002E550() == 1)
                func_800291A0(2);
            else
                func_800291A0(1);
            break;
        case 2:
            if (D_8008D7A0 != 0)
            {
                func_80028808(D_8008D7A0);
                D_8008D7A0 = 0;
            }
            break;
        }
    }

    // (2) Start the new preview / update the stored state. goto-dispatch to match retail's
    //     branch-forward block layout (play, music-start, stop-dispatch, store).
    if (newBoolPlay != D_8008D794 || newRow != D_8008D798)
    {
        if (newBoolPlay == 0) goto stop_old;
        if (newRow == 0) goto play_fx;
        if (newRow == 1) goto music_start;
        goto store_state;
    play_fx:
        func_80028468(0x48, 0);
        goto store_state;
    music_start:
        if (func_8002E550() == newRow)
            func_80028C78(2, 0, (void*)0, 0, newRow);
        else
            func_80028C78(1, 0, (void*)0, 0, newRow);
        goto store_state;
    stop_old:
        switch (D_8008D798)
        {
        case 0:
            func_80028844(0x48);
            break;
        case 1:
            if (func_8002E550() == 1)
                func_800291A0(2);
            else
                func_800291A0(1);
            break;
        case 2:
            if (D_8008D7A0 != 0)
            {
                func_80028808(D_8008D7A0);
                D_8008D7A0 = 0;
            }
            break;
        }
    store_state:
        D_8008D794 = newBoolPlay;
        D_8008D798 = newRow;
    }

    // (3) Voice row: play the followed driver's line on the frame cadence.
    if (D_8008D794 != 0 && D_8008D798 == 2)
    {
        uint frameCount = D_8008D2AC->frameTimer_MainFrame_ResetDB;
        if (frameCount == (frameCount / 25) * 25)
        {
            if (frameCount == (frameCount / 50) * 50)
                D_8008D7A0 = func_80028468(D_80086E84[D_8008D2AC->cameraDC0_driverToFollow->driverID] + 0x1c, 0);
            else
                D_8008D7A0 = func_80028468(D_80086E84[D_8008D2AC->cameraDC0_driverToFollow->driverID] + 0x2c, 0);
        }
    }
}
