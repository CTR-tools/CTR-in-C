// LOAD_HowlHeaderSectors @ 0x80032498  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 400, grind floor). Residual: gcc computes
// the final `return 1` (after `if(CdReadSync!=0) return 0`) as `sltiu v0,v0,1`
// (== CdReadSync==0), where retail emits a plain `li v0,1` in the delay slot.
// Loads N sectors of a HOWL bank header from disc: SetMode; out-of-bounds guard
// ((firstSector+numSector)*0x800 - cdlFile->size >= 0x800 -> 0); CdControl(Setloc)
// at CdPosToInt(pos)+firstSector; CdRead(numSector) (fail->0); CdReadSync(nonzero->0);
// return 1. NOTE: the in-repo decomp #if0`d the bounds check; retail (and saphi) keep it.
#include "CTR.h"

struct CdlFILE { int pos; int size; char name[16]; };   /* pos@0, size@4 */

extern void func_8001C470(void);                      /* CDSYS_SetMode_StreamData */
extern int func_80071F68(void *);                     /* CdPosToInt */
extern void func_80071E64(int, void *);               /* CdIntToPos */
extern int func_80071A24(int, void *, void *);        /* CdControl */
extern int func_80076F10(int, void *, int);           /* CdRead */
extern int func_800770AC(int, void *);                /* CdReadSync */

int LOAD_HowlHeaderSectors(struct CdlFILE *cdlFileHWL, void *ptrDestination, int firstSector, int numSector)
{
    char loc[8];
    char buf[8];
    int sizeOver;

    func_8001C470();

    sizeOver = (firstSector + numSector) * 0x800 - cdlFileHWL->size;
    if (sizeOver >= 0x800)
        return 0;

    func_80071E64(func_80071F68(cdlFileHWL) + firstSector, loc);
    func_80071A24(2, loc, buf);

    if (func_80076F10(numSector, ptrDestination, 0x80) == 0)
        return 0;
    if (func_800770AC(0, 0) != 0)
        return 0;

    return 1;
}
