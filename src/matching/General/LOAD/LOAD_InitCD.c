// LOAD_InitCD @ 0x80031c58  (vs SCUS_944.26.exe)
// SCORE 0. Initializes the CD subsystem: CDSYS_Init(1) (func_8001C360).
// (saphi has an intentional #ifdef CTR_NATIVE branch for the no-disc native port.)
#include "CTR.h"

extern void func_8001C360(int);   // CDSYS_Init

void LOAD_InitCD(void)
{
    func_8001C360(1);
}
