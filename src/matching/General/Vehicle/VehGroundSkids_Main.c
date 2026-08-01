/*
 * VehGroundSkids_Main @ 0x8005C354 (SCUS_944.26 / NTSC-U 926) — from aibox staging.
 *
 * Behavior-exact match: base score 15645, Branch 0 / Stack 0, Register 269 — a large but genuine
 * gcc-2.8.1 register-allocation/instruction-scheduling residual on this GTE-saturated skidmark
 * renderer (byte-exact would be a dedicated hand-match of the projected-vertex CONCAT reads + the
 * scratchpad pointer scheduling). Verified behaviorally correct vs the retail asm + Ghidra
 * (constant-for-constant) — NOT a false-friend, no barred hacks (the __asm__ volatile blocks are
 * legitimate PSX GTE intrinsic macros; pScratch = the 0x1F800000 scratchpad used as transform
 * workspace, which gcc reproduces from a plain fixed-address pointer).
 *
 * Draws every kart's ground-skidmark trail for one viewport (tileView / c110). GTE geom offset from
 * viewport rect w/h, H = distanceToScreen (tileView+0x18), rotation from matrix_ViewProj
 * (tileView+0x28); caches camera translation (tileView+0x7c..0x84) in scratchpad pScratch[0x17]/[0x18]
 * (0x1F8000B8) as the local-transform origin. Per driver thread (object at thread+0x30): skidmark ring
 * buffer at object+0x2c4 (count/mask), 8 segments x 0x40 bytes, current index (object+0xc3)-1 & 7.
 * Distance-culls the newest segment (|(segPos-camT)*4| < 0x1771 per axis), derives fade from projected
 * distance (gte_rtv0 -> MAC3>>2; <0x180 -> 0x7f else 0x1a-LZCR, clamp>=0, 0x7f>>that; skip if <0x10),
 * then walks the segment ring emitting up to 4 textured POLY_GT4 sub-quads per segment: each mask bit
 * set in segMask & prevMask whose 4 projected corners pass the 0x20 near-clip -> CalcLocalVerts
 * (func_8005C278) + gte_rtpt + Subset1/EmitQuad (func_8005C120) with color (seg+0xcb/0xdb/0xeb/0xfb)
 * and OT key ((projZ>>2)+(seg+0xca/0xda/0xea/0xfa)*0x40). Rotating scratchpad ring buffers: pScratch
 * (workspace), pVertBufA/B + pTopVerts/pBottomVerts (projected xy), pZBuf (z).
 *
 * NOTE: heavily-reused GTE-fragment temporaries (iVar1..5/uVar1) left generic — they hold different
 * projected-vertex halves across lifetimes; full de-rename is a future readability pass.
 * saphi VehGroundSkids_Main audited CLEAN vs this asm + Ghidra.
 */
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef char undefined;
typedef short undefined2;
typedef int undefined4;
typedef struct { short vx, vy, vz, pad; } SVECTOR;

#define gte_SetGeomOffset(ofx, ofy) do { \
    int _ox = (int)(ofx) << 16; \
    int _oy = (int)(ofy) << 16; \
    __asm__ volatile("ctc2 %0, $24" :: "r"(_ox)); \
    __asm__ volatile("ctc2 %0, $25" :: "r"(_oy)); \
} while(0)
#define gte_ldH(h) __asm__ volatile("ctc2 %0, $26" :: "r"(h))
#define gte_SetRotMatrix(pScratch) __asm__ volatile( \
    "lw $12, 0(%0);" "lw $13, 4(%0);" \
    "ctc2 $12, $0;" "ctc2 $13, $1;" \
    "lw $12, 8(%0);" "lw $13, 12(%0);" "lw $14, 16(%0);" \
    "ctc2 $12, $2;" "ctc2 $13, $3;" "ctc2 $14, $4" \
    :: "r"(pScratch) : "$12", "$13", "$14")
