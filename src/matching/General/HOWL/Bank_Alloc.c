#include "HOWL.h"

// Bank_Alloc @ 0x800292FC (SCUS_944.26.exe, 926/NTSC-U).
// SCORE-0 match against SCUS_944.26.exe (926/NTSC-U).
//
// Prepares to load one SPU sample bank: records its flags (bit0 of *(ptrBank+2)), its CD sector
// offset (g_pHowlBankOffsets[bankID]), and the bank pointer; pushes a MEMPACK bookmark and
// allocates a 0x800-byte temp RAM block for the sample-block parse, then sets loadStage=0 to
// kick off the async load. Returns 1 (also when audio disabled, with loadStage=4), or 0 if the
// RAM alloc fails (popping the bookmark).
//
// MATCH NOTES:
//  - Direct `return 1;`/`return 0;` at each exit (not a `result` variable) so gcc keeps the
//    return value literally in v0 and jumps to the shared epilogue (the #12 dual-return pattern).
//  - The flags assignment is an explicit if/else (not `x = (expr) != 0`) so gcc emits retail's
//    branch-then-store-1/store-0 with the bankID mask scheduled into the branch tails.
//
// Build note: gp-relative globals (gp=0x8008CF6C) are DEFINED (not extern) so gcc emits %gp_rel;
// patch_gprel resolves each from its D_<addr> name (GP_VALUE=0x8008CF6C). D_8008D7E4 and
// D_800110CC are absolute (%hi/%lo).

extern u_char   D_8008D05C;      // boolAudioEnabled
extern u_short* D_8008D7E4;      // g_pHowlBankOffsets (ptr to ushort[])
extern char     D_800110CC[];    // alloc debug tag (2nd arg to MEMPACK_AllocMem)

u_char D_8008D774 = 0;   // g_nBankLoadStage    (gp+0x808, sb)
int    D_8008D77C = 0;   // g_nBankFlags        (gp+0x810, sw)
int    D_8008D778 = 0;   // g_nBankSectorOffset (gp+0x80C, sw)
int    D_8008D780 = 0;   // g_pLastBank         (gp+0x814, sw)
void*  D_8008D788 = 0;   // g_pSampleBlock2     (gp+0x81C, sw)
void*  D_8008D784 = 0;   // g_pSampleBlock1     (gp+0x818, sw)

extern void  func_8003E978(void);           // MEMPACK_PushBookmark
extern void* func_8003E874(int, char*);     // MEMPACK_AllocMem(size, tag)
extern void  func_8003E9D0(void);           // MEMPACK_PopBookmark

uint Bank_Alloc(int bankID, int ptrBank)
{
    if (D_8008D05C == 0)
    {
        D_8008D774 = 4;
        return 1;
    }

    if ((*(u_short*)(ptrBank + 2) & 1) != 0)
    {
        D_8008D77C = 1;
    }
    else
    {
        D_8008D77C = 0;
    }
    D_8008D778 = D_8008D7E4[bankID & 0xffff];
    D_8008D780 = ptrBank;
    func_8003E978();
    D_8008D788 = func_8003E874(0x800, D_800110CC);
    if (D_8008D788 == 0)
    {
        func_8003E9D0();
        return 0;
    }
    D_8008D784 = D_8008D788;
    D_8008D774 = 0;
    return 1;
}
