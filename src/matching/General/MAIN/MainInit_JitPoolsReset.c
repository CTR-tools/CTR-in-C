// MainInit_JitPoolsReset @ 0x8003b2d4  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Clears all 8 JIT allocation pools (JitPool_Clear) at gGT+0x18D0 stride 0x28:
// thread, instance, smallStack, mediumStack, largeStack, particle, oscillator, rain.
#include "CTR.h"

struct JitPool { char pad[0x28]; };
struct GameTracker { char pad0[0x18D0]; struct JitPool JitPools[8]; };   /* 0x18D0, stride 0x28 */

extern void func_80030FDC(struct JitPool *pool);   /* JitPool_Clear */

void MainInit_JitPoolsReset(struct GameTracker *gGT)
{
    func_80030FDC(&gGT->JitPools[0]);   /* 0x18D0 thread      */
    func_80030FDC(&gGT->JitPools[1]);   /* 0x18F8 instance    */
    func_80030FDC(&gGT->JitPools[2]);   /* 0x1920 smallStack  */
    func_80030FDC(&gGT->JitPools[3]);   /* 0x1948 mediumStack */
    func_80030FDC(&gGT->JitPools[4]);   /* 0x1970 largeStack  */
    func_80030FDC(&gGT->JitPools[5]);   /* 0x1998 particle    */
    func_80030FDC(&gGT->JitPools[6]);   /* 0x19C0 oscillator  */
    func_80030FDC(&gGT->JitPools[7]);   /* 0x19E8 rain        */
}
