// CTR_MatrixToRot @ 0x80021edc  (vs SCUS_944.26.exe)
//
// BEHAVIOR-EXACT best-match (Branch Differences = 0, Stack Differences = 0).
// Residual score ~9145 (Register Differences = 109; a -j8 grind only reached
// ~8930): a whole-function gcc-2.8.1-vs-retail register-ALLOCATION wall on a
// large (214-insn) high-pressure routine. The retail spills `matrix`, the
// bit-2 flag and the bit-0 flag to their stack home slots and RELOADS `matrix`
// from 0x4C(sp) at every use, whereas this gcc keeps `matrix` in a callee-saved
// register (fp/s8); it also materializes the constant 1 for the `==1` test and
// REUSES it as the shift amount for the x2 index math (`sllv ...,a0` vs `sll
// ...,1`). These are allocation/scheduling artifacts, not behavioral: control
// flow, memory accesses, arithmetic and all call arguments are identical. No
// natural source change forces the retail's spill pattern without a reuse hack,
// so this is delivered as the clean, verified-correct reference source.
//
// Extracts an Euler rotation SVECTOR from a rotation-free instance MATRIX
// (e.g. an AngleAxis matrix, as for a fired bowling bomb). `flags` selects the
// axis permutation via two char index tables at 0x8008d004 / 0x8008d00c:
//   b0 = swap the X/Z result axes
//   b1 = which matrix element layout (transposed pick) to read from
//   b2 = negate the whole result vector
// Uses MATH_FastSqrt (0x8003d214) for the gimbal magnitude and ratan2
// (0x8007173c) for each angle; rot->pad receives the raw flags at the end.
//
// The reference decomp for this function is
// src/decompile/General/CTR/CTR_17_MatrixToRot.c; saphi's CTR_Matrix.c is the
// same logic (verified here) plus an intentional #ifdef CTR_NATIVE that mirrors
// the 0x8008d004 table through sdata.

#include "CTR.h"

typedef struct { short vx, vy, vz, pad; } SVECTOR;
typedef struct { short m[3][3]; long t[3]; } MATRIX;

extern char D_8008D004[];   // axis index table 1 (0x8008d004)
extern char D_8008D00C[];   // axis index table 2 (0x8008d00c)

extern int func_8003D214(int, int);   // MATH_FastSqrt(value, 0x18)
extern int func_8007173C(int, int);   // ratan2(y, x)

void CTR_MatrixToRot(SVECTOR *rot, MATRIX *matrix, u_int flags)
{
    int iVar2;
    int iVar3;
    short *psVar5;
    short *psVar11;

    u_int uVar7 = flags >> 2 & 1;

    char *table1 = D_8008D004;
    char *table2 = D_8008D00C;

    u_int t1value = (u_int)table1[flags >> 3 & 3];

    u_int t2value1 = (u_int)table2[t1value + uVar7];
    u_int t2value2 = (u_int)table2[t1value - (uVar7 - 1)];

    if ((flags >> 1 & 1) == 1)
    {
        psVar5 = matrix->m[t1value] + t2value1;
        iVar2 = (int)*psVar5;

        psVar11 = matrix->m[t1value] + t2value2;
        iVar3 = (int)*psVar11;

        iVar2 = func_8003D214(iVar2 * iVar2 + iVar3 * iVar3, 0x18) >> 12;

        if (iVar2 < 0x11)
        {
            rot->vx = (short)func_8007173C(-(int)matrix->m[t2value1][t2value2], (int)*(short *)((int)matrix->m + t2value1 * 8));
            rot->vy = (short)func_8007173C(iVar2, (long)*(short *)((int)matrix->m + t1value * 8));
            rot->vz = (short)0;
        }
        else
        {
            rot->vx = (short)func_8007173C((int)*psVar5, (int)*psVar11);
            rot->vy = (short)func_8007173C(iVar2, (int)*(short *)((int)matrix->m + t1value * 8));
            rot->vz = (short)func_8007173C((int)matrix->m[t2value1][t1value], -(int)matrix->m[t2value2][t1value]);
        }
    }
    else
    {
        psVar5 = (short *)((int)matrix->m + t1value * 8);
        iVar2 = (int)*psVar5;

        psVar11 = matrix->m[t2value1] + t1value;
        iVar3 = (int)*psVar11;

        iVar2 = func_8003D214(iVar2 * iVar2 + iVar3 * iVar3, 0x18) >> 12;

        if (iVar2 < 0x11)
        {
            rot->vx = (short)func_8007173C(-(int)matrix->m[t2value1][t2value2], (int)*(short *)((int)matrix->m + t2value1 * 8));
            rot->vy = (short)func_8007173C(-(int)matrix->m[t2value2][t1value], iVar2);
            rot->vz = (short)0;
        }
        else
        {
            rot->vx = (short)func_8007173C((int)matrix->m[t2value2][t2value1], (int)*(short *)((int)matrix->m + t2value2 * 8));
            rot->vy = (short)func_8007173C(-(int)matrix->m[t2value2][t1value], iVar2);
            rot->vz = (short)func_8007173C((int)*psVar11, (int)*psVar5);
        }
    }

    if (uVar7 == 1)
    {
        rot->vx = -rot->vx;
        rot->vz = -rot->vz;
        rot->vy = -rot->vy;
    }

    if ((flags & 1) == 1)
    {
        rot->pad = rot->vx;
        rot->vx = rot->vz;
        rot->vz = rot->pad;
    }

    rot->pad = (short)flags;
}
