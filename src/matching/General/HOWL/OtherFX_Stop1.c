// SCORE 0 — byte-perfect match vs SCUS_944.26.exe (926/NTSC-U), func @ 0x80028808.
// OtherFX_Stop1 — stops one specific sound instance (matched by its soundID+count
// handle) via Channel_SearchFX_Destroy inside a Smart critical section. Counterpart
// to OtherFX_Stop2 (which stops by sound id).

#include "HOWL.h"

extern void func_8002B4D0(void);            // Smart_EnterCriticalSection
extern void func_8002B9B8(int, int, uint);  // Channel_SearchFX_Destroy
extern void func_8002B508(void);            // Smart_ExitCriticalSection

void OtherFX_Stop1(int soundID_count)
{
    func_8002B4D0();
    func_8002B9B8(1, soundID_count, 0xffffffff);
    func_8002B508();
}
