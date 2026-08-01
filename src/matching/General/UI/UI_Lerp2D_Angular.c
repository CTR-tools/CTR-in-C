/*
 * UI_Lerp2D_Angular @ 0x8004EAA8 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 3205, Branch 0 / Stack 0, Register 37 — a genuine
 * gcc-2.8.1 instruction-scheduling wall (retail fills the two integer-mult delay slots
 * asymmetrically between the two branches; natural C emits nops there). A 130s -j4 grind
 * found nothing below 3205 without an artificial dead-store. From aibox staging — the
 * prior seed reached 2345 only via a dead `sVar4 = product >> 16` store + an `inline_fn`
 * shift-wrapper (codegen-steering artifacts, not natural C); those were removed.
 * NOT a false-friend — the seed was behaviorally correct.
 *
 * Computes a standings-icon (x,y) during a rank-change swing animation:
 *   x = 0x14 ± (MATH_Sin((frameCounter<<11)/5) * 0x14 >> 12)   (+ if just-passed, - else)
 *   y = 0x39 + drawnPosition*0x1B + ((absolute-drawn)*0x1B*frameCounter)/5   (eased vert step)
 * The /5 uses the 0x66666667 signed div-magic; sign(mulhi)==sign(dividend) since the magic
 * is positive, so `(short)(hi>>0x1f)` in the seed and `(short)(product>>0x1f)` are equal —
 * this delivery uses the product's sign (matches Ghidra/decomp UI_20_Lerp2D_Angular).
 *
 * saphi UI_Lerp2D_Angular (game/UI/UI_Lerp2D.c) audited CLEAN vs this asm + Ghidra.
 */
typedef unsigned int uint;
typedef long long longlong;
typedef unsigned long long ulonglong;

extern int func_8003D184(int angle);   /* MATH_Sin */

void UI_Lerp2D_Angular(short *ptrPos, short drawnPosition, short absolutePosition, short frameCounter)
{
  int iSin;
  int iStepNum;
  int iVertEased;

  iStepNum = (int)drawnPosition;
  iVertEased = (int)absolutePosition;
  if (iVertEased < iStepNum)
  {
    iSin = func_8003D184(((int)frameCounter << 0xb) / 5);
    iStepNum = frameCounter * 0x1b * (iVertEased - iStepNum);
    iVertEased = (int)((ulonglong)((longlong)iStepNum * 0x66666667) >> 0x20);
    *ptrPos = (short)(iSin * 0x14 >> 0xc) + 0x14;
  }
  else
  {
    iSin = func_8003D184(((int)frameCounter << 0xb) / 5);
    iStepNum = frameCounter * 0x1b * (iVertEased - iStepNum);
    iVertEased = (int)((ulonglong)((longlong)iStepNum * 0x66666667) >> 0x20);
    *ptrPos = 0x14 - (short)(iSin * 0x14 >> 0xc);
  }
  ptrPos[1] = drawnPosition * 0x1b + ((short)(iVertEased >> 1) - (short)(iStepNum >> 0x1f)) + 0x39;
}