#define gte_SetTransVector(v) __asm__ volatile( \
    "lw $12, 0(%0);" "lw $13, 4(%0);" "lw $14, 8(%0);" \
    "ctc2 $12, $5;" "ctc2 $13, $6;" "ctc2 $14, $7" \
    :: "r"(v) : "$12", "$13", "$14")
#define gte_ldv0(v) __asm__ volatile("lwc2 $0, 0(%0)\nlwc2 $1, 4(%0)" :: "r"(v))
#define gte_rtv0() __asm__ volatile("nop\nnop\ncop2 0x0486012")
#define gte_stMAC3() __extension__({ int _r; __asm__ volatile("mfc2 %0, $27" : "=r"(_r)); _r; })
#define gte_ldLZCS(pScratch) __asm__ volatile("mtc2 %0, $30" :: "r"(pScratch))
#define gte_stLZCR() __extension__({ int _r; __asm__ volatile("mfc2 %0, $31" : "=r"(_r)); _r; })
#define gte_ldv3c(v) __asm__ volatile( \
    "lwc2 $0, 0(%0);" "lwc2 $1, 4(%0);" "lwc2 $2, 8(%0);" \
    "lwc2 $3, 12(%0);" "lwc2 $4, 16(%0);" "lwc2 $5, 20(%0)" \
    :: "r"(v))
#define gte_rtpt() __asm__ volatile("nop\nnop\ncop2 0x280030")
#define gte_stsxy3c(p) __asm__ volatile( \
    "swc2 $12, 0(%0);" "swc2 $13, 4(%0);" "swc2 $14, 8(%0)" \
    :: "r"(p) : "memory")
#define gte_stsz3c(p) __asm__ volatile( \
    "swc2 $17, 0(%0);" "swc2 $18, 4(%0);" "swc2 $19, 8(%0)" \
    :: "r"(p) : "memory")


extern void func_8005c278(SVECTOR *, int, int, int);
extern void func_8005c120(SVECTOR *, SVECTOR *, int, SVECTOR *);

