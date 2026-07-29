#include "HOWL.h"

// howl_ParseHeader @ 0x80029A50 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 MATCH against SCUS_944.26.exe (decomp-permuter base score = 0).
//
// Given the loaded HWL sound-bank header in memory, derives and caches all section base pointers
// (byte-address accumulation from the header, sizeof HowlHeader = 0x28):
//   sampleAddrTable = header + 0x28;      metaOtherFX = + numSpuAddrs*4;
//   metaEngineFX    = + numOtherFX*8;     bankOffsets = + numEngineFX*8;
//   songOffsets     = + numBanks*2;       end         = + numSequences*2.
//
// Match notes:
//   * The header-pointer store (D_8008D7C0 = howlHeader) must be the FIRST statement to reproduce
//     retail's schedule (gcc keeps howlHeader live in $a0 and emits `sw $a0,0x854(gp)` mid-body).
//   * `sampleAddrTable` is a match-necessary temp: splitting `p = howlHeader + 0x28` into its own
//     register (retail's `addiu $v0,$a0,0x28`) is what drives the winning register allocation.
//     Inlining it (p = howlHeader + 0x28 directly) regresses the score to 1225.
//
// Build note: the 7 cached pointers are GP-relative (gp=0x8008CF6C); DEFINED (not extern) so gcc
// emits %gp_rel (GP_VALUE=0x8008CF6C). Stored as int (byte addresses); could be typed later.

int D_8008D7C0 = 0;   // g_pHowlHeader          (gp+0x854)
int D_8008D7D0 = 0;   // g_pHowlMetaEngineFX    (gp+0x864)
int D_8008D7D4 = 0;   // g_pHowlEnd             (gp+0x868)
int D_8008D7D8 = 0;   // g_pHowlMetaOtherFX     (gp+0x86C)
int D_8008D7DC = 0;   // g_pHowlSampleAddrTable (gp+0x870)
int D_8008D7E0 = 0;   // g_pHowlSongOffsets     (gp+0x874)
int D_8008D7E4 = 0;   // g_pHowlBankOffsets     (gp+0x878)

void howl_ParseHeader(struct HowlHeader* howlHeader)
{
    int p;
    int sampleAddrTable;

    D_8008D7C0 = (int)howlHeader;
    sampleAddrTable = (int)howlHeader + 0x28;
    p = sampleAddrTable;
    D_8008D7DC = p;
    p = p + howlHeader->numSpuAddrs * 4;
    D_8008D7D8 = p;
    p = p + howlHeader->numOtherFX * 8;
    D_8008D7D0 = p;
    p = p + howlHeader->numEngineFX * 8;
    D_8008D7E4 = p;
    p = p + howlHeader->numBanks * 2;
    D_8008D7E0 = p;
    p = p + howlHeader->numSequences * 2;
    D_8008D7D4 = p;
}
