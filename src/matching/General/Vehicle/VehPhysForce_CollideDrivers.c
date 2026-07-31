/*
 * VehPhysForce_CollideDrivers @ 0x8005EBAC (SCUS_944.26 / NTSC-U 926)
 *
 * NATURAL readable de-reused form (user-selected over the reuse-shaped 800 variant).
 * Behavior-exact: base score 1565, Branch Differences 0, Stack Differences 0,
 * Register Differences 25 — a genuine gcc-2.8.1 register-allocation/scheduling wall.
 * Every value has its own distinct named local instead of mirroring retail's register
 * reuse, which raises the honest score above the reuse-shaped variant but keeps the code
 * natural and readable. No volatile/type-pun/empty-block/register-asm/reuse hacks.
 * Disassembly-verified: identical observable effects (all stores/offsets/shifts) vs the
 * retail asm. Finished from the "near completed" prior grind set.
 *
 * Behavior (thread=a0, driver=a1), verified vs asm + saphi game/Vehicle/VehPhysForce.c:
 *   - velocity{x,y,z}@0x88/8c/90 -= accel{x,y,z}@0x3cc/3ce/3d0 (subu)
 *   - stepFlags@0xbc: KILL_PLANE(0x4000) -> collisionFlags@0xaa |= 1 (grab request)
 *   - turbo pads (single shared VehFire_Increment call site, reached via gotos to match
 *     retail): SUPER(0x2) -> (driver,0x78,5,0x800);
 *              TURBO(0x1) -> gGT->gameMode2(0x8) & CHEAT_TURBOPAD(0x100000) ?
 *                              (driver,0x78,5,0x800) : (driver,0x3c0,5,0x100)
 *   - WATER_BSP(0x8000): thread->inst@0x34 -> vertSplit@0x56=0, flags@0x28 |= SPLIT_LINE(0x2000);
 *                        else flags &= ~SPLIT_LINE
 *   - if !(thread->flags@0x1c & DISABLE_COLLISION 0x1000): build a search bucket at
 *     driver->posCurr>>8, dist=0x7fffffff, collide vs sibling + robot buckets, and if a
 *     driver was hit within (r1+r2)^2 -> VehPhysCrash_AnyTwoCars(thread,&search,&velocity)
 *   - if collisionFlags & SURFACE_PUSHBACK(0x2): if the dot of spsNormalVec with
 *     (posCurr>>8 - spsHitPos) (+4 on Y via quadBlockHeight) < 0, push velocity += diff<<6.
 *
 * saphi VehPhysForce_CollideDrivers audited CLEAN vs this asm + Ghidra.
 */
typedef unsigned int uint;
typedef unsigned short ushort;

extern char *D_8008D2AC;                               /* gGT */
extern void func_8005ABFC(int, int, int, int);         /* VehFire_Increment */
extern void func_80042348(int, void *);                /* PROC_CollidePointWithBucket */
extern void func_8005D404(int, int, int *);            /* VehPhysCrash_AnyTwoCars */

void VehPhysForce_CollideDrivers(int thread, int driver)
{
  uint stepFlags;
  int turboPower, fireReserve;
  int waterInst;
  int hitDriver;
  int radiusSum;
  int deltaX, deltaY, deltaZ;
  short hitY, normalY;
  int surfaceDot;
  int velYOld, pushY;
  struct
  {
    short sv[3];
    short pad;
    uint ptr;
    int dist;
  } search;

  *((int *) (driver + 0x88)) -= (int) (*((short *) (driver + 0x3cc)));
  *((int *) (driver + 0x8c)) -= (int) (*((short *) (driver + 0x3ce)));
  stepFlags = *((uint *) (driver + 0xbc));
  *((int *) (driver + 0x90)) -= (int) (*((short *) (driver + 0x3d0)));
  if ((stepFlags & 0x4000) != 0)
  {
    *((ushort *) (driver + 0xaa)) |= 1;
  }

  /* Turbo pads: one shared VehFire_Increment call site reached via gotos (matches retail). */
  if ((stepFlags & 2) == 0)
  {
    if ((stepFlags & 1) == 0)
    {
      goto after_turbo;
    }
    turboPower = 0x3c0;
    if ((*((uint *) (D_8008D2AC + 8)) & 0x100000) == 0)   /* CHEAT_TURBOPAD */
    {
      goto turbo_normal;
    }
  }
  turboPower = 0x78;
  fireReserve = 0x800;
  goto turbo_fire;
turbo_normal:
  fireReserve = 0x100;
turbo_fire:
  func_8005ABFC(driver, turboPower, 5, fireReserve);

after_turbo:
  if ((stepFlags & 0x8000) != 0)   /* WATER_BSP */
  {
    waterInst = *((int *) (thread + 0x34));
    *((short *) (waterInst + 0x56)) = 0;
    *((uint *) (waterInst + 0x28)) |= 0x2000;
  }
  else
  {
    *((uint *) ((*((int *) (thread + 0x34))) + 0x28)) &= ~0x2000;
  }

  if ((*((uint *) (thread + 0x1c)) & 0x1000) == 0)   /* !DISABLE_COLLISION */
  {
    search.sv[0] = (short) ((*((int *) (driver + 0x2d4))) >> 8);
    search.sv[1] = (short) ((*((int *) (driver + 0x2d8))) >> 8);
    search.sv[2] = (short) ((*((int *) (driver + 0x2dc))) >> 8);
    search.dist = 0x7fffffff;
    search.ptr = 0;
    func_80042348(*((int *) (thread + 0x10)), search.sv);
    func_80042348(*((int *) (D_8008D2AC + 0x1b40)), search.sv);
    hitDriver = (int) search.ptr;
    if (hitDriver != 0)
    {
      radiusSum = ((int) (*((short *) (thread + 0x42)))) + ((int) (*((short *) (hitDriver + 0x42))));
      if (search.dist < radiusSum * radiusSum)
      {
        func_8005D404(thread, (int) search.sv, (int *) (driver + 0x88));
      }
    }
  }

  if ((*((ushort *) (driver + 0xaa)) & 2) != 0)   /* SURFACE_PUSHBACK */
  {
    deltaX = ((*((int *) (driver + 0x2d4))) >> 8) - ((int) (*((short *) (driver + 0xac))));
    deltaZ = ((*((int *) (driver + 0x2dc))) >> 8) - ((int) (*((short *) (driver + 0xb0))));
    hitY = *((short *) (driver + 0xae));
    normalY = *((short *) (driver + 0xb8));
    deltaY = (((*((int *) (driver + 0x2d0))) >> 8) - ((int) hitY)) + 4;
    surfaceDot = (((*((short *) (driver + 0xb4))) * deltaX)
               + ((*((short *) (driver + 0xb6))) * deltaY))
               + (normalY * deltaZ);
    if (surfaceDot < 0)
    {
      velYOld = *((int *) (driver + 0x8c));
      pushY = ((*((int *) (driver + 0x2d8))) >> 8) - ((int) hitY);
      *((int *) (driver + 0x88)) += deltaX * 0x40;
      *((int *) (driver + 0x8c)) = (pushY * 0x40) + velYOld;
      *((int *) (driver + 0x90)) += deltaZ * 0x40;
    }
  }
  return;
}
