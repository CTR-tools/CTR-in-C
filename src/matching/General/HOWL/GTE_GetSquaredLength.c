#include "HOWL.h"

// GTE_GetSquaredLength @ 0x8002E658 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Squared length of a 3-vector: loads vx/vy/vz into IR1/IR2/IR3, gte_sqr0 (SQR), returns
// MAC1+MAC2+MAC3 = vx*vx+vy*vy+vz*vz. Companion to GTE_GetSquaredDistance. Used by PlaySound3D.
// Verified vs Ghidra + h121. Leaf GTE fn; components at short-indices 0/2/4 (byte 0/4/8), lwc2-loaded.

int GTE_GetSquaredLength(short* vec3)
{
    short* p1;
    short* p2;
    int mac1, mac2, mac3;

    gte_ldIR1(vec3);
    p1 = vec3 + 2;
    gte_ldIR2(p1);
    p2 = vec3 + 4;
    gte_ldIR3(p2);
    gte_sqr0();
    gte_stMAC1(mac1);
    gte_stMAC2(mac2);
    gte_stMAC3(mac3);
    return mac1 + mac2 + mac3;
}
