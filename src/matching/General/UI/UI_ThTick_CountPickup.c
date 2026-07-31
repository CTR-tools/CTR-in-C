/*
 * UI_ThTick_CountPickup @ 0x8004C718 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: Branch 0 / Stack 0, Register 3 (base score 15).
 * The 3-register residual is a genuine gcc-2.8.1 v0-vs-v1 coloring wall in the
 * final flag block (retail keeps the result in v0; gcc-from-source colors it v1).
 * The only sub-15 permuter candidate was behaviorally BROKEN (it collapsed the
 * `flags |= 0x80` set-path into `flags = old & (old|0x80)` == old, dropping the
 * bit) — rejected per the no-hacky-match rule. This is the natural, correct form.
 * vs SCUS_944.26.exe; finished from the "near completed" prior grind set.
 *
 * Per-frame thread tick for the wumpa-count HUD pickup element:
 *   - forces the instance colour to 0xFFFF0000,
 *   - in 1-player games, fades the element (alphaScale) by the shared
 *     wumpaShineResult once the player holds >= 10 wumpas (0 below that),
 *   - spins the element (rot.y) faster for the time-crate model than others,
 *   - rebuilds its rotation matrix, and
 *   - shows/hides it via the HUD-enabled / not-demo flag test.
 */
typedef unsigned int uint;

/* gGT pointer (absolute) + sdata->wumpaShineResult (gp-relative, gp+0xA28). */
extern char *D_8008D2AC;
short D_8008D994 = 0;

extern void func_8006C2A4(int mat, int rot);        /* ConvertRotToMatrix */
extern void func_8006C3B0(int dst, int a, int b);   /* MatrixRotate       */

void func_8004C718(int bucket)
{
    char *gGT;
    short rotSpd;
    uint flags;
    int mat;
    int inst;
    int obj;

    obj  = *((int *) (bucket + 0x30));   /* bucket->object */
    inst = *((int *) (bucket + 0x34));   /* bucket->inst   */
    gGT  = D_8008D2AC;

    *((uint *) (inst + 0x24)) = 0xFFFF0000;   /* inst->colorRGBA */

    /* do/while(0) mirrors the retail goto-tail layout (real body, not an empty block). */
    do
    {
        if (gGT[0x1CA8] == 1)                                              /* numPlyrCurrGame == 1 */
        {
            if ((*((short *) (0x10 + (*((int *) (inst + 0x18)))))) != 0x5C)   /* model->id != TIME_CRATE */
            {
                if ((*((signed char *) ((*((int *) (gGT + 0x24EC))) + 0x30))) >= 10)   /* drivers[0]->numWumpas */
                {
                    *((short *) (inst + 0x22)) = ((int) (((short) D_8008D994) - 0x80)) << 4;
                }
                else
                {
                    *((unsigned short *) (inst + 0x22)) = 0;   /* alphaScale: no shine */
                }
            }
        }

        if ((*((short *) ((*((int *) (inst + 0x18))) + 0x10))) == 0x5C)   /* time-crate: slower spin */
        {
            rotSpd = (*((short *) (obj + 2))) + 0x40;
        }
        else
        {
            rotSpd = (*((short *) (obj + 2))) + 0x80;
        }
        *((short *) (obj + 2)) = rotSpd;   /* obj->rot.y */
    }
    while (0);

    mat = inst + 0x30;                 /* &inst->matrix */
    func_8006C2A4(mat, obj);           /* ConvertRotToMatrix(mat, obj->rot) */
    func_8006C3B0(mat, obj + 8, mat);  /* MatrixRotate(mat, &obj->m, mat)   */

    /* HUD enabled and not demo mode -> visible (clear 0x80); else invisible (set 0x80). */
    if (((*((uint *) (D_8008D2AC + 0x1D30))) & 0xFF0100) != 0x100)
    {
        flags = (*((uint *) (inst + 0x28))) | 0x80;
    }
    else
    {
        flags = 0xFFFFFF7F;
        flags = (*((uint *) (inst + 0x28))) & flags;
    }
    *((uint *) (inst + 0x28)) = flags;   /* inst->flags */
}
