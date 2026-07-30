// CTR_unknownMaybeThunk2 @ 0x80021e1c  (vs SCUS_944.26.exe)
//
// BEHAVIOR-EXACT best-match (Branch Differences = 0, Stack Differences = 0).
// Residual score = 115 (Register Differences = 3): identical register-allocation
// wall to CTR_unknownMaybeThunk1 — gcc-2.8.1 folds the sign-extend into the
// counter register (`sra a2,v0,24`) where retail keeps it in v0 and copies
// (`sra v0,v0,24` + `addu a2,v0,zero`). Pure allocation artifact; everything
// else is instruction-identical.
//
// Same PackBits-style RLE expander as Thunk1 but OR-composites into the
// destination (`*out |= ...`) instead of overwriting — i.e. it merges a
// compressed bit-layer onto an existing buffer. dst=a0, src=a1, signed control
// byte c=*src: c==0 done; c<0 OR next byte (-c+1) times; c>0 OR the next c bytes.

#include "CTR.h"

void CTR_unknownMaybeThunk2(void *dst, void *src)
{
    u8 *out = (u8 *)dst;
    s8 *rle = (s8 *)src;

    while (*rle != 0)
    {
        int count = *rle;

        if (count < 0)
        {
            u8 value;
            count = -count + 1;
            rle++;
            value = (u8)*rle;
            rle++;
            while (count != 0)
            {
                *out++ |= value;
                count--;
            }
        }
        else
        {
            rle++;
            while (count != 0)
            {
                *out++ |= (u8)*rle++;
                count--;
            }
        }
    }
}
