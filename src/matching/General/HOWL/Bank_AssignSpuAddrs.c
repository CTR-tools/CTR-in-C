#include "HOWL.h"

// Bank_AssignSpuAddrs @ 0x800293B8 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. This readable form scores 1755 (the permuter reached 1490
//   with throwaway reordering artifacts, no path to 0). USER-GRANTED time-boxed exception after a
//   dedicated multi-fire permuter grind plateaued. The ENTIRE remaining diff is confined to the two
//   stage-1 loops; every other part of the function is BYTE-PERFECT: the 5-way stage dispatch, all
//   guard chains, stages 2 and 3, and the shared epilogue.
//
//   Residual cause = a gcc-2.8.1 loop code-generation choice we can't drive from C:
//     * loop 1 (the size-sum) — gcc strength-reduces the `block1[i]` access into a second
//       induction variable (a moving pointer + init moves + a post-loop `addiu -2` fixup); retail
//       keeps a single induction variable `i` and recomputes `i*2` (indexed addressing). Loop 2 IS
//       indexed and structurally matches, because its aliasing `sae` store forces a block1 reload
//       each iteration that blocks strength reduction; loop 1 has no such store, so gcc reduces it.
//       Tried: `block1[i]`, `block1[i+1]` post-increment, explicit `*(u16*)((int)block1 + i*2)`
//       cast, and caching block1 in a local — all leave the strength reduction in place.
//     * loop 2 — only register-name differences, perturbed by loop 1's extra induction register.
//
//   The dispatch match came from writing the state machine as `switch(stage){case 0/1/2/3}` (gcc's
//   switch comparison-tree, incl. the `slti` range split, matches retail where if/else-if did not)
//   and inverting the pre-loop-2 bankFlags if/else to match retail's branch polarity.
//
// Non-blocking 5-stage SPU sound-bank loader; caller polls until it returns true (stage 4).
//   stage 0: start reading the bank's 1-sector sample-block header into block2 -> stage 1.
//   stage 1: once loaded, sum allocSize from each sample's size*8; record max into the Bank
//            (or bail to stage 4 if it won't fit), realloc MEMPACK, stream the data sectors into
//            block2+0x800, assign each new sample's SPU address from the running allocPtr/Bank.min
//            -> stage 2.
//   stage 2: once loaded, SpuSetTransferStartAddr + SpuRead to SPU RAM (if within 0x7e000) -> 3.
//   stage 3: once the SPU transfer completes, advance allocPtr, mark Bank loaded (flags|2),
//            MEMPACK_PopBookmark -> stage 4.
//   stage 4: done.
//
// Build note: gp-relative globals (gp=0x8008CF6C) are DEFINED (not extern) so gcc emits %gp_rel
// (GP_VALUE=0x8008CF6C). D_8008D7DC and D_80095E7C are absolute (%hi/%lo). allocSize is uint
// (srl shifts + unsigned bounds compares).

extern u_short* D_8008D7DC;      // g_pHowlSampleAddrTable (ushort[] pairs {spuAddr, spuSize})
extern char     D_80095E7C[];    // g_KartHwlCdFile (passed by address to the LOAD funcs)

u_char   D_8008D774 = 0;   // g_nBankLoadStage    (gp+0x808, u8)
uint     D_8008D764 = 0;   // g_nAudioAllocSize   (gp+0x7F8, uint)
int      D_8008D768 = 0;   // g_nNumAudioSectors  (gp+0x7FC, int)
int      D_8008D770 = 0;   // g_nAudioAllocPtr    (gp+0x804, int)
int      D_8008D778 = 0;   // g_nBankSectorOffset (gp+0x80C, int)
int      D_8008D77C = 0;   // g_nBankFlags        (gp+0x810, int)
int      D_8008D780 = 0;   // g_pLastBank         (gp+0x814, int)  raw-offset Bank ptr
u_short* D_8008D784 = 0;   // g_pSampleBlock1     (gp+0x818, u16*) loaded header [0]=count,[1..]=ids
void*    D_8008D788 = 0;   // g_pSampleBlock2     (gp+0x81C, void*)

