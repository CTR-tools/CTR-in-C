#include "CTR.h"

/* MATH_VectorNormalize(vec) @ 0x8003D378
 * Normalize the 3D short vector in place to unit length 0x1000 (1.0 in the
 * GTE's 1.19.12 fixed point): len = MATH_VectorLength(vec); if len != 0, each
 * component = (component << 12) / len. A zero vector is left unchanged.
 * Returns the length. The div-by-zero / INT_MIN overflow break() traps are
 * compiler-generated. */

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
} SVECTOR;

extern int func_8003D328(SVECTOR *vec);   /* MATH_VectorLength */

int MATH_VectorNormalize(SVECTOR *vec)
{
    int len;

    len = func_8003D328(vec);
    if (len != 0) {
        vec->vx = (vec->vx << 12) / len;
        vec->vy = (vec->vy << 12) / len;
        vec->vz = (vec->vz << 12) / len;
    }
    return len;
}
