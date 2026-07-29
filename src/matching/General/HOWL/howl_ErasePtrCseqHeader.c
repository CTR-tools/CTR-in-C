#include "HOWL.h"

// howl_ErasePtrCseqHeader @ 0x80029DC0 (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 MATCH against SCUS_944.26.exe (decomp-permuter base score = 0).
//
// Clears the loaded CSEQ header pointer (g_pCseqHeader = NULL), marking no song data resident.
//
// Build note: g_pCseqHeader is GP-relative (gp=0x8008CF6C); DEFINED (not extern) so gcc emits
// %gp_rel (GP_VALUE=0x8008CF6C). patch_gprel derives the gp offset from the D_<addr> name.

int D_8008D7B0 = 0;   // g_pCseqHeader (gp+0x844)

void howl_ErasePtrCseqHeader(void)
{
    D_8008D7B0 = 0;
}
