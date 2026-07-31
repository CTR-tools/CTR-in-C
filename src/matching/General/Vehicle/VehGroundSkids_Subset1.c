/*
 * VehGroundSkids_Subset1 @ 0x8005C120 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score ~1315, Branch 0 / Stack 0 (residual is a
 * genuine gcc-2.8.1 hand-scheduling / register-allocation wall). Retail hand-fills
 * every load-delay slot with a useful move (e.g. saving prevXY into t2 in the slot
 * after `lw render`, and reusing param reg a1 for the prim cursor); gcc-2.8.1 from
 * natural C emits `nop` in those slots and allocates prim into t0 / gGT into t2,
 * which cascades into register-name differences throughout. A 380-iteration -j4
 * grind only reached 1190 (a behavior-neutral else-branch statement swap) — no
 * natural C removes the scheduling gap, and no hacks are permitted. Finished from
 * the "near completed" prior grind set.
 *
 * Builds one skid-mark POLY_GT4 (13 words) into the back buffer's primitive pool
 * (if there's room), fills its 4 vertex colors (near/far), packed XY from the
 * current/previous world points, and the texture UV/tpage words from icon 0x2f;
 * the tpage brightness bit (0x00600000 vs 0x00400000) is chosen by the segment
 * flag (scratch+0x24 bit 0). Finally links the packet into the OT at depth>>6.
 *
 * saphi VehGroundSkids_Subset1 (game/Vehicle/VehGroundSkids.c) audited CLEAN vs
 * this asm + Ghidra (field-for-field identical).
 */

typedef unsigned int uint;

extern char *D_8008D2AC;   /* sdata->gGT */

/* currXY/prevXY: packed XY word pairs; depth: OT depth; scratch: VehGroundSkids scratch */
void VehGroundSkids_Subset1(uint *currXY, uint *prevXY, int depth, int scratch)
{
    char *gGT;
    int   backBuffer;
    uint *poly;         /* POLY_GT4 cursor */
    uint  tpage;
    uint  brightness;
    uint *otSlot;

    gGT = D_8008D2AC;
    backBuffer = *((int *) (D_8008D2AC + 0x10));
    poly = *((uint **) (backBuffer + 0x80));                 /* primMem.cursor */

    if ((poly + 0xd) <= (*((uint **) (backBuffer + 0x84))))  /* primMem.guardEnd — room for 13 words? */
    {
        *((uint **) (backBuffer + 0x80)) = poly + 0xd;

        poly[1]   = *((uint *) (scratch + 0x1c));            /* r0 = colorNear */
        poly[4]   = *((uint *) (scratch + 0x1c));            /* r1 = colorNear */
        poly[7]   = *((uint *) (scratch + 0x20));            /* r2 = colorFar  */
        poly[10]  = *((uint *) (scratch + 0x20));            /* r3 = colorFar  */

        poly[2]   = *currXY;                                 /* x0 */
        poly[5]   = currXY[1];                               /* x1 */
        poly[8]   = *prevXY;                                 /* x2 */
        poly[0xb] = prevXY[1];                               /* x3 */

        poly[3] = *((uint *) ((*((int *) (gGT + 0x1fa8))) + 0x14));  /* u0 = icon texWord[0] */

        if (((*((uint *) (scratch + 0x24))) & 1) != 0)       /* segment flag */
        {
            tpage      = (*((uint *) ((*((int *) (gGT + 0x1fa8))) + 0x18))) & 0xff9fffff;
            brightness = 0x600000;
        }
        else
        {
            tpage      = (*((uint *) ((*((int *) (gGT + 0x1fa8))) + 0x18))) & 0xff9fffff;
            brightness = 0x400000;
        }
        poly[6] = tpage | brightness;                        /* u1 = tpage */

        *((short *) (poly + 9))   = *((unsigned short *) ((*((int *) (D_8008D2AC + 0x1fa8))) + 0x1c)); /* u2 */
        *((short *) (poly + 0xc)) = *((unsigned short *) ((*((int *) (D_8008D2AC + 0x1fa8))) + 0x1e)); /* u3 */

        otSlot = (uint *) ((*((int *) ((*((int *) (scratch + 0x18))) + 0xf4))) + ((depth >> 6) * 4));
        poly[0] = (*otSlot) | 0xc000000;                     /* tag: next = *ot | len<<24 */
        *otSlot = ((uint) poly) & 0xffffff;                  /* *ot = &poly */
    }
    return;
}
