/*
 * VehGroundShadow_Main @ 0x8005B720 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: Branch 0 / Stack 0, base score 19850 (Register 458) — a LARGE
 * gcc-2.8.1 register-allocation/scheduling residual on this 477-line GTE routine.
 * NOT byte-close; byte-exact is a future dedicated hand-match. Structurally faithful to the
 * recovered Ghidra decompile (verified vs asm: no false-friend, no barred hacks — the
 * __asm__ volatile blocks are legitimate GTE intrinsics). Generic decompiler locals
 * (puVar/iVar/uVar) left as-is (full de-rename is a future pass); the scratchpad cursors
 * follow the Ghidra plate names.  From aibox staging.
 *
 * Draws the blob/ground shadows under all karts: stages two shadow textures via
 * VehGroundShadow_Subset1 (func_8005b6b8) into the PSX scratchpad, sets up the GTE, then per
 * player-viewport and per active driver projects a distance-faded shadow quad (color ramp
 * gGT+0x180..0x200 -> 0x1f intensity) into the back-buffer prim list, bailing if the prim
 * buffer is nearly full. Calls VehPhysForce_RotAxisAngle (func_8005f89c) for the local-axis
 * setup.  saphi game/Vehicle/VehGroundShadow.c:299 VehGroundShadow_Main audited CLEAN vs this
 * asm + Ghidra.
 */
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef char undefined;
typedef short undefined2;
typedef int undefined4;
typedef struct { short vx, vy, vz, pad; } SVECTOR;
typedef struct { short m[3][3]; short pad; int t[3]; } MATRIX;
typedef struct { int vx, vy, vz; } VECTOR;

#define gte_SetGeomOffset(ofx, ofy) do { \
    int _ox = (int)(ofx) << 16; \
    int _oy = (int)(ofy) << 16; \
    __asm__ volatile("ctc2 %0, $24" :: "r"(_ox)); \
    __asm__ volatile("ctc2 %0, $25" :: "r"(_oy)); \
} while(0)
#define gte_ldH(h) __asm__ volatile("ctc2 %0, $26" :: "r"(h))
#define gte_SetRotMatrix(r0) __asm__ volatile( \
    "lw $12, 0(%0);" "lw $13, 4(%0);" \
    "ctc2 $12, $0;" "ctc2 $13, $1;" \
    "lw $12, 8(%0);" "lw $13, 12(%0);" "lw $14, 16(%0);" \
    "ctc2 $12, $2;" "ctc2 $13, $3;" "ctc2 $14, $4" \
    :: "r"(r0) : "$12", "$13", "$14")
#define gte_SetTransVector(v) __asm__ volatile( \
    "lw $12, 0(%0);" "lw $13, 4(%0);" "lw $14, 8(%0);" \
    "ctc2 $12, $5;" "ctc2 $13, $6;" "ctc2 $14, $7" \
    :: "r"(v) : "$12", "$13", "$14")
#define gte_SetLightMatrix(m) __asm__ volatile( \
    "lw $12, 0(%0);" "lw $13, 4(%0);" \
    "ctc2 $12, $8;" "ctc2 $13, $9;" \
    "lw $12, 8(%0);" "lw $13, 12(%0);" "lw $14, 16(%0);" \
    "ctc2 $12, $10;" "ctc2 $13, $11;" "ctc2 $14, $12" \
    :: "r"(m) : "$12", "$13", "$14")
#define gte_ldv0(v) __asm__ volatile("lwc2 $0, 0(%0)\nlwc2 $1, 4(%0)" :: "r"(v))
#define gte_rtv0() __asm__ volatile("nop\nnop\ncop2 0x0486012")
#define gte_llv0() __asm__ volatile("nop\nnop\ncop2 0x004A6012")
#define gte_stMAC3() __extension__({ int _r; __asm__ volatile("mfc2 %0, $27" : "=r"(_r)); _r; })
#define gte_stMAC1() __extension__({ int _r; __asm__ volatile("mfc2 %0, $25" : "=r"(_r)); _r; })
#define gte_stMAC2() __extension__({ int _r; __asm__ volatile("mfc2 %0, $26" : "=r"(_r)); _r; })
#define gte_ldv3c(v) __asm__ volatile( \
    "lwc2 $0, 0(%0);" "lwc2 $1, 4(%0);" "lwc2 $2, 8(%0);" \
    "lwc2 $3, 12(%0);" "lwc2 $4, 16(%0);" "lwc2 $5, 20(%0)" \
    :: "r"(v))
