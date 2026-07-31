// LOAD_LangFile @ 0x80032b50  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 235, grind plateau 225 non-natural).
// Residual: retail loads numStrings/strArray into $v0/$v1 then copies to $t0/$a3
// for the loop; gcc-2.8.1 loads them straight into the loop-safe regs (2 fewer
// moves). Behavior identical.
// Loads the language file: alloc lngFile buffer (once), ReadFile_ex it, then
// relocate the string-pointer array by adding the buffer base to each entry.
// Match levers: sdata->lngFile is gp-relative (init `= 0` so it lands in
// small-data), langBufferSize/numLngStrings/lngStrings use ABSOLUTE addressing,
// and the loop index i is a `short` (retail sign-extends via sll16/sra14).
#include "CTR.h"

struct LngFile { int numStrings; int offsetToPtrArr; };

extern void *func_8003E874(int size, void *name);                                             /* MEMPACK_AllocMem  */
extern void *func_800321B4(void *bigfile, int type, int index, void *dest, int *sizePtr, void *cb); /* LOAD_ReadFile_ex */

extern int   D_8008CF6C;    /* sdata->langBufferSize (absolute) */
extern char  D_80011174[];  /* "lang buffer" */
void  *D_8008D858 = 0;      /* sdata->lngFile (gp+0x8EC) */
extern int   D_8008D110;    /* sdata->numLngStrings (absolute) */
extern char **D_8008D878;   /* sdata->lngStrings (absolute) */

void LOAD_LangFile(void *bigfilePtr, int lang)
{
    struct LngFile *lngFile;
    int size;
    short i;
    int numStrings;
    char **strArray;

    if (D_8008D858 == 0)
        D_8008D858 = func_8003E874(D_8008CF6C, D_80011174);

    lngFile = func_800321B4(bigfilePtr, 1, lang + 0xEA, D_8008D858, &size, 0);
    if (lngFile == 0)
        return;

    numStrings = lngFile->numStrings;
    strArray = (char **)((char *)lngFile + lngFile->offsetToPtrArr);

    D_8008D110 = numStrings;
    D_8008D878 = strArray;

    for (i = 0; i < numStrings; i++)
        strArray[i] = (char *)((int)strArray[i] + (int)lngFile);
}
