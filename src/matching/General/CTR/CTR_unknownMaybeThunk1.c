// CTR_unknownMaybeThunk1 @ 0x80021da0  (vs SCUS_944.26.exe)
//
// BEHAVIOR-EXACT best-match (Branch Differences = 0, Stack Differences = 0).
// Residual score = 115 (Register Differences = 3): the ONLY divergence is which
// register receives the sign-extended control byte. gcc-2.8.1 here folds the
// sign-extend and the move into the loop-counter register into one insn
// (`sra v1,v0,24`), whereas retail keeps the value in v0 (`sra v0,v0,24`) and
// emits a separate `addu v1,v0,zero` copy for the literal-run counter. That
// extra copy is a pure register-allocation artifact; no natural source change
// forces it without a reuse/temp hack. Everything else (control flow, loads,
// stores, arithmetic, both loop bodies) is instruction-identical.
//
// PackBits-style RLE expander: dst=a0, src=a1. Signed control byte c = *src:
//   c == 0  -> done
//   c  < 0  -> repeat next byte (-c + 1) times            (fill run)
//   c  > 0  -> copy the next c bytes verbatim             (literal run)
// The control byte is read twice (lb for the !=0 test, lbu then sll/sra to
// materialize the signed value) — reproduced by `while(*rle!=0){ count=*rle; }`.

#include "CTR.h"

void CTR_unknownMaybeThunk1(void *dst, void *src)
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
                *out++ = value;
                count--;
            }
        }
        else
        {
            rle++;
            while (count != 0)
            {
                *out++ = (u8)*rle++;
                count--;
            }
        }
    }
}
