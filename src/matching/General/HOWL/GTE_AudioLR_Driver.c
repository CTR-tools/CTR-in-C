#include "HOWL.h"

// GTE_AudioLR_Driver @ 0x8002E5CC (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Like GTE_AudioLR_Inst, but for a Driver: builds the world-space vector from the camera (matrix m's
// translation t[]) to the driver's position (d->posCurr >> 8), rotates it into camera space via
// SetRotMatrix(m)+gte_rtv0, and writes the rotated x/y/z into returnVect[0..2]. Verified vs Ghidra + h120.
// SetRotMatrix = func_800718FC. NOTE: posCurr >> 8 is a SIGNED (sra) shift (Ghidra's (uint) is an artifact).

extern void func_800718FC(MATRIX* m);   // SetRotMatrix

struct struct_Driver
{
    char _pad[0x2D4];
    struct { int x, y, z; } posCurr;   // 0x2D4
};

void GTE_AudioLR_Driver(MATRIX* m, struct struct_Driver* d, VECTOR* returnVect)
{
    SVECTOR local_10;

    local_10.vx = (short)(d->posCurr.x >> 8) - (short)m->t[0];
    local_10.vy = (short)(d->posCurr.y >> 8) - (short)m->t[1];
    local_10.vz = (short)(d->posCurr.z >> 8) - (short)m->t[2];
    func_800718FC(m);
    gte_ldv0(&local_10);
    gte_rtv0();
    gte_stlvnl0(&returnVect->vx);
    gte_stlvnl1(&returnVect->vy);
    gte_stlvnl2(&returnVect->vz);
}
