#include "CTR.h"

// CTR_CycleTex_Model @ 0x80021A20 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH.
// Like CTR_CycleTex_LEV but ptrActiveTex is a pointer-to-pointer: writes the frame texture THROUGH
// it (*ptrActiveTex = ptrArray[frameCurr]) instead of storing into the field. dst read before the
// frameCurr store; s16 array index; raw timer. Same signed-modulo frame calc + variable-stride walk.

void CTR_CycleTex_Model(struct AnimTex *animtex, int timer)
{
    struct AnimTex *cur = animtex;

    while (*(int *)cur != (int)animtex)
    {
        int frameCurr;
        void **dst;

        frameCurr = (timer + cur->frameOffset) >> cur->frameSkip;
        frameCurr = frameCurr % cur->numFrames;
        dst = (void **)cur->ptrActiveTex;
        cur->frameCurr = frameCurr;
        *dst = cur->ptrArray[(short)frameCurr];
        cur = (struct AnimTex *)&cur->ptrArray[cur->numFrames];
    }
}
