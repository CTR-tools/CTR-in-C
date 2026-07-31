// LOAD_Callback_Podiums @ 0x80031a64  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 225, grind 195). Clear load_inProgress;
// data.podiumModel_podiumStands (abs D_80083A38) = lqs->ptrDestination. Residual:
// maspsx emits the absolute store as `lui at; sw` + nop-filled jr delay slot,
// where retail keeps the addr in a reg and fills the delay slot with the store.
// Score with GP_VALUE=0x8008CF6C.
#include "CTR.h"
struct LoadQueueSlot { char pad0[0xC]; void *ptrDestination; };
extern void *D_80083A38;   // data.podiumModel_podiumStands (absolute)
int D_8008D0A4 = 0;
void LOAD_Callback_Podiums(struct LoadQueueSlot *lqs)
{
    D_8008D0A4 = 0;
    D_80083A38 = lqs->ptrDestination;
}
