#include "CTR.h"

// CTR_CycleTex_LEV @ 0x80021984 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH.
// Walks the AnimTex list (variable-size entries, terminator = an entry whose ptrActiveTex==head);
// per entry: frameCurr = ((timer + frameOffset) >> frameSkip) % numFrames (signed), store @0xA, set
// ptrActiveTex = ptrArray[frameCurr], advance to &ptrArray[numFrames]. KEY: the array index is s16
// ((short)frameCurr) -> sll16/sra14. NB: retail uses RAW timer (the decomp FPS_HALF is a decomp artifact).

void CTR_CycleTex_LEV(struct AnimTex *animtex, int timer)
{
    struct AnimTex *cur = animtex;

    while (*(int *)cur != (int)animtex)
    {
        int frameCurr = (timer + cur->frameOffset) >> cur->frameSkip;
        frameCurr = frameCurr % cur->numFrames;
        cur->frameCurr = frameCurr;
        cur->ptrActiveTex = cur->ptrArray[(short)frameCurr];
        cur = (struct AnimTex *)&cur->ptrArray[cur->numFrames];
    }
}