extern int  func_80032594(void*, void*, int, int);  // LOAD_HowlSectorChainStart(cd, dst, sect, n)
extern int  func_8003266C(void);                    // LOAD_HowlSectorChainEnd
extern void func_8003E94C(int);                     // MEMPACK_ReallocMem(size)
extern void func_800762B0(uint);                    // SpuSetTransferStartAddr(addr)
extern void func_80076310(void*, int);              // SpuRead(dst, size)
extern int  func_80076370(int);                     // SpuIsTransferCompleted(mode)
extern void func_8003E9D0(void);                    // MEMPACK_PopBookmark

int Bank_AssignSpuAddrs(void)
{
    int i;
    int transferDone;
    int stage;
    u_short* sae;
    uint addr;
    u_short spuAddr;
    u_short* sampleTable;
    u_short* hdr;

    if (D_8008D774 == 4)
    {
        return 1;
    }

    stage = D_8008D774;
    switch (stage)
    {
    case 0:
        if (func_80032594(D_80095E7C, D_8008D788, D_8008D778, 1) != 0)
        {
            D_8008D774 = 1;
        }
        break;
    case 1:
        if (func_8003266C() != 0)
        {
            D_8008D764 = 0;
            i = 0;
            hdr = D_8008D784;
            if (*hdr != 0)
            {
                do
                {
                    i = i + 1;
                    D_8008D764 = D_8008D764 +
                        (uint)D_8008D7DC[(uint)hdr[i] * 2 + 1] * 8;
                } while (i < (int)(uint)*hdr);
            }
            if (D_8008D77C == 0)
            {
                *(short*)(D_8008D780 + 6) = (short)(D_8008D764 >> 3);
            }
            else if (*(u_short*)(D_8008D780 + 6) < D_8008D764)
            {
                D_8008D774 = 4;
                return 1;
            }
            D_8008D768 = (D_8008D764 + 0x7ff) >> 0xb;
            func_8003E94C(D_8008D768 * 0x800 + 0x800);
            i = func_80032594(D_80095E7C, (void*)((int)D_8008D788 + 0x800), D_8008D778 + 1, D_8008D768);
            if (i == 0)
            {
                return 0;
            }
            if (D_8008D77C != 0)
            {
                spuAddr = *(u_short*)(D_8008D780 + 4);
            }
            else
            {
                spuAddr = D_8008D770;
                *(u_short*)(D_8008D780 + 4) = D_8008D770;
            }
            sampleTable = D_8008D7DC;
            i = 0;
            if (*D_8008D784 != 0)
            {
                do
                {
                    sae = sampleTable + (uint)D_8008D784[i + 1] * 2;
                    if (*sae == 0)
                    {
                        *sae = spuAddr;
                    }
                    i = i + 1;
                    spuAddr = spuAddr + sae[1];
                } while (i < (int)(uint)*D_8008D784);
            }
            D_8008D774 = 2;
        }
        break;
    case 2:
        if (func_8003266C() != 0)
        {
            addr = (uint)*(u_short*)(D_8008D780 + 4) * 8;
            if (addr + D_8008D764 < 0x7e000)
            {
                func_800762B0(addr);
                func_80076310((void*)((int)D_8008D788 + 0x800), D_8008D764);
            }
            D_8008D774 = 3;
        }
        break;
    case 3:
        transferDone = func_80076370(0);
        if (transferDone != 0)
        {
            if (D_8008D77C == 0)
            {
                D_8008D770 = D_8008D770 + (D_8008D764 >> 3);
            }
            *(u_short*)(D_8008D780 + 2) = *(u_short*)(D_8008D780 + 2) | 2;
            func_8003E9D0();
            D_8008D774 = 4;
        }
        break;
    }
    return D_8008D774 == 4;
}
