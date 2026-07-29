#include "HOWL.h"

// GTE_AudioLR_Inst @ 0x8002E55C (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH against SCUS_944.26.exe.
// Rotates the (world-space) sound->camera direction vector returnVect into camera space using the
// camera's rotation matrix (SetRotMatrix(pushBuffer) + gte_rtv0), writing the rotated x/y/z back into
// returnVect[0..2]. Used by PlaySound3D to derive the stereo balance angle. Verified vs Ghidra + h119.
// SetRotMatrix = func_800718FC (in-binary library fn, real jal). VECTOR/SVECTOR/MATRIX + GTE macros
// are shared from HOWL.h.

extern void func_800718FC(MATRIX* m);   // SetRotMatrix

void GTE_AudioLR_Inst(MATRIX* pushBuffer, VECTOR* returnVect)
{
    SVECTOR local_10;

    local_10.vx = (short)returnVect->vx;
    local_10.vy = (short)returnVect->vy;
    local_10.vz = (short)returnVect->vz;
    func_800718FC(pushBuffer);
    gte_ldv0(&local_10);
    gte_rtv0();
    gte_stlvnl0(&returnVect->vx);
    gte_stlvnl1(&returnVect->vy);
    gte_stlvnl2(&returnVect->vz);
}
