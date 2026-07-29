// SCORE 0 — byte-perfect match vs SCUS_944.26.exe (926/NTSC-U), func @ 0x80028844.
// OtherFX_Stop2 — stops ALL playing instances of a sound effect: masks the handle to
// the 16-bit id and calls Channel_SearchFX_Destroy(1, soundID&0xffff, 0xffff) (match
// mask 0xffff = id only) inside a Smart critical section. Counterpart to OtherFX_Stop1
// (one instance, full soundID+count handle, mask 0xffffffff).

#include "HOWL.h"

extern void func_8002B4D0(void);            // Smart_EnterCriticalSection
extern void func_8002B9B8(int, int, uint);  // Channel_SearchFX_Destroy
extern void func_8002B508(void);            // Smart_ExitCriticalSection

void OtherFX_Stop2(int soundID_count)
{
    func_8002B4D0();
    func_8002B9B8(1, soundID_count & 0xffff, 0xffff);
    func_8002B508();
}
