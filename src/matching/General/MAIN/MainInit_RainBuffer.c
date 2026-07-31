// MainInit_RainBuffer @ 0x8003b008  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 485). Residual: walk-pointer register
// allocation/addressing (retail keeps pPlayerBase=gGT walking +0x30 with 0x1A40
// offset; gcc-2.8.1 bases it differently). Grind floor 115 was a BROKEN non-natural
// mutation (set pPlayerBase constant, ignoring i) — natural form shipped per no-hacks.
// Per-player rain-buffer init: for each of numPlyrCurrGame players, copy the level
// rain template (level1->rainBuffer @+0x104, 0x30 bytes) into gGT->rainBuffer[i]
// (stride 0x30 @+0x1A40), then divide numParticles_curr (int) and numParticles_max
// (u16) by numPlyrCurrGame to split rain density across split-screen viewports.
#include "CTR.h"

struct RainBuffer {
    int numParticles_curr;    /* 0x0 */
    unsigned short numParticles_max;  /* 0x4 */
    unsigned short vanishRate;        /* 0x6 */
    char unk_8[0x30 - 8];
};

struct Level {
    char pad_104[0x104];
    struct RainBuffer rainBuffer;   /* 0x104 */
};

struct GameTracker {
    char pad_160[0x160];
    struct Level *level1;           /* 0x160 */
    char pad_1A40[0x1A40 - 0x164];
    struct RainBuffer rainBuffer;   /* 0x1A40 */
    char pad_1CA8[0x1CA8 - (0x1A40 + 0x30)];
    unsigned char numPlyrCurrGame;  /* 0x1CA8 */
};

void MainInit_RainBuffer(struct GameTracker *gGT)
{
    int i = 0;
    struct GameTracker *pPlayerBase = gGT;

    if (gGT->numPlyrCurrGame != 0)
    {
        do
        {
            pPlayerBase->rainBuffer = gGT->level1->rainBuffer;
            pPlayerBase->rainBuffer.numParticles_curr = pPlayerBase->rainBuffer.numParticles_curr / gGT->numPlyrCurrGame;
            pPlayerBase->rainBuffer.numParticles_max = pPlayerBase->rainBuffer.numParticles_max / gGT->numPlyrCurrGame;
            i++;
            pPlayerBase = (struct GameTracker *)((char *)pPlayerBase + 0x30);
        } while (i < gGT->numPlyrCurrGame);
    }
}
