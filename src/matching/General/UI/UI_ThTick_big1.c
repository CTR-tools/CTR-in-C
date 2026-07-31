typedef unsigned int uint;
typedef unsigned short u16;

extern char *D_8008D2AC;                                   /* gGT pointer */
extern void func_8006C3B0(int dst, int src1, int src2);    /* MATH_MatrixMultiplication */

/*
 * UI_ThTick_big1 @ 0x8004CA04 (SCUS_944.26 / NTSC-U 926)
 *
 * Byte-exact match: score 0, Branch/Stack/Register differences all 0.
 * Finished from the "near completed" prior grind set. NOTE: the prior score-5
 * snapshot was a FALSE match — it collapsed the show-path to `flags & (flags|0x80)`
 * (== flags), so it never set the 0x80 hide bit. This version is byte-exact AND
 * behaviorally correct (verified by disassembly + Ghidra).
 *
 * Per-frame tick for the large rank-number HUD element. Builds a uniform-scale
 * matrix (diag = obj->scale, off-diagonal 0) into inst->matrix, composes the
 * object orientation via MATH_MatrixMultiplication, then toggles the HIDE_MODEL
 * (0x80) flag: shown (bit cleared) only when the HUD is enabled and not in demo
 * mode ((gGT->hudState & 0xFF0100) == 0x100), hidden (bit set) otherwise.
 */
void func_8004CA04(int thread)
{
    int inst;
    u16 scale;
    int obj;

    obj  = *((int *) (thread + 0x30));   /* thread->object */
    inst = *((int *) (thread + 0x34));   /* thread->inst   */

    scale = *((u16 *) (obj + 6));                 /* obj->scale */
    *((u16 *) (inst + 0x32)) = 0;
    *((u16 *) (inst + 0x34)) = 0;
    *((u16 *) (inst + 0x36)) = 0;
    *((u16 *) (inst + 0x30)) = scale;             /* matrix.m[0][0] */

    scale = *((u16 *) (obj + 6));
    *((u16 *) (inst + 0x3A)) = 0;
    *((u16 *) (inst + 0x3C)) = 0;
    *((u16 *) (inst + 0x3E)) = 0;
    *((u16 *) (inst + 0x38)) = scale;             /* matrix.m[1][1] */

    *((u16 *) (inst + 0x40)) = *((u16 *) (obj + 6));  /* matrix.m[2][2] */

    func_8006C3B0(inst + 0x30, obj + 8, inst + 0x30);

    if (((*((uint *) (D_8008D2AC + 0x1D30))) & 0xFF0100) != 0x100)
    {
        *((uint *) (inst + 0x28)) |= 0x80;        /* hide  (set HIDE_MODEL) */
    }
    else
    {
        *((uint *) (inst + 0x28)) &= ~0x80;       /* show  (clear HIDE_MODEL) */
    }
}
