/*
 * UI_CupStandings_FinalizeCupRanks @ 0x8005607C (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 5430, Branch 0 / Stack 0, Register 78 — a genuine
 * gcc-2.8.1 register-allocation/scheduling wall (register-heavy, call-free routine).
 * From aibox staging; vs SCUS_944.26.exe.
 *
 * ⚠️ The aibox seed scored a deceptively-low 2985 by being a FALSE-FRIEND: it reused the
 * tie-count local (the outer selection-sort bound) as the swap-slot byte-offset scratch
 * (`tieCount = idx >> 0xE`), which collapsed the outer bound to 0 after the first pass, so
 * the tie-break sort ran ONCE instead of tieCount+1 times (bug only visible on tied cup
 * scores). Retail keeps the bound invariant in $t1 (reloading $a2 from $t1 each outer
 * iteration) and computes the slot offset in a separate register. Repaired here: the bound
 * (`tieCount`) is a dedicated invariant and the swap slot offset is computed inline. Also
 * removed a barred `do{}while(0)` empty-block hack. Correctness over score.
 *
 * Behavior: over the top (up to 4) drivers, count how many are tied with rank[0] on the
 * gGT+0x1E60 field (tieCount), then selection-sort those tieCount+1 slots ascending by the
 * driver's +0x24EC->+0x482 field, swapping entries in the cup-rank array D_80086D44[].
 *
 * saphi UI_CupStandings_FinalizeCupRanks audited CLEAN vs this asm + Ghidra.
 */

typedef unsigned int uint;
typedef unsigned char byte;

extern char *D_8008D2AC;   /* gGT */
extern int D_80086D44;     /* cupRankArray[] (one int per rank slot) */

void UI_CupStandings_FinalizeCupRanks(void)
{
  char *gGT;
  int scan;
  uint numDrivers;
  int scanIdx;
  int *slot;
  int slotIdx;
  short bestVal;
  short tieCount;
  int bestIndex;

  gGT = D_8008D2AC;
  bestIndex = -1;
  numDrivers = ((uint) ((byte) D_8008D2AC[0x1CA8])) + ((uint) ((byte) D_8008D2AC[0x1CAB]));
  tieCount = 0;
  if (4 < (int) numDrivers)
  {
    numDrivers = 4;
  }
  slotIdx = 1;
  if (1 < (int) numDrivers)
  {
    tieCount = 0;
    do
    {
      scan = slotIdx << 0x10;
      slotIdx = slotIdx + 1;
      if ((*((int *) ((D_8008D2AC + ((*((int *) (((int) (&D_80086D44)) + (scan >> 0xE)))) * 4)) + 0x1E60))) != (*((int *) ((D_8008D2AC + (D_80086D44 * 4)) + 0x1E60))))
      {
        break;
      }
      tieCount = tieCount + 1;
    }
    while (((slotIdx * 0x10000) >> 0x10) < (int) numDrivers);
  }
  slotIdx = 0;
  if (0 < (((int) tieCount) + 1))
  {
    do
    {
      bestVal = 99;
      scanIdx = (int) ((short) slotIdx);
      scan = slotIdx;
      while (scanIdx < (((int) tieCount) + 1))
      {
        if ((*((short *) ((*((int *) ((gGT + ((*((int *) (((int) (&D_80086D44)) + ((scan << 0x10) >> 0xE)))) * 4)) + 0x24EC))) + 0x482))) < bestVal)
        {
          bestIndex = scan;
          bestVal = *((short *) ((*((int *) ((gGT + ((*((int *) (((int) (&D_80086D44)) + ((scan << 0x10) >> 0xE)))) * 4)) + 0x24EC))) + 0x482));
        }
        scan = scan + 1;
        scanIdx = scan * 0x10000;
        scanIdx = scanIdx >> 0x10;
      }

      scan = slotIdx << 0x10;
      slotIdx = slotIdx + 1;
      slot = (int *) (((int) (&D_80086D44)) + (scan >> 0xE));
      bestVal = *((short *) slot);
      *slot = (int) ((short) bestIndex);
      (&D_80086D44)[(int) ((short) bestIndex)] = (int) bestVal;
    }
    while (((slotIdx * 0x10000) >> 0x10) < (((int) tieCount) + 1));
  }
}