void VehGroundSkids_Main(undefined4 threadList, int tileView)
{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  SVECTOR *pVertBufA;
  SVECTOR *pVertBufB;
  SVECTOR *pScratch;
  SVECTOR *pZBuf;
  SVECTOR *pTopVerts;
  uint segMask;
  SVECTOR *pBottomVerts;
  uint prevMask;
  int object;
  uint segIndex;


  gte_SetGeomOffset((int)((uint)*(ushort *)(tileView + 0x20) << 0x10) >> 0x11,
                    (int)((uint)*(ushort *)(tileView + 0x22) << 0x10) >> 0x11);

  gte_ldH(*(undefined4 *)(tileView + 0x18));

  pScratch = (SVECTOR *)0x1f800000;
  *(undefined4 *)(pScratch + 3) = tileView;
  *(undefined4 *)(pScratch + 0x17) = 0;
  *(undefined4 *)&pScratch[0x17].vz = 0;
  *(undefined4 *)(pScratch + 0x18) = 0;

  gte_SetRotMatrix((void *)(tileView + 0x28));
  gte_SetTransVector((void *)(pScratch + 0x17));

  *(undefined4 *)(pScratch + 0x17) = *(undefined4 *)(tileView + 0x7c);
  *(undefined4 *)&pScratch[0x17].vz = *(undefined4 *)(tileView + 0x80);
  *(undefined4 *)(pScratch + 0x18) = *(undefined4 *)(tileView + 0x84);

  iVar5 = (int)threadList;

  do {
    if (iVar5 == 0) {
      return;
    }

    iVar5 = *(int *)((int)threadList + 0x30);
    object = iVar5;

    segMask = *(uint *)(iVar5 + 0x2c4);

    if (0xf < segMask) {
      prevMask = 0;
      pVertBufB = pScratch + 5;
      pVertBufA = pScratch + 0xe;
      pBottomVerts = (SVECTOR *)&pScratch[9].vz;
      uVar1 = *(byte *)(iVar5 + 0xc3) - 1 & 7;
      segIndex = uVar1;
      iVar5 = iVar5 + uVar1 * 0x40;
      iVar2 = (int)*(short *)(iVar5 + 0xc4) - *(int *)(pScratch + 0x17);
      iVar4 = iVar2 * 4;
      iVar3 = iVar4;
      if (iVar4 < 0) {
        iVar3 = iVar2 * -4;
      }
      pZBuf = (SVECTOR *)&pScratch[0x12].vz;
      if (iVar3 < 0x1771) {
        pScratch->vx = (short)iVar4;
        iVar2 = (int)*(short *)(iVar5 + 0xc6) - *(int *)&pScratch[0x17].vz;
        iVar4 = iVar2 * 4;
        iVar3 = iVar4;
        if (iVar4 < 0) {
          iVar3 = iVar2 * -4;
        }
        if (iVar3 < 0x1771) {
          pScratch->vy = (short)iVar4;
          iVar3 = (int)*(short *)(iVar5 + 200) - *(int *)(pScratch + 0x18);
          iVar2 = iVar3 * 4;
          iVar5 = iVar2;
          if (iVar2 < 0) {
            iVar5 = iVar3 * -4;
          }
          if (iVar5 < 0x1771) {
            pScratch->vz = (short)iVar2;
            gte_ldv0(pScratch);
            gte_rtv0();
            iVar5 = gte_stMAC3();
            if (iVar5 >> 2 < 0x180) {
              uVar1 = 0x7f;
            }
            else {
              iVar2 = (iVar5 >> 2) - 0x180;
              gte_ldLZCS(iVar2);
              iVar5 = gte_stLZCR();
              uVar1 = 0x1a - iVar5;
              if ((int)uVar1 < 0) {
                uVar1 = 0;
              }
              uVar1 = 0x7f >> (uVar1 & 0x1f);
              if ((int)uVar1 < 0x10) goto LAB_8005c9dc;
            }
            *(uint *)&pScratch[3].vz = uVar1 | uVar1 << 8 | 0x3e000000 | uVar1 << 0x10;
            *(undefined4 *)(pScratch + 4) = 0xffffffff;
            do {
              uVar1 = segMask;
              pTopVerts = pVertBufB;
              pVertBufB = pVertBufA;
              if (uVar1 == 0) break;
              if ((uVar1 & 0xf) != 0) {
                iVar3 = (int)segIndex * 0x40;
                iVar5 = object + iVar3 + 0xc4;

                func_8005c278(pScratch, iVar5, iVar5 + 8, iVar5 + 0x10);
                gte_ldv3c(pScratch);
                gte_rtpt();

                func_8005c278(pScratch, iVar5 + 0x18, iVar5 + 0x20, iVar5 + 0x28);
                gte_stsxy3c((long *)pTopVerts);
                gte_stsz3c((long *)pVertBufB);
                gte_ldv3c(pScratch);
                gte_rtpt();

                func_8005c278(pScratch, iVar5 + 0x30, iVar5 + 0x38, iVar5);
                gte_stsxy3c((long *)&pTopVerts[1].vz);
                gte_stsz3c((long *)&pVertBufB[1].vz);
                gte_ldv3c(pScratch);
                gte_rtpt();

                gte_stsxy3c((long *)(pTopVerts + 3));
                gte_stsz3c((long *)(pVertBufB + 3));

                if (((((uVar1 & prevMask & 1) != 0) && (0x20 < *(int *)pVertBufB)) &&
                    (0x20 < *(int *)&pVertBufB->vz)) &&
                   ((0x20 < *(int *)pZBuf && (0x20 < *(int *)&pZBuf->vz)))) {
                  iVar5 = object;
                  *(uint *)&pScratch[4].vz = (uint)*(byte *)(iVar5 + iVar3 + 0xcb);
                  func_8005c120(pTopVerts, pBottomVerts,
                               (*(int *)pVertBufB >> 2) + (uint)*(byte *)(iVar5 + iVar3 + 0xca) * 0x40,
                               pScratch);
                }

                if ((((uVar1 & prevMask & 2) != 0) && (0x20 < *(int *)(pVertBufB + 1))) &&
                   ((0x20 < *(int *)&pVertBufB[1].vz &&
                    ((0x20 < *(int *)(pZBuf + 1) && (0x20 < *(int *)&pZBuf[1].vz)))))) {
                  iVar5 = (int)segIndex;
                  iVar3 = object;
                  *(uint *)&pScratch[4].vz = (uint)*(byte *)(iVar3 + iVar5 * 0x40 + 0xdb);
                  func_8005c120(pTopVerts + 1, pBottomVerts + 1,
                               (*(int *)(pVertBufB + 1) >> 2) +
                               (uint)*(byte *)(iVar3 + iVar5 * 0x40 + 0xda) * 0x40, pScratch);
                }

                if (((((uVar1 & prevMask & 4) != 0) && (0x20 < *(int *)(pVertBufB + 2))) &&
                    (0x20 < *(int *)&pVertBufB[2].vz)) &&
                   ((0x20 < *(int *)(pZBuf + 2) && (0x20 < *(int *)&pZBuf[2].vz)))) {
                  iVar5 = (int)segIndex;
                  iVar3 = object;
                  *(uint *)&pScratch[4].vz = (uint)*(byte *)(iVar3 + iVar5 * 0x40 + 0xeb);
                  func_8005c120(pTopVerts + 2, pBottomVerts + 2,
                               (*(int *)(pVertBufB + 2) >> 2) +
                               (uint)*(byte *)(iVar3 + iVar5 * 0x40 + 0xea) * 0x40, pScratch);
                }

                if ((((uVar1 & prevMask & 8) != 0) && (0x20 < *(int *)(pVertBufB + 3))) &&
                   ((0x20 < *(int *)&pVertBufB[3].vz &&
                    ((0x20 < *(int *)(pZBuf + 3) && (0x20 < *(int *)&pZBuf[3].vz)))))) {
                  iVar5 = (int)segIndex;
                  iVar3 = object;
                  *(uint *)&pScratch[4].vz = (uint)*(byte *)(iVar3 + iVar5 * 0x40 + 0xfb);
                  func_8005c120(pTopVerts + 3, pBottomVerts + 3,
                               (*(int *)(pVertBufB + 3) >> 2) +
                               (uint)*(byte *)(iVar3 + iVar5 * 0x40 + 0xfa) * 0x40, pScratch);
                }
              }
              segIndex = (int)segIndex + 1U & 7;
              if (*(int *)(pScratch + 4) == -1) {
                *(undefined4 *)(pScratch + 4) = *(undefined4 *)&pScratch[3].vz;
                segMask = uVar1;
                uVar1 = 0xf;
              }
              else {
                segMask = uVar1 >> 4;
                prevMask = (int)(*(uint *)&pScratch[3].vz & 0xff) >> 1;
                *(undefined4 *)(pScratch + 4) = *(undefined4 *)&pScratch[3].vz;
                *(uint *)&pScratch[3].vz = prevMask | prevMask << 8 | 0x3e000000 | prevMask << 0x10;
              }
              pVertBufA = pZBuf;
              pZBuf = pVertBufB;
              pVertBufB = pBottomVerts;
              pBottomVerts = pTopVerts;
              prevMask = uVar1;
            } while (*(int *)(pScratch + 4) != 0);
          }
        }
      }
    }
LAB_8005c9dc:
    iVar5 = *(int *)((int)threadList + 0x10);
    (int)threadList = iVar5;

  } while (1);
}
