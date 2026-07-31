// LOAD_HowlSectorChainEnd @ 0x8003266c  (vs SCUS_944.26.exe)
// SCORE 0. Called after each chained HOWL read: if howlChainState==-1 (error),
// restart via LOAD_HowlSectorChainStart with the saved howlChainParams and
// return 0; else return (howlChainState==0) (compiles to sltiu). gp+0x8D4..0x8E4.
// Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"

extern int func_80032594(void *, void *, int, int);   /* LOAD_HowlSectorChainStart */

int D_8008D840 = 0;   /* sdata->howlChainState (gp+0x8D4) */
int D_8008D844 = 0;   /* howlChainParams[0] (gp+0x8D8) */
int D_8008D848 = 0;   /* howlChainParams[1] */
int D_8008D84C = 0;   /* howlChainParams[2] */
int D_8008D850 = 0;   /* howlChainParams[3] */

int LOAD_HowlSectorChainEnd(void)
{
    int howlChainState = D_8008D840;

    if (howlChainState == -1)
    {
        func_80032594((void *)D_8008D844, (void *)D_8008D848, D_8008D84C, D_8008D850);
        return 0;
    }

    return howlChainState == 0;
}
