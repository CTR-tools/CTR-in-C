/*
 * VehPhysCrash_BounceSelf @ 0x8005CF64 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 1210, Branch 0 / Stack 0, Register 50 — a genuine
 * gcc-2.8.1 scheduling wall (opcode multiset is identical to retail; only delay-slot
 * fills and the placement of the div-by-6 magic block differ). Delivered NATURAL:
 * the aibox seed reached 970 only with (unsigned long long) type-pun hacks + permuter
 * PERM_RANDOMIZE/empty-block/duplicate-store spaghetti (all barred by the no-hacky-match
 * mandate) — this is the clean rewrite from the asm, verified behaviorally identical.
 *
 * Projects the position delta onto a surface normal (dot >> 12), and if the driver is
 * moving into the surface (sign of dot gated by boolOtherDriver), reflects the delta by
 * subtracting (dot*normal)/6 >> 9 per axis, clamps the new Y to 0x3200, and tracks the
 * peak |dot| in the gp-relative global vehicleCollisionImpactStrength (0x8008D9F4).
 *
 * saphi VehPhysCrash_BounceSelf (game/Vehicle/VehPhysCrash.c) audited CLEAN vs this asm + Ghidra.
 */

/* gp-relative (gp+0xA88 = 0x8008D9F4): sdata->vehicleCollisionImpactStrength (running max |dot|) */
int D_8008D9F4 = 0;

int VehPhysCrash_BounceSelf(short *normal, int *origin, int *vel, int boolOtherDriver)
{
    int diffX = vel[0] - origin[0];
    int diffY = vel[1] - origin[1];
    int diffZ = vel[2] - origin[2];
    int dot = (diffX * normal[0] + diffY * normal[1] + diffZ * normal[2]) >> 12;
    int projX, projY, projZ, absDot, newY;

    if (boolOtherDriver != 0) {
        if (dot <= 0) return 0;
    } else {
        if (dot >= 0) return 0;
    }

    absDot = (dot < 0) ? -dot : dot;
    if (D_8008D9F4 < absDot) D_8008D9F4 = absDot;

    projX = dot * normal[0];
    projY = dot * normal[1];
    projZ = dot * normal[2];

    diffX -= ((projX / 6 + (projX >> 31)) >> 9) - (projX >> 31);
    diffY -= ((projY / 6 + (projY >> 31)) >> 9) - (projY >> 31);
    diffZ -= ((projZ / 6 + (projZ >> 31)) >> 9) - (projZ >> 31);

    vel[0] = origin[0] + diffX;
    newY = origin[1] + diffY;
    if (vel[1] < newY && newY > 0x3200) newY = 0x3200;
    vel[1] = newY;
    vel[2] = origin[2] + diffZ;
    return 0;
}
