// LOAD_FindFile @ 0x80032438  (vs SCUS_944.26.exe)
// SCORE 0. Returns whether a file exists on disc. NULL filename/cdlFile -> 0.
// SetMode, StringToUpper(filename), return (CdSearchFile(cdlFile, filename) != 0).
// Key: the null guard is a combined `filename==0 || cdlFile==0` (matches the
// retail inline return-0 block layout; saphi`s two separate ifs are equivalent).
#include "CTR.h"

struct CdlFILE { int pos; int size; char name[16]; };

extern void func_8001C470(void);                      /* CDSYS_SetMode_StreamData */
extern void func_80031C1C(char *);                    /* LOAD_StringToUpper */
extern int func_800722B8(struct CdlFILE *, char *);   /* CdSearchFile */

int LOAD_FindFile(char *filename, struct CdlFILE *cdlFile)
{
    if (filename == 0 || cdlFile == 0)
        return 0;

    func_8001C470();
    func_80031C1C(filename);

    return func_800722B8(cdlFile, filename) != 0;
}
