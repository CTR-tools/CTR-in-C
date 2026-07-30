// CTR_EmptyFunc_MainFrame_ResetDB @ 0x80021c8c  (vs SCUS_944.26.exe)
//
// SCORE 0 exact match. Empty function: the retail body is just `jr $ra; nop`
// (an 8-byte no-op stub, likely a stubbed-out per-frame DB reset hook).

#include "CTR.h"

void CTR_EmptyFunc_MainFrame_ResetDB(void)
{
}
