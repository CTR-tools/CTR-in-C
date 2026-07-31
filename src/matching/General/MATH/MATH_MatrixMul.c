/*
 * MATH_MatrixMul @ 0x8003d460 (SCUS_944.26, NTSC-U 926)
 *
 * Full affine (rigid-body) matrix compose: out = m2 . m3 (apply m3, then m2).
 *   Rotation:    out.rot = m2.rot * m3.rot   (via MATH_MatrixMultiplication, rotation-only 3x3)
 *   Translation: out.t   = m2.rot * m3.t + m2.t
 *                          (MatrixVecMultiply rotates m3.t into out.t, then m2.t is added in)
 * Distinct from the rotation-only MATH_MatrixMultiplication (func_8006C3B0) it calls.
 *
 * Match: score 0 (Branch 0 / Stack 0 / Register 0) vs retail.
 */
#include "CTR.h"

/* PSY-Q MATRIX: 3x3 rotation (short m[3][3] @ 0x00) + long translation (t[3] @ 0x14). */
typedef struct {
    short m[3][3];
    long  t[3];
} MATRIX;

extern void func_8006C3B0(MATRIX *out, MATRIX *m2, MATRIX *m3); /* MATH_MatrixMultiplication (rot 3x3) */
extern void func_8006C6F0(long *v0, long *v1);                  /* MatrixVecMultiply(m3->t, out->t) */

void MATH_MatrixMul(MATRIX *out, MATRIX *m2, MATRIX *m3)
{
    func_8006C3B0(out, m2, m3);
    func_8006C6F0(m3->t, out->t);

    out->t[0] += m2->t[0];
    out->t[1] += m2->t[1];
    out->t[2] += m2->t[2];
}
