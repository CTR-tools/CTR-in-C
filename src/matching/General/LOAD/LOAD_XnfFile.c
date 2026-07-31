// LOAD_XnfFile @ 0x80032344  (vs SCUS_944.26.exe)
// SCORE 0. Loose disc-file reader (by filename, not bigfile index). StringToUpper,
// SetMode, CdSearchFile (fail -> return the caller ptrDestination); *size=cdlFile.size;
// if ptrDestination==NULL alloc (size+0x7ffU)&~0x7ff sectors then bail on NULL;
// CdControl(Setloc); CdRead (fail->0); CdReadSync (nonzero->0); ReallocMem(size) if
// we allocated (originalDst==NULL); return ptrDestination. Key: 0x7ffU (unsigned ->
// srl, not sra). Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"

struct CdlFILE { int pos; int size; char name[16]; };   /* size@4, 24 bytes */

extern void func_80031C1C(char *);                    /* LOAD_StringToUpper */
extern void func_8001C470(void);                      /* CDSYS_SetMode_StreamData */
extern int func_800722B8(struct CdlFILE *, char *);   /* CdSearchFile */
extern void *func_8003E874(int, char *);              /* MEMPACK_AllocMem */
extern int func_80071A24(int, struct CdlFILE *, void *); /* CdControl */
extern int func_80076F10(int, void *, int);           /* CdRead */
extern int func_800770AC(int, void *);                /* CdReadSync */
extern void func_8003E94C(int);                       /* MEMPACK_ReallocMem */

void *LOAD_XnfFile(char *filename, void *ptrDestination, int *size)
{
    struct CdlFILE cdlFile;
    char buf[8];
    void *originalDst = ptrDestination;

    func_80031C1C(filename);
    func_8001C470();

    if (func_800722B8(&cdlFile, filename) == 0)
        return ptrDestination;

    *size = cdlFile.size;

    if (ptrDestination == 0)
    {
        ptrDestination = func_8003E874((cdlFile.size + 0x7ffU) >> 0xb << 0xb, filename);
        if (ptrDestination == 0)
            return 0;
    }

    func_80071A24(2, &cdlFile, buf);

    if (func_80076F10((cdlFile.size + 0x7ffU) >> 0xb, ptrDestination, 0x80) == 0)
        return 0;
    if (func_800770AC(0, 0))
        return 0;

    if (originalDst == 0)
        func_8003E94C(cdlFile.size);

    return ptrDestination;
}
