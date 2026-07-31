// LOAD_ReadDirectory @ 0x80031c78  (vs SCUS_944.26.exe)
// SCORE 0. Reads a bigfile directory header from disc: SetMode, CdSearchFile
// (return-checked), AllocMem(0x4000), CdControl(Setloc), CdRead (checked),
// CdReadSync (checked), store bh->cdpos=CdPosToInt, ReallocMem to
// 8+8*numEntry, sdata->ptrBigfileCdPos_2=bh (gp+0x130). Key match: alias
// pCdlFile=cdlFile so gcc caches &cdlFile in s1 across the calls. NOTE: the
// in-repo decomp omits the return checks AND does AllocMem before CdSearchFile;
// the retail (and saphi) do neither. Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"

struct BigHeader { int cdpos; int numEntry; };

extern void func_8001C470(void);                        // CDSYS_SetMode_StreamData
extern int func_800722B8(void *, char *);               // CdSearchFile
extern struct BigHeader *func_8003E874(int, char *);    // MEMPACK_AllocMem
extern void func_80071A24(int, void *, void *);         // CdControl
extern int func_80076F10(int, void *, int);             // CdRead
extern int func_800770AC(int, int);                     // CdReadSync
extern int func_80071F68(void *);                       // CdPosToInt
extern void func_8003E94C(int);                         // MEMPACK_ReallocMem

struct BigHeader *D_8008D09C = 0;   // sdata->ptrBigfileCdPos_2 (gp+0x130)

struct BigHeader *LOAD_ReadDirectory(char *filename)
{
    char cdlFile[24];
    char buf[8];
    char *pCdlFile;
    struct BigHeader *bh;

    func_8001C470();
    pCdlFile = cdlFile;
    if (func_800722B8(pCdlFile, filename) == 0)
        return 0;
    bh = func_8003E874(0x4000, filename);
    func_80071A24(2, cdlFile, buf);
    if (func_80076F10(8, bh, 0x80) == 0)
        return 0;
    if (func_800770AC(0, 0) != 0)
        return 0;
    bh->cdpos = func_80071F68(cdlFile);
    func_8003E94C(bh->numEntry * 8 + 8);
    D_8008D09C = bh;
    return bh;
}
