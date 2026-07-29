#include "HOWL.h"

// howl_SetSong @ 0x80029C40 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 10, with 25/25 INSTRUCTION PARITY
//   (zero inserted/deleted instructions) and byte-for-byte-correct behavior. USER-GRANTED
//   exception. The entire residual is a SINGLE gcc-2.8.1 register-allocation tie-break: the
//   sectorOffset `lhu`/`sw` uses v0 here vs v1 in retail. Retail loads the halfword into v1 (reusing
//   the just-freed g_pHowlSongOffsets register) so v0 stays free for the `li v0,1` return value;
//   gcc instead reuses v0 (the address register) for the load. Every C form tried loads into v0
//   (routing the load through the return var `ret` = 10; a separate temp = 120; operand-swap = 10;
//   a local pointer + index = 170), so v1 is unreachable from source — it is an artifact of the
//   original compile's allocation order. Everything else matches exactly: the inverted-polarity
//   branch (`beqz v0, <stage3>`), the dual `li v0,1` return in each path, all four gp-relative
//   globals, and the howl_ErasePtrCseqHeader call.
//
// Selects a song for loading. If howl is initialized: erase the current CSEQ header, reset
// songLoadStage=0, and record the song's CD sector offset from g_pHowlSongOffsets[songID].
// If not initialized: just set songLoadStage=3. Always returns 1.
//
// Match-necessary form:
//   * `if (initialized != 0) {...} else {stage=3}` — inverted from the reference `if (init==0)` so
//     the initialized block is inline and stage=3 is out-of-line, matching retail's `beqz v0,...`.
//   * `uint ret` assigned 1 in EACH branch (return ret) — reproduces retail's dual `li v0,1`; a
//     plain `return 1` in each branch makes gcc flip the whole block layout (score 600).
//   * `D_8008D7A8 = (ret = *(u_short*)(...))` — routes the load through the return register, which
//     removes an otherwise-needed `move v0,v1` (this is the 120 -> 10 step).
//
// Build note: all globals are GP-relative (gp=0x8008CF6C); DEFINED (not extern) so gcc emits
// %gp_rel (GP_VALUE=0x8008CF6C). patch_gprel derives each gp offset from its D_<addr> name.

extern void func_80029DC0(void);   // howl_ErasePtrCseqHeader

u_char D_8008D05C = 0;   // g_bHowlInitialized   (gp+0xF0)
u_char D_8008D7A4 = 0;   // g_nSongLoadStage     (gp+0x838)
int    D_8008D7A8 = 0;   // g_nSongSectorOffset  (gp+0x83C)
int    D_8008D7E0 = 0;   // g_pHowlSongOffsets   (gp+0x874)

uint howl_SetSong(uint songID)
{
    uint ret;

    if (D_8008D05C != 0)
    {
        func_80029DC0();
        D_8008D7A4 = 0;
        D_8008D7A8 = (ret = *(u_short*)((songID & 0xffff) * 2 + D_8008D7E0));
        ret = 1;
    }
    else
    {
        D_8008D7A4 = 3;
        ret = 1;
    }
    return ret;
}
