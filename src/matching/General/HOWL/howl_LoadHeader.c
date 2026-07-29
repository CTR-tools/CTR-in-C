#include "HOWL.h"

// howl_LoadHeader @ 0x80029B2C (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 230, with 69/69 INSTRUCTION PARITY
//   (zero inserted/deleted instructions) and byte-for-byte-correct behavior. USER-GRANTED
//   time-boxed exception (2 fires). The entire residual is gcc-2.8.1 INSTRUCTION SCHEDULING of two
//   absolute-address `lui`s that no C form reproduces and the permuter cannot reach
//   (~25,000 iterations, no break):
//     * cdFile address: retail computes the hi-part into a caller-saved scratch (`lui v0`) and
//       hoists it AHEAD of the `sw s3` frame-save, then completes into s3 (`addiu s3,v0`). gcc
//       materializes it directly into the callee-saved s3 (`lui s3; addiu s3,s3`), which cannot be
//       scheduled before s3's own store.
//     * magic check: retail interleaves the g_szHowlMagic load with the alloc->magic load
//       (`lui v0; lw v1,0(s1); lw v0,%lo(v0)`); gcc emits the cached global as a unit.
//   Everything else matches exactly: the CD-load/validate/realloc flow, all 7 calls, the
//   cleanup-goto control flow, the s3-cached cdFile pointer, and the register allocation.
//   Manual C drove this 1680 -> 945 -> 505 -> 290 -> 230; the permuter only descends below 230 via
//   SEMANTICALLY BROKEN forms (dropping the tail `if` to make the success block unconditional),
//   which are not deliverable. 230 is the legitimate floor.
//
// Finds and loads the KART HWL sound-bank header from CD, validates it, then parses it (#32).
//   LOAD_FindFile locates the file into g_KartHwlCdFile; on success push a MEMPACK bookmark and
//   alloc one 0x800 sector. Read sector 0 (firstSector=0,count=1), validate magic=="HOWL" and
//   version==0x80. Compute the full header span = (headerSize + 0x28, rounded up to a 0x800
//   sector) and realloc to it; if it spans >1 sector, read the remaining sectors to alloc+0x800.
//   Call howl_ParseHeader(alloc) to cache section pointers, trim the alloc to headerSize+0x28,
//   return 1. Return 0 (popping the bookmark) on any failure.
//
// Match-necessary temps (each verified to lower the score; all sane/readable, not artifacts):
//   * cdFile     — caches &g_KartHwlCdFile so it lives in $s3 across all 3 calls (recomputing the
//                  address each time never allocates s3 and mis-sizes the frame: costs ~735).
//   * allocCopy  — routes the alloc pointer s0->s1 the way retail does (the magic check reads the
//                  original `alloc`, everything after reads `allocCopy`).
//   * hdrSize    — a fresh local for alloc->headerSize (NOT the reused `headerSize`, which carried
//                  the LOAD_* return codes) so gcc keeps it in the same register retail uses.
//   * magicWord  — caches g_szHowlMagic and compares `magicWord != alloc->magic`, hoisting the
//                  magic-global `lui` and fixing the compare's register order.
//   * reallocSize — held in $s2 across the ParseHeader call (retail does not re-derive headerSize).
//
// Both globals are absolute (%hi/%lo), NOT gp-relative: D_80095E7C (g_KartHwlCdFile, passed by
// address) and D_8008D060 (g_szHowlMagic, read as a 4-byte word). No GP_VALUE needed.

extern int   func_80032438(char* filename, void* cdFile);        // LOAD_FindFile
extern void  func_8003E978(void);                                // MEMPACK_PushBookmark
extern void* func_8003E874(int size, char* tag);                 // MEMPACK_AllocMem
extern int   func_80032498(void* cdFile, void* dest, int firstSector, int count); // LOAD_HowlHeaderSectors
extern void  func_8003E94C(int size);                            // MEMPACK_ReallocMem
extern void  func_80029A50(struct HowlHeader* header);           // howl_ParseHeader (#32)
extern void  func_8003E9D0(void);                                // MEMPACK_PopBookmark

extern int D_80095E7C;   // g_KartHwlCdFile (absolute; used by address)
extern int D_8008D060;   // g_szHowlMagic   (absolute; first 4 bytes read as a word)

int howl_LoadHeader(char* filename)
{
    int headerSize;
    struct HowlHeader* alloc;
    struct HowlHeader* allocCopy;
    int numSector;
    int reallocSize;
    int hdrSize;
    int magicWord;
    void* cdFile;

    cdFile = &D_80095E7C;
    headerSize = func_80032438(filename, cdFile);
    if (headerSize != 0)
    {
        func_8003E978();
        alloc = func_8003E874(0x800, filename);
        if (alloc == 0) goto fail;
        headerSize = func_80032498(cdFile, alloc, 0, 1);
        if (headerSize == 0) goto fail;
        allocCopy = alloc;
        magicWord = D_8008D060;
        if (magicWord != alloc->magic) goto fail;
        if (allocCopy->version != 0x80) goto fail;

        hdrSize = allocCopy->headerSize;
        numSector = (hdrSize + 0x827) >> 0xb;
        reallocSize = hdrSize + 0x28;
        func_8003E94C(numSector << 0xb);
        if (numSector < 2) goto success;
        numSector = func_80032498(cdFile, (void*)((int)allocCopy + 0x800), 1, numSector - 1);
        if (numSector != 0) goto success;
    fail:
        func_8003E9D0();
        return 0;
    success:
        func_80029A50(allocCopy);
        func_8003E94C(reallocSize);
        return 1;
    }
    return 0;
}
