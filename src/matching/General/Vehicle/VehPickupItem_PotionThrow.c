/*
 * VehPickupItem_PotionThrow @ 0x800652C8 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 4005, Branch 0 / Stack 0, Register 33 —
 * a genuine gcc-2.8.1 register-allocation/scheduling wall on this 3-similar-block
 * dispatch. Finished from the "aibox" prior WIP set (the seed base.c was pure
 * hack-soup: PERM_RANDOMIZE(...), a volatile local, comma operators, do/while(0),
 * and duplicate/dead stores — all barred; rebuilt natural from the asm. Also
 * rejected a permuter false-friend at 3885 that hoisted the `velocity.y=0x30`
 * store out of the flags&1 block into the no-flag return-0 path (behaviorally wrong).
 *
 * Launches a thrown potion/mine projectile: sets its velocity from the source
 * instance's forward/right basis vectors (inst->matrix.m[0][2] @+0x34,
 * m[2][2] @+0x40) scaled by a throw strength chosen from `flags`:
 *   flags & 4  -> forward toss  : (basis * 15) >> 9         (== (basis*0x78)>>12)
 *   flags & 2  -> backward toss : (basis * -0x78) >> 12
 *   flags & 1  -> random spread : (basis * ((rng & 0x1F) - 0x10)) >> 12
 *   else       -> return 0 (nothing thrown)
 * then velocity.y = 0x30, crateInst(+8) = NULL, extraFlags(+0x28) |= 2; return 1.
 *
 * saphi game/Vehicle/VehPickupItem.c:283 VehPickupItem_PotionThrow audited CLEAN:
 * it uses a unified `throwVelocity` scalar (0x78 / -0x78 / rng) and computes
 * (m * throwVelocity) >> 12 uniformly. For the flags&4 path saphi's (m*0x78)>>12 is
 * BIT-IDENTICAL to retail's (m*15)>>9 [120/4096 == 15/512, and (a<<3)>>12 == a>>9].
 * Blocks B/C match directly. No saphi bug.
 */
typedef unsigned short ushort;
typedef unsigned int uint;

int D_8008D668 = 0;                        /* potion-throw RNG seed */
extern uint func_8006C684(void *seed);     /* MixRNG_Scramble */

ushort VehPickupItem_PotionThrow(int mine, int inst, uint flags)
{
    short velZ;
    short ret;

    if ((flags & 4) != 0)                  /* forward toss */
    {
        *(short *)(mine + 0xC) = (*(short *)(inst + 0x34) * 15) >> 9;
        *(short *)(mine + 0xE) = 0x30;
        *(int *)(mine + 8) = 0;
        *(ushort *)(mine + 0x28) |= 2;
        velZ = (*(short *)(inst + 0x40) * 15) >> 9;
        ret = 1;
    }
    else if ((flags & 2) != 0)             /* backward toss */
    {
        *(short *)(mine + 0xC) = (*(short *)(inst + 0x34) * -0x78) >> 12;
        *(short *)(mine + 0xE) = 0x30;
        *(int *)(mine + 8) = 0;
        *(ushort *)(mine + 0x28) |= 2;
        velZ = (*(short *)(inst + 0x40) * -0x78) >> 12;
        ret = 1;
    }
    else if ((flags & 1) != 0)             /* random spread */
    {
        int off = (func_8006C684(&D_8008D668) & 0x1F) - 0x10;
        *(short *)(mine + 0xC) = (*(short *)(inst + 0x34) * off) >> 12;
        *(short *)(mine + 0xE) = 0x30;
        *(int *)(mine + 8) = 0;
        *(ushort *)(mine + 0x28) |= 2;
        velZ = (*(short *)(inst + 0x40) * off) >> 12;
        ret = 1;
    }
    else
    {
        return 0;
    }
    *(short *)(mine + 0x10) = velZ;
    return ret;
}
