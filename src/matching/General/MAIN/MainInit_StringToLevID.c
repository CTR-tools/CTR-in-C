// MainInit_StringToLevID @ 0x8003c1d4  (vs SCUS_944.26.exe)  [syms: "unused"]
// BEHAVIOR-EXACT (Branch/Stack 0; base 115, Reg 3; grind floor 115).
// Residual: gcc folds +4 into the cursor init (s0=base+4, read 0(s0)) where retail
// keeps s0=entry base and reads 4(s0) — a strength-reduction addressing choice.
// Linear scan of data.metaDataLEV (0x41 entries, stride 0x18): returns the first
// levelID whose name_Debug (@+4) matches str via strncmp(name,str,strlen(name)); 0 if none.
#include "CTR.h"

struct MetaLev { char pad0[4]; char *name_Debug; char pad8[0x18 - 8]; };
extern struct MetaLev D_80083A80[];   /* data.metaDataLEV (stride 0x18) */

extern int func_80077CC8(char *s);                    /* strlen  */
extern int func_800769A0(char *a, char *b, int n);    /* strncmp */

int MainInit_StringToLevID(char *str)
{
    struct MetaLev *m = D_80083A80;
    int levelID;

    for (levelID = 0; levelID < 0x41; levelID++, m++)
    {
        if (func_800769A0(m->name_Debug, str, func_80077CC8(m->name_Debug)) == 0)
            return levelID;
    }
    return 0;
}
