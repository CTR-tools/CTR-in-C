// LOAD_Robots2P @ 0x80032700  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 1955, grind plateau ~1675 non-natural).
// Residual: whole-function reg-alloc wall — retail keeps TWO induction vars
// (robotSet pointer t1 += 4 for the append + byte-offset t2 += 4 for the inner
// base[4*i+j] index) and hoists both global base addresses to the top; gcc-2.8.1
// assigns the IV/base registers differently. Behavior identical.
// 2P robot-AI set picker: scan 7 sets of 4 AIs, pick the first whose 4 members
// none collide with p1/p2, copy them into characterIDs[2..5], queue BI_2PARCADEPACK+i.
#include "CTR.h"

extern short D_80086E84[];   // data.characterIDs (short[])
extern u8 D_80083A54[];      // data.characterIDs_2P_AIs (u8[])

extern void func_80032D30(void *bigfile, int type, int index, void *dest, void *callback);   // LOAD_AppendQueue

void LOAD_Robots2P(void *bigfile, int p1, int p2, void *callback)
{
    short *cids = D_80086E84;
    u8 *base = D_80083A54;
    u8 *robotSet;
    int i, j;
    int boolFoundRepeat;

    for (i = 0; i < 7; i++)
    {
        robotSet = &base[4 * i];
        boolFoundRepeat = 0;
        for (j = 0; j < 4; j++)
        {
            if (base[4 * i + j] == p1 || base[4 * i + j] == p2)
            {
                boolFoundRepeat = 1;
                break;
            }
        }
        if (!boolFoundRepeat)
            break;
    }

    if (i > 6)
        return;

    cids[2] = robotSet[0];
    cids[3] = robotSet[1];
    cids[4] = robotSet[2];
    cids[5] = robotSet[3];

    func_80032D30(bigfile, 2, 0x144 + i, 0, callback);
}
