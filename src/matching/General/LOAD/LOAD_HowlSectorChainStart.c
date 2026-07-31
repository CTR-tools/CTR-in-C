// LOAD_HowlSectorChainStart @ 0x80032594  (vs SCUS_944.26.exe)
// SCORE 0. Kicks off a chained HOWL data read. numSector==0 -> 1. Backs up the
// 4 params to sdata->howlChainParams (gp+0x8D8..0x8E4), SetMode, out-of-bounds
// guard (sizeOver>=0x800 -> 0), CdControl(Setloc) at CdPosToInt(pos)+firstSector,
// howlChainState=1 (gp+0x8D4), CdReadCallback(LOAD_HowlCallback), return CdRead!=0.
// Key: the 4 params are individual gp-relative globals (a gp array would compile
// absolute). NOTE: in-repo decomp #if0`d the bounds check; retail (and saphi) keep it.
// Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"

struct CdlFILE { int pos; int size; char name[16]; };

extern void func_8001C470(void);                      /* CDSYS_SetMode_StreamData */
extern int func_80071F68(void *);                     /* CdPosToInt */
extern void func_80071E64(int, void *);               /* CdIntToPos */
extern int func_80071A24(int, void *, void *);        /* CdControl */
extern void func_800771B0(void *);                    /* CdReadCallback */
extern int func_80076F10(int, void *, int);           /* CdRead */
extern void func_8003254C(void);                      /* LOAD_HowlCallback */

int D_8008D840 = 0;   /* sdata->howlChainState (gp+0x8D4) */
int D_8008D844 = 0;   /* howlChainParams[0] (gp+0x8D8) */
int D_8008D848 = 0;   /* howlChainParams[1] */
int D_8008D84C = 0;   /* howlChainParams[2] */
int D_8008D850 = 0;   /* howlChainParams[3] */

int LOAD_HowlSectorChainStart(struct CdlFILE *cdlFileHWL, void *ptrDestination, int firstSector, int numSector)
{
    char loc[8];
    char buf[8];
    int sizeOver;

    if (numSector == 0)
        return 1;

    D_8008D844 = (int)cdlFileHWL;
    D_8008D848 = (int)ptrDestination;
    D_8008D84C = firstSector;
    D_8008D850 = numSector;

    func_8001C470();

    sizeOver = (firstSector + numSector) * 0x800 - cdlFileHWL->size;
    if (sizeOver >= 0x800)
        return 0;

    func_80071E64(func_80071F68(cdlFileHWL) + firstSector, loc);
    func_80071A24(2, loc, buf);

    D_8008D840 = 1;

    func_800771B0(func_8003254C);
    return func_80076F10(numSector, ptrDestination, 0x80) != 0;
}
