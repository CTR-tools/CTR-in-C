// MainInit_VisMem @ 0x8003af84  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Points gGT->visMem1 at the level's visibility data (level1->visMem @0x190) and,
// for each of numPlyrCurrGame players, zeroes visMem1->visLeafSrc/visFaceSrc/
// visOVertSrc/visSCVertSrc[i] (arrays @0x40/0x50/0x60/0x70). gGT is the a0 param
// (not gp-relative); asm reloads gGT->visMem1 (@0x1A38) before each store.
#include "CTR.h"

struct Level { char pad0[0x190]; void *visMem; };
struct VisMem {
    char pad0[0x40];
    void *visLeafSrc[4];    /* 0x40 */
    void *visFaceSrc[4];    /* 0x50 */
    void *visOVertSrc[4];   /* 0x60 */
    void *visSCVertSrc[4];  /* 0x70 */
};
struct GameTracker {
    char pad0[0x160];
    struct Level *level1;          /* 0x160 */
    char pad1[0x1A38 - 0x164];
    struct VisMem *visMem1;        /* 0x1A38 */
    char pad2[0x1CA8 - 0x1A3C];
    unsigned char numPlyrCurrGame; /* 0x1CA8 */
};

void MainInit_VisMem(struct GameTracker *gGT)
{
    struct VisMem *visMem = gGT->level1->visMem;
    gGT->visMem1 = visMem;

    if (visMem != 0 && gGT->numPlyrCurrGame != 0)
    {
        int i = 0;
        do
        {
            gGT->visMem1->visLeafSrc[i] = 0;
            gGT->visMem1->visFaceSrc[i] = 0;
            gGT->visMem1->visOVertSrc[i] = 0;
            gGT->visMem1->visSCVertSrc[i] = 0;
            i++;
        } while (i < gGT->numPlyrCurrGame);
    }
}
