#include "HOWL.h"

// howl_LoadSong @ 0x80029CA4 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 80, with 71/71 INSTRUCTION PARITY
//   (zero inserted/deleted instructions) and byte-for-byte-correct behavior. USER-GRANTED
//   time-boxed exception (3 fires). The entire residual is a SINGLE gcc-2.8.1 branch-delay-slot
//   scheduling tie-break: in the stage-0 dispatch (`beqz $v1, <stage0>`), retail hoists cdFile's
//   `lui $a0, %hi(g_KartHwlCdFile)` (arg-1's address hi-part) into the delay slot and completes it
//   with `addiu $a0` in the handler; gcc instead hoists block1's `lui $a1, %hi(g_abHwlSampleBlock1)`
//   (arg-2's hi-part). Both are equivalent; which %hi the scheduler picks for the one delay slot is
//   an artifact of the original compile. The permuter cracked 3125 -> 215 -> 80 but plateaued at 80
//   (thousands of iterations); manual scheduling nudges (cdFile temp / offset temp in case 0) also
//   could not flip it. Everything else matches exactly: the switch dispatch tree, the case-body
//   order, the block1 base materialization, all 3 calls, the 3 direct stage re-reads, and the
//   `return (stage == 3)`.
//
// Non-blocking 4-stage CSEQ-song streaming loader; poll until it returns true (done). Dispatch on
// g_nSongLoadStage:
//   0 -> kick off reading the 1st sector (song header) into g_abHwlSampleBlock1
//        (LOAD_HowlSectorChainStart @ g_nSongSectorOffset) -> stage 1
//   1 -> once that chain completes (LOAD_HowlSectorChainEnd), read the remaining
//        ((songSize+0x7ff)/0x800 - 1) sectors into g_abHwlSampleBlocks (block1+0x800) -> stage 2
//        (songSize = first int of g_abHwlSampleBlock1)
//   2 -> once that completes, howl_ParseCseqHeader(g_abHwlSampleBlock1) -> stage 3
//   3 -> loaded. Returns (stage == 3).
//
// Match-necessary form:
//   * `if (stage == 3) return 1;` then `switch (stage) { case 0/1/2 }` (insight: switch reproduces
//     retail's dispatch tree `beq==1 / slti<2 / beqz==0 / beq==2`; nested if/else-if mis-lays-out).
//   * cases in numeric order 0,1,2 so the case bodies lay out to match.
//   * `blocksOff = 0x800` temp so `D_80090584 + blocksOff` is NOT folded into the address constant,
//     matching retail's block1-base-in-a-register form (this is the 215 -> 80 step).
//   * g_nSongLoadStage read directly each time (3 lbu reads) to reproduce retail's re-reads; songSize
//     read via `*(uint*)` so the sector-count shift is `srl` (unsigned), not `sra`.
//
// Build note: the stage/offset globals are GP-relative (gp=0x8008CF6C); DEFINED (not extern) so gcc
// emits %gp_rel (GP_VALUE=0x8008CF6C). g_KartHwlCdFile (D_80095E7C, by address) and
// g_abHwlSampleBlock1 (D_80090584) are absolute (%hi/%lo), left extern.

extern int  func_8003266C(void);                                 // LOAD_HowlSectorChainEnd
extern int  func_80032594(void* cdFile, void* dest, int offset, int count); // LOAD_HowlSectorChainStart
extern void func_80029AB4(void* header);                         // howl_ParseCseqHeader (#33)

extern int    D_80095E7C;      // g_KartHwlCdFile    (absolute; used by address)
extern u_char D_80090584[];    // g_abHwlSampleBlock1 (absolute; 0x800-byte song buffer)

u_char D_8008D7A4 = 0;   // g_nSongLoadStage    (gp+0x838)
int    D_8008D7A8 = 0;   // g_nSongSectorOffset (gp+0x83C)

int howl_LoadSong(void)
{
    int done;
    int started;
    int blocksOff;

    if (D_8008D7A4 == 3)
    {
        return 1;
    }
    switch (D_8008D7A4)
    {
    case 0:
        if ((started = func_80032594(&D_80095E7C, D_80090584, D_8008D7A8, 1), started != 0))
        {
            D_8008D7A4 = 1;
        }
        break;
    case 1:
        blocksOff = 0x800;
        done = func_8003266C();
        if ((done != 0) &&
            (started = func_80032594(&D_80095E7C, D_80090584 + blocksOff, D_8008D7A8 + 1,
                                     ((*(uint*)D_80090584 + 0x7ff) >> 0xb) - 1), started != 0))
        {
            D_8008D7A4 = 2;
        }
        break;
    case 2:
        if ((done = func_8003266C(), done != 0))
        {
            func_80029AB4(D_80090584);
            D_8008D7A4 = 3;
        }
        break;
    }
    return (D_8008D7A4 == 3);
}
