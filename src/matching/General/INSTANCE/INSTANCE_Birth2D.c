/*
 * INSTANCE_Birth2D @ 0x800308E4 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 45, Branch Differences 0, Stack Differences 0,
 * Register Differences 9 — a genuine gcc-2.8.1 register-allocation wall (a 180s
 * permuter grind of 77 iterations never beat 45). Finished from the "near completed"
 * prior grind set.
 *
 * Allocates a 2D instance from the instance JIT pool (gGT+0x18F8), initialises it
 * via INSTANCE_Birth with draw flags 0x40F, points player-0's per-player draw
 * record (idpp[0] at inst+0x74) at gGT->pushBuffer_UI (gGT+0x1388), then zeroes
 * idpp[i].pushBuffer (stride 0x88) for the remaining active players i=1..numPlyr-1.
 * Retail performs the inst+0x74 write even when the pool returns NULL (a PS1 low-RAM
 * write); saphi guards that under CTR_NATIVE (intentional adaptation, not a bug).
 *
 * saphi INSTANCE_Birth2D audited CLEAN vs this asm + Ghidra.
 */

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned int undefined4;

extern char *D_8008D2AC;                                   /* gGT */
extern int func_800310D4(void *pool);                      /* JitPool_Add */
extern void func_80030778(int inst, undefined4 model, undefined4 name,
                          undefined4 thread, undefined4 flags); /* INSTANCE_Birth */

int INSTANCE_Birth2D(undefined4 model, undefined4 name, undefined4 thread)
{
  int inst;
  unsigned char numPlyrOff;
  int numPlyrOff2;
  int idppCursor;
  char *gGT;
  int plyr;
  gGT = D_8008D2AC;
  inst = func_800310D4(D_8008D2AC + 0x18F8);               /* JitPools.instance */
  if (inst != 0)
  {
    func_80030778(inst, model, name, thread, 0x40F);
  }
  gGT = D_8008D2AC;
  *((char **) (inst + 0x74)) = D_8008D2AC + 5000;          /* idpp[0].pushBuffer = &gGT->pushBuffer_UI (gGT+0x1388) */
  plyr = 1;
  if (plyr < ((int) ((uint) ((byte) gGT[0x1CA8]))))        /* numPlyrCurrGame */
  {
    numPlyrOff = 0x1CA8;
    idppCursor = inst;
    numPlyrOff2 = numPlyrOff;
    do
    {
      *((undefined4 *) (idppCursor + 0xFC)) = 0;           /* idpp[plyr].pushBuffer = 0 */
      idppCursor = idppCursor + 0x88;
      plyr = plyr + 1;
    }
    while (plyr < ((int) ((uint) ((byte) gGT[numPlyrOff2]))));
  }
  return inst;
}
