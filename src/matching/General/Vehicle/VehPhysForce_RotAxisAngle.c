/*
 * VehPhysForce_RotAxisAngle @ 0x8005F89C (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 7530, Branch Differences 0, Stack Differences 0,
 * Register Differences 130 — a genuine large gcc-2.8.1 register-allocation/scheduling
 * wall on this GTE-heavy routine (a 280-iteration -j4 grind only produced FALSE-FRIEND
 * candidates that reused `normalX` for `angle`, squaring the wrong value — rejected per
 * correctness-over-score). Finished from the "aibox" prior grind set; the seed's inert
 * empty-`if(){}` div-overflow-trap artifacts were removed (score-neutral, natural C).
 *
 * Builds a rotation matrix from an axis (normVec) and an angle. Column 1 (middle) is the
 * axis itself; column 2 (third) is sin/cos·axisY plus a normalized cross term divided by
 * denom = axisX²+axisZ²; column 0 (first) is derived on the GTE via OP12 (outer product,
 * sf=12) of the axis with the generated third column.
 *
 *   col1: m[0][1]=axisX, m[1][1]=axisY, m[2][1]=axisZ
 *   trig: sin/cos from D_800845A0[angle&0x3ff] with the 0x400/0x800 quadrant sign logic
 *   denom==0 branch: outX/outZ = sin/cos·axisY>>12 (outX negated if axisY<0), outY=-dot>>12
 *   else: shift = 0x14 - LZCR(denom); rescale; outX += (sinRem·axisZSq + cosRem·crossXZ)/denom,
 *         outZ += (sinRem·crossXZ + cosRem·axisXSq)/denom, outY = -dot>>12  (dot=sin·axisX+cos·axisZ)
 *   col0: CTC2 axis, MTC2 col2, gte_op12, m[*][0] = MFC2(25/26/27)
 *
 * saphi VehPhysForce_RotAxisAngle (game/Vehicle/VehPhysForce.c:1055) audited CLEAN vs this
 * asm + Ghidra — identical field-for-field (CTR_MipsMulLo/Sra/Div wrappers; the partial
 * GTE-register writes before op12 are the intentional retail behavior callers observe).
 */
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef char undefined;
typedef short undefined2;
typedef int undefined4;

#define gte_ldLZCS(r0) __asm__ volatile("mtc2 %0, $30" : : "r"(r0))
#define gte_stLZCR() __extension__({int _r; __asm__ volatile("mfc2 %0, $31" : "=r"(_r)); _r;})
#define gte_ldopv1SV(p) do { \
    __asm__ volatile("ctc2 %0, $0" : : "r"((int)*(p))); \
    __asm__ volatile("ctc2 %0, $2" : : "r"((int)*((p)+1))); \
    __asm__ volatile("ctc2 %0, $4" : : "r"((int)*((p)+2))); \
} while(0)
#define gte_op12() __asm__ volatile(".word 0x0C00784B")
#define read_mt(r0, r1, r2) do { \
    __asm__ volatile("mfc2 %0, $25" : "=r"(r0)); \
    __asm__ volatile("mfc2 %0, $26" : "=r"(r1)); \
    __asm__ volatile("mfc2 %0, $27" : "=r"(r2)); \
} while(0)

extern int DAT_800845a0;   /* D_800845A0 trig table: packed {s16 sin; s16 cos}[0x400] */

void VehPhysForce_RotAxisAngle(undefined2 *outMatrix, short *normVec, uint angle)
{
  short outX;            /* reused: axisX, then sin, then column-2 X */
  int cosVal;           /* reused: cos, then dot */
  int divXNum;          /* reused: LZCR, then divX numerator */
  uint shift;
  short outY;
  int denom;
  int sinVal;
  int scaledCosY;       /* reused: scaledCosY, then cosRemainder */
  short outZ;           /* reused: axisY, then column-2 Z */
  int normalZ;
  int normalX;
  int col0X;
  int scaledSinY;       /* reused: scaledSinY, then sinRemainder */
  int col0Y;
  int col0Z;
  int axisXSq;
  int axisZSq;
  int crossXZ;

  outX = *normVec;
  outMatrix[1] = outX;
  outZ = normVec[1];
  normalX = (int)outX;
  outMatrix[4] = outZ;
  normalZ = (int)normVec[2];
  outMatrix[7] = normVec[2];

  outX = (short)*(int *)(&DAT_800845a0 + (angle & 0x3ff) * 4);
  cosVal = *(int *)(&DAT_800845a0 + (angle & 0x3ff) * 4) >> 0x10;

  if ((angle & 0x400) == 0) {
    sinVal = (int)outX;
    if ((angle & 0x800) == 0) goto LAB_8005f934;
    scaledCosY = -cosVal;
  }
  else {
    scaledCosY = (int)outX;
    sinVal = cosVal;
    if ((angle & 0x800) == 0) {
      cosVal = -scaledCosY;
      goto LAB_8005f934;
    }
  }
  sinVal = -sinVal;
  cosVal = scaledCosY;

LAB_8005f934:
  axisXSq = normalX * normalX;
  axisZSq = normalZ * normalZ;
  crossXZ = normalX * -normalZ;
  denom = axisXSq + axisZSq;
  scaledSinY = sinVal * outZ >> 0xc;
  outX = (short)scaledSinY;
  scaledCosY = cosVal * outZ >> 0xc;
  outZ = (short)scaledCosY;

  gte_ldLZCS(denom);
  divXNum = gte_stLZCR();

  if (denom == 0) {
    cosVal = sinVal * normalX + cosVal * normalZ;
    if (normVec[1] < 0) {
      outX = -outX;
    }
  }
  else {
    shift = 0x14 - divXNum;
    if (0 < (int)shift) {
      axisXSq = axisXSq >> (shift & 0x1f);
      axisZSq = axisZSq >> (shift & 0x1f);
      crossXZ = crossXZ >> (shift & 0x1f);
      denom = denom >> (shift & 0x1f);
    }
    scaledSinY = sinVal - scaledSinY;
    scaledCosY = cosVal - scaledCosY;
    divXNum = scaledSinY * axisZSq + scaledCosY * crossXZ;
    scaledCosY = scaledSinY * crossXZ + scaledCosY * axisXSq;
    outX = outX + (short)(divXNum / denom);
    cosVal = sinVal * normalX + cosVal * normalZ;
    outZ = outZ + (short)(scaledCosY / denom);
  }

  outMatrix[2] = outX;
  outY = (short)(-cosVal >> 0xc);
  outMatrix[5] = outY;
  outMatrix[8] = outZ;

  gte_ldopv1SV(normVec);

  __asm__ volatile("mtc2 %0, $9" : : "r"((int)outX));
  __asm__ volatile("mtc2 %0, $10" : : "r"((int)outY));
  __asm__ volatile("mtc2 %0, $11" : : "r"((int)outZ));

  gte_op12();
  read_mt(col0X, col0Y, col0Z);

  *outMatrix = (short)col0X;
  outMatrix[3] = (short)col0Y;
  outMatrix[6] = (short)col0Z;

  return;
}