#define gte_rtpt() __asm__ volatile("nop\nnop\ncop2 0x0280030")
#define gte_stsxy3c(p) __asm__ volatile( \
    "swc2 $12, 0(%0);" "swc2 $13, 4(%0);" "swc2 $14, 8(%0)" \
    :: "r"(p) : "memory")
#define gte_stsz3c(p) __asm__ volatile( \
    "swc2 $17, 0(%0);" "swc2 $18, 4(%0);" "swc2 $19, 8(%0)" \
    :: "r"(p) : "memory")

extern uint DAT_1f800000;
extern uint _DAT_1f800090;
extern uint DAT_1f800094;
extern uint DAT_1f800098;
extern uint DAT_1f800224;
extern uint DAT_1f800230;
extern unsigned char *PTR_DAT_8008d2ac;
extern int func_8005b6b8(void *, int);
extern void func_8005f89c(MATRIX *, int);

void VehGroundShadow_Main(void)
{
  undefined *puVar1;
  char cVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  short sVar8;
  int iVar9;
  MATRIX *pMVar10;
  int iMac3;
  int iVar11;
  uint *puVar12;
  uint uVar13;
  short sVar14;
  undefined *puVar15;
  uint *puVar16;
  int iVar17;
  short sVar18;
  int iVar19;
  int iVar20;
  uint uVar21;
  MATRIX *r0;
  uint *puVar22;
  uint *puVar23;
  uint *puVar24;
  uint *puVar25;
  undefined2 *puVar26;
  SVECTOR *r0_00;
  undefined *puVar27;
  uint *puVar28;
  undefined auStack72[32];

  puVar27 = auStack72;
  puVar23 = &DAT_1f800000;

  iVar4 = func_8005b6b8(&DAT_1f800224, 0);

  if (
      ((iVar4 != 0) &&
       (iVar4 = func_8005b6b8(&DAT_1f800230, 1), iVar4 != 0)) &&
      (puVar28 = *(uint **)(*(int *)(PTR_DAT_8008d2ac + 0x10) + 0x80),
       puVar28 + 0x140 < *(uint **)(*(int *)(PTR_DAT_8008d2ac + 0x10) + 0x84))
     )
  {
    _DAT_1f800090 = 0;
    DAT_1f800094 = 0;
    DAT_1f800098 = 0;
    gte_SetTransVector((VECTOR *)&_DAT_1f800090);

    iVar4 = 0;

    puVar15 = (undefined *)((uint)puVar23 | 0xb6);
    uVar21 = (uint)puVar23 | 0xa4;

    do {
      uVar6 = uVar21;
      puVar1 = PTR_DAT_8008d2ac;

      iVar11 = *(int *)(PTR_DAT_8008d2ac + iVar4 * 4 + 0x24ec);

      if (iVar11 == 0) {
        *(undefined4 *)(puVar15 + 2) = 0;
      }
      else {
        *(int *)(puVar15 + 2) = iVar11;

        iVar19 = *(int *)(iVar11 + 0x1c);

        *(int *)(puVar15 + 6) = iVar19;

        *(undefined2 *)(puVar15 + 0x14) = *(undefined2 *)(iVar19 + 0x28);

        iVar17 = (byte)puVar1[0x1ca8] - 1;

        if (-1 < iVar17) {
          iVar9 = iVar17 * 0x88 + iVar19;

          do {
            puVar1 = (undefined *)(iVar9 + 0xb8);

            iVar9 = iVar9 + -0x88;

            iVar5 = uVar6 + iVar17;
            iVar17 = iVar17 + -1;
            *(undefined *)(iVar5 + 0x1c) = *puVar1;
          } while (-1 < iVar17);
        }

        *(short *)(puVar15 + 0xe) = (short)((uint)*(undefined4 *)(iVar11 + 0x2d4) >> 8);
        *(short *)(puVar15 + 0x10) = (short)((uint)*(undefined4 *)(iVar11 + 0x2d0) >> 8) + 3;
        *(short *)(puVar15 + 0x12) = (short)((uint)*(undefined4 *)(iVar11 + 0x2dc) >> 8);

        if ((*(ushort *)(puVar15 + 0x14) & 0x2000) == 0) {
          cVar2 = *(char *)(iVar19 + 0x50);
        }
        else {
          cVar2 = *(char *)(iVar19 + 0x51);
        }
        puVar15[1] = cVar2 + '\x01';
      }
      *puVar15 = 0;
      puVar1 = PTR_DAT_8008d2ac;

      iVar4 = iVar4 + 1;

      puVar15 = puVar15 + 0x28;
      uVar21 = uVar6 + 0x28;
    } while (iVar4 < 8);

    *(undefined4 *)(uVar6 + 0x3c) = 0;

    iVar4 = (byte)puVar1[0x1ca8] - 1;

    if (-1 < iVar4) {
      iVar11 = -1;
      r0_00 = (SVECTOR *)(puVar23 + 0x12);
      uVar21 = 1;

      puVar26 = (undefined2 *)(puVar1 + iVar4 * 0x110 + 0x1a0);

      do {
        puVar23[0x90] = *(uint *)(puVar26 + -0x10);

        puVar23[0x8f] = *(uint *)(puVar26 + 0x5e);

        gte_SetGeomOffset((int)((uint)(ushort)puVar26[-0xc] << 0x10) >> 0x11,
                          (int)((uint)(ushort)puVar26[-0xb] << 0x10) >> 0x11);

        gte_ldH(puVar23[0x90]);

        puVar23[0x24] = *(uint *)(puVar26 + 0x22);
        puVar23[0x25] = *(uint *)(puVar26 + 0x24);
        puVar23[0x26] = *(uint *)(puVar26 + 0x26);

        puVar23[0x14] = *(uint *)(puVar26 + -8);
        puVar23[0x15] = *(uint *)(puVar26 + -6);
        puVar23[0x16] = *(uint *)(puVar26 + -4);
        puVar23[0x17] = *(uint *)(puVar26 + -2);
        puVar23[0x18] = *(uint *)(puVar26 + -0);
        gte_SetRotMatrix((MATRIX *)(puVar23 + 0x14));

        puVar12 = puVar23 + 0x29;
        uVar6 = puVar23[0x2e];
        if (uVar6 != 0) {
          puVar22 = puVar23 + 0x2e;
          do {
            if (*(char *)((int)puVar22 + -2) != iVar11) {
              if ((*(ushort *)((int)puVar22 + 0x12) & 0x80) == 0) {
                if ((*(byte *)((int)puVar12 + iVar4 + 0x1c) & 0x40) != 0) {
                  uVar3 = *(ushort *)(puVar22 + 3);
                  *(short *)(puVar23 + 0x27) =
                       (short)((uint)uVar3 - (uint)*(ushort *)(puVar23 + 0x24)) * 4;
                  iVar9 = (int)(((uint)uVar3 - (uint)*(ushort *)(puVar23 + 0x24)) * 0x40000) >> 0x10;

                  iVar17 = (uint)*(ushort *)((int)puVar22 + 0xe) - (uint)*(ushort *)(puVar23 + 0x25);
                  *(short *)((int)puVar23 + 0x9e) = (short)iVar17 * 4;
                  iVar19 = iVar17 * 0x40000 >> 0x10;

                  iVar17 = (int)(((uint)*(ushort *)(puVar22 + 4) - (uint)*(ushort *)(puVar23 + 0x26)
                                 ) * 0x40000) >> 0x10;
                  *(short *)(puVar23 + 0x28) =
                       (short)((uint)*(ushort *)(puVar22 + 4) - (uint)*(ushort *)(puVar23 + 0x26)) *
                       4;

                  uVar7 = (uint)((int)puVar23[0x90] < 0x101);

                  if ((uVar7 == 0) ||
                     ((((iVar9 < 0x1771 && (iVar19 < 0x1771)) &&
                       ((iVar17 < 0x1771 && ((-0x1771 < iVar9 && (-0x1771 < iVar19)))))) &&
                      (-0x1771 < iVar17)))) {
                    gte_ldv0((SVECTOR *)(puVar23 + 0x27));
                    gte_rtv0();
                    iVar5 = gte_stMAC3();
                    iVar5 = iVar5 >> 2;

                    if (-0x35 < iVar5) {
                      if (uVar7 == 0) {
                        uVar7 = 0x2e1f1f1f;
                      }
                      else {
                        if (iVar5 < 0x180) {
                          uVar7 = 0x1f;
                        }
                        else {
                          iVar5 = (0x200 - iVar5) * 0x1f;

                          if (iVar5 < 0) {
                            iVar5 = iVar5 + 0x7f;
                          }

                          uVar7 = iVar5 >> 7;

                          if ((int)uVar7 < 1) goto LAB_8005c0b8;
                        }

                        uVar7 = uVar7 | uVar7 << 8 | 0x2e000000 | uVar7 << 0x10;
                      }
                      puVar25 = puVar12;
                      if (*(char *)((int)puVar22 + -2) == '\0') {
                        iVar17 = 0x100 - (*(int *)(uVar6 + 0x2d8) - *(int *)(uVar6 + 0x2d0) >> 8);

                        if ((iVar17 < 1) || (0x109 < iVar17)) goto LAB_8005bb94;

                        r0 = (MATRIX *)(puVar23 + 0x1c);
                        if (0x100 < iVar17) {
                          iVar17 = 0x100;
                        }
                        iVar19 = (int)*(short *)(uVar6 + 0x2ee);
                        *(int *)(puVar27 + 0x10) = iVar11;
                        *(uint *)(puVar27 + 0x14) = uVar21;
                        *(SVECTOR **)(puVar27 + 0x18) = r0_00;
                        pMVar10 = r0;

                        func_8005f89c(r0, uVar6 + 0x370);

                        gte_SetLightMatrix(r0);

                        uVar6 = 0;

                        iVar20 = iVar17 * 0x29 >> 6;
                        iVar5 = iVar17 * 0x34 >> 6;
                        *(short *)(puVar23 + 0x12) = (short)(iVar17 * 0x28 >> 6);
                        *(undefined2 *)((int)puVar23 + 0x4a) = 0;
                        *(undefined2 *)(puVar23 + 0x13) = 0;
                        r0_00 = *(SVECTOR **)(puVar27 + 0x18);
                        uVar21 = *(uint *)(puVar27 + 0x14);
                        iVar11 = *(int *)(puVar27 + 0x10);
                        puVar25 = puVar12;

                        do {
                          gte_ldv0(r0_00);
                          gte_llv0();
                          if (uVar6 == 0) {
                            *(undefined2 *)(puVar23 + 0x12) = 0;
                            *(short *)(puVar23 + 0x13) = (short)iVar20;
                          }
                          else if (uVar6 == uVar21) {
                            *(short *)(puVar23 + 0x13) = (short)iVar5;
                          }
                          iVar9 = gte_stMAC1();
                          iVar19 = gte_stMAC2();
                          iMac3 = gte_stMAC3();
                          pMVar10 = (MATRIX *)iMac3;
                          *(short *)puVar12 = (short)iVar9;
                          *(short *)((int)puVar12 + 2) = (short)iVar19;
                          *(short *)(puVar12 + 1) = (short)(int)pMVar10;

                          uVar6 = uVar6 + 1;

                          puVar12 = (uint *)((int)puVar12 + 6);

                        } while ((int)uVar6 < 3);

                        gte_SetRotMatrix((MATRIX *)(puVar23 + 0x14));

                        sVar8 = *(short *)(puVar23 + 0x27);
                        sVar14 = *(short *)((int)puVar23 + 0x9e);
                        sVar18 = *(short *)(puVar23 + 0x28);

                        *(char *)((int)puVar22 + -2) = (char)uVar21;
                        iVar9 = (int)sVar8;
                        iVar19 = (int)sVar14;
                        iVar17 = (int)sVar18;
                      }

                      sVar18 = (short)iVar9;
                      *(short *)(puVar23 + 0x83) = sVar18;
                      sVar14 = (short)iVar19;
                      *(short *)((int)puVar23 + 0x20e) = sVar14;
                      sVar8 = (short)iVar17;

                      *(short *)(puVar23 + 0x84) = sVar8;
                      *(short *)(puVar23 + 0x85) =
                           (sVar18 - *(short *)puVar25) - *(short *)((int)puVar22 + -0xe);
                      *(short *)((int)puVar23 + 0x216) =
                           (sVar14 - *(short *)((int)puVar22 + -0x12)) - *(short *)(puVar22 + -3);
                      *(short *)(puVar23 + 0x86) =
                           (sVar8 - *(short *)(puVar22 + -4)) - *(short *)((int)puVar22 + -10);
                      iVar5 = 0;
                      *(short *)(puVar23 + 0x87) = sVar18 - *(short *)((int)puVar22 + -0xe);
                      iVar20 = 0x24;
                      *(short *)((int)puVar23 + 0x21e) = sVar14 - *(short *)(puVar22 + -3);
                      *(short *)(puVar23 + 0x88) = sVar8 - *(short *)((int)puVar22 + -10);
                      puVar24 = puVar23;
                      do {
                        gte_ldv3c((SVECTOR *)(puVar24 + 0x83));
                        gte_rtpt();
                        sVar8 = (short)iVar9;
                        sVar14 = (short)iVar19;
                        sVar18 = (short)iVar17;
                        if (iVar5 == 0) {
                          *(short *)(puVar24 + 0x83) =
                               (*(short *)puVar25 + sVar8) - *(short *)((int)puVar22 + -0xe);
                          *(short *)((int)puVar24 + 0x20e) =
                               (*(short *)((int)puVar22 + -0x12) + sVar14) -
                               *(short *)(puVar22 + -3);
                          *(short *)(puVar24 + 0x84) =
                               (*(short *)(puVar22 + -4) + sVar18) - *(short *)((int)puVar22 + -10);
                          *(short *)(puVar24 + 0x85) = *(short *)puVar25 + sVar8;
                          *(short *)((int)puVar24 + 0x216) =
                               *(short *)((int)puVar22 + -0x12) + sVar14;
                          *(short *)(puVar24 + 0x86) = *(short *)(puVar22 + -4) + sVar18;
                          *(short *)(puVar24 + 0x87) =
                               *(short *)(puVar22 + -2) + *(short *)puVar25 + sVar8;
                          *(short *)((int)puVar24 + 0x21e) =
                               *(short *)((int)puVar22 + -6) +
                               *(short *)((int)puVar22 + -0x12) + sVar14;
                          *(short *)(puVar24 + 0x88) =
                               *(short *)(puVar22 + -1) + *(short *)(puVar22 + -4) + sVar18;
                        }
                        else if (iVar5 == 3) {
                          *(short *)(puVar24 + 0x83) = *(short *)(puVar22 + -2) + sVar8;
                          *(short *)((int)puVar24 + 0x20e) = *(short *)((int)puVar22 + -6) + sVar14;
                          *(short *)(puVar24 + 0x84) = *(short *)(puVar22 + -1) + sVar18;
                          *(short *)(puVar24 + 0x85) =
                               *(short *)(puVar22 + -2) + (sVar8 - *(short *)puVar25);
                          *(short *)((int)puVar24 + 0x216) =
                               *(short *)((int)puVar22 + -6) +
                               (sVar14 - *(short *)((int)puVar22 + -0x12));
                          *(short *)(puVar24 + 0x86) =
                               *(short *)(puVar22 + -1) + (sVar18 - *(short *)(puVar22 + -4));
                          *(short *)(puVar24 + 0x87) = sVar8 - *(short *)puVar25;
                          *(short *)((int)puVar24 + 0x21e) =
                               sVar14 - *(short *)((int)puVar22 + -0x12);
                          *(short *)(puVar24 + 0x88) = sVar18 - *(short *)(puVar22 + -4);
                        }
                        gte_stsxy3c((long *)puVar23);
                        gte_stsz3c((long *)((int)puVar24 + iVar20));
                        iVar20 = iVar20 + 0xc;
                        iVar5 = iVar5 + 3;
                        puVar23 = puVar23 + 3;
                      } while (iVar5 < 9);
                      uVar6 = 0;
                      puVar16 = puVar28 + 8;
                      do {
                        puVar23 = puVar24 + 0x89;
                        if ((uVar6 & 1) != 0) {
                          puVar23 = puVar24 + 0x8c;
                        }
                        puVar16[-7] = uVar7;
                        puVar16[-5] = *puVar23;
                        puVar16[-3] = puVar23[1];
                        *(undefined2 *)(puVar16 + -1) = *(undefined2 *)(puVar23 + 2);
                        *(undefined2 *)(puVar16 + 1) = *(undefined2 *)((int)puVar23 + 10);
                        if (uVar6 == uVar21) {
                          puVar16[-6] = puVar24[8];
                          puVar16[-4] = *puVar24;
                          puVar16[-2] = puVar24[7];
LAB_8005c044:
                          *puVar16 = puVar24[6];
                        }
                        else if ((int)uVar6 < 2) {
                          if (uVar6 == 0) {
                            puVar16[-6] = puVar24[8];
                            puVar16[-4] = *puVar24;
                            puVar16[-2] = puVar24[1];
                            *puVar16 = puVar24[2];
                          }
                        }
                        else if (uVar6 == 2) {
                          puVar16[-6] = puVar24[4];
                          puVar16[-4] = *puVar24;
                          puVar16[-2] = puVar24[3];
                          *puVar16 = puVar24[2];
                        }
                        else if (uVar6 == 3) {
                          puVar16[-6] = puVar24[4];
                          puVar16[-4] = *puVar24;
                          puVar16[-2] = puVar24[5];
                          goto LAB_8005c044;
                        }
                        iVar19 = ((int)puVar24[9] >> 8) + (int)*(char *)((int)puVar22 + -1);
                        iVar17 = iVar19;
                        if (iVar19 < 0) {
                          iVar17 = 0;
                        }
                        if (0x3ff < iVar19) {
                          iVar17 = 0x3ff;
                        }
                        puVar16 = puVar16 + 10;
                        uVar6 = uVar6 + 1;
                        uVar13 = (uint)puVar28 & 0xffffff;
                        puVar23 = (uint *)(puVar24[0x8f] + iVar17 * 4);
                        *puVar28 = *puVar23 | 0x9000000;
                        puVar28 = puVar28 + 10;
                        *puVar23 = uVar13;
                        puVar23 = puVar24;
                        puVar12 = puVar25;
                      } while ((int)uVar6 < 4);
                    }
                  }
                }
              }
              else {
LAB_8005bb94:
                *(char *)((int)puVar22 + -2) = (char)iVar11;
              }
            }
LAB_8005c0b8:
            puVar22 = puVar22 + 10;
            uVar6 = *puVar22;
            puVar12 = puVar12 + 10;
          } while (uVar6 != 0);
        }
        iVar4 = iVar4 + -1;
        puVar26 = puVar26 + -0x88;
      } while (-1 < iVar4);
    }

    *(uint **)(*(int *)(PTR_DAT_8008d2ac + 0x10) + 0x80) = puVar28;
  }
  return;
}
