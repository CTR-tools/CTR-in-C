// LOAD_ReadFile (LOAD_ReadFile_ex) @ 0x800321b4  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 4915, grind floor 4715). Residual: a
// whole-function gcc-2.8.1-vs-retail register-ALLOCATION wall on this 100-insn
// retry-loop reader (retail spills/reloads the params across the CD calls more
// heavily; mine is 90 insns vs retail 100). Core file reader: BIG_GETENTRY
// (bigfile+8), size@+4/offset@+0; *sizePtr=eSize; CdIntToPos; flags|=LT_SETADDR
// & AllocMem sectorSize if ptrDst==0 (else flags&=~LT_SETADDR); retry loop
// {CdControl; set callbackCdReadSuccess + CdReadCallback; CdRead; if !callback
// CdReadSync==0} until control&read ok; ReallocMem(size) if sync+alloced.
// Note: pycparser needs void* (not fn-ptr) params. Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"
struct BigEntry { int offset; int size; };
struct BigHeader { int cdpos; int numEntry; struct BigEntry entries[1]; };   /* entries@+8 */
struct LoadQueueSlot { char pad0[0x4]; u16 flags; };
extern void func_8001C470(void);
extern void func_80071E64(int, void *);
extern void *func_8003E874(int, char *);
extern int func_80071A24(int, void *, void *);
extern void func_800771B0(void *);
extern int func_80076F10(int, void *, int);
extern int func_800770AC(int, void *);
extern void func_8003E94C(int);
extern void func_80032110(void);
extern struct LoadQueueSlot D_80083A3C;
extern char D_8008D0AC[];
void *D_8008D85C = 0;
void *LOAD_ReadFile(struct BigHeader *bigfile, u_int loadType, int subfileIndex, void *ptrDst, int *sizePtr, void *callback)
{
    int uVar5;
    char cdLoc[8];
    char paramOutput[8];
    struct LoadQueueSlot *lqs = &D_80083A3C;
    int eSize = bigfile->entries[subfileIndex].size;
    int sectorCount = (eSize + 0x7ff) >> 0xb;
    int readComplete = 1;
    void *originalDst = ptrDst;
    *sizePtr = eSize;
    func_80071E64(bigfile->cdpos + bigfile->entries[subfileIndex].offset, cdLoc);
    if (ptrDst == 0)
    {
        lqs->flags |= 0x1;
        ptrDst = func_8003E874(sectorCount << 0xb, D_8008D0AC);
        if (ptrDst == 0)
            return 0;
    }
    else
    {
        lqs->flags &= ~0x1;
    }
    while (1)
    {
        uVar5 = func_80071A24(2, cdLoc, paramOutput);
        if (callback != 0)
        {
            D_8008D85C = callback;
            func_800771B0(func_80032110);
        }
        else
        {
            D_8008D85C = 0;
            func_800771B0(0);
        }
        uVar5 &= func_80076F10(sectorCount, ptrDst, 0x80);
        if (callback == 0)
            readComplete = func_800770AC(0, 0) == 0;
        if (uVar5 != 0 && readComplete != 0)
            break;
    }
    if (callback == 0 && originalDst == 0)
        func_8003E94C(*sizePtr);
    return ptrDst;
}
