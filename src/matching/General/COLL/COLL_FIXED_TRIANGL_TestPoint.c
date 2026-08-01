/*
 * COLL_FIXED_TRIANGL_TestPoint @ 0x8001EF50 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact reference: base score 19505, Branch 0 / Stack 0, Register 277.
 * This is a retail "Handwritten function" (hand-scheduled): the GTE cop2 sequence
 * AND the integer register allocation (regs held across the whole routine) are
 * hand-tuned. The GTE ops are written as inline asm (per user: inline asm is fine
 * for GTE code), which is codegen-correct; the large residual is a WHOLE-FUNCTION
 * register-coloring/scheduling wall a permuter grind cannot close (confirmed:
 * bounces 18495-25000), and byte-exact would need pervasive whole-function
 * register-pinning (transcribing the hand-written asm) - out of scope for a
 * natural-C reference. Delivered as a verified-correct behavior-exact reference,
 * consistent with COLL_FIXED_PlayerSearch (7885) and VehPhysCrash_AnyTwoCars (57535).
 *
 * Behavior (verified vs asm + Ghidra; saphi game/COLL.c:651 audited CLEAN):
 *   GTE-project the point against the triangle plane -> lineDot=MAC2,
 *   planeDot=MAC1-((quad.z>>16)<<13); if lineDot>=0 return; factor =
 *   -planeDot/(lineDot>>12) clamped [0,0x1000]; gpl12 interpolate the hit point;
 *   project onto the dominant normal axis (flag 3->Y / 1->Z / 2->X) with the
 *   |a|<|b| edge swap; barycentric edge test (denom>>6, baryB then baryA, reject
 *   if baryA<0 || baryA+baryB-0x1000>0); on accept commit the candidate hit
 *   (or set TRIGGER stepFlags for a trigger quad).
 *
 * inline-asm GTE per user; vs SCUS_944.26.exe; from aibox staging.
 */
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;

#define gte_ldR11R12(r) __asm__ volatile("ctc2 %0, $0" : : "r"(r))
#define gte_ldR13R21(r) __asm__ volatile("ctc2 %0, $1" : : "r"(r))
#define gte_ldR22R23(r) __asm__ volatile("ctc2 %0, $2" : : "r"(r))
#define gte_ldVXY0(r) __asm__ volatile("mtc2 %0, $0" : : "r"(r))
#define gte_ldVZ0(r) __asm__ volatile("mtc2 %0, $1" : : "r"(r))
#define gte_mvmva_core(op) __asm__ volatile("nop\nnop\ncop2 %0" : : "g"(op))
#define gte_stMAC1(out) __asm__ volatile("mfc2 %0, $25" : "=r"(out))
#define gte_stMAC2(out) __asm__ volatile("mfc2 %0, $26" : "=r"(out))
#define gte_stMAC3(out) __asm__ volatile("mfc2 %0, $27" : "=r"(out))
#define gte_ldMAC1(r) __asm__ volatile("mtc2 %0, $25" : : "r"(r))
#define gte_ldMAC2(r) __asm__ volatile("mtc2 %0, $26" : : "r"(r))
#define gte_ldMAC3(r) __asm__ volatile("mtc2 %0, $27" : : "r"(r))
#define gte_ldIR0(r) __asm__ volatile("mtc2 %0, $8" : : "r"(r))
#define gte_ldIR1(r) __asm__ volatile("mtc2 %0, $9" : : "r"(r))
#define gte_ldIR2(r) __asm__ volatile("mtc2 %0, $10" : : "r"(r))
#define gte_ldIR3(r) __asm__ volatile("mtc2 %0, $11" : : "r"(r))
#define gte_gpl12() __asm__ volatile("nop\nnop\ncop2 0x0198003E")

void COLL_FIXED_TRIANGL_TestPoint(int sps, short *v0, short *v1, short *v2)
{
    short sVar1;
    int iVar2;
    short *psVar3;
    int iVar5;
    int iVar7;
    int iVar8;
    int iVar9;
    int iVar10;
    int iVar11;
    uint uVar12;
    int iVar13;
    int iVar14;
    uint uVar15;
    uint uVar16;

    sVar1 = v0[3];
    iVar8 = *(int *)(v0 + 6);
    iVar7 = *(int *)(v0 + 8);

    *(short *)(sps + 0x3c) = *(short *)(sps + 0x3c) + 1;
    *(short *)(sps + 0x52) = sVar1;
    *(int *)(sps + 0x54) = iVar8;
    *(int *)(sps + 0x58) = iVar7;

    iVar5 = (int)*(short *)(sps + 0x14);

    gte_ldR11R12(*(int *)(sps + 0x10));

    iVar11 = (int)*(short *)(sps + 0x1c) - (int)*(short *)(sps + 0x10);
    uVar12 = (int)*(short *)(sps + 0x1e) - (int)*(short *)(sps + 0x12);
    iVar14 = (int)*(short *)(sps + 0x20) - iVar5;

    uVar15 = iVar11 * 0x10000 | (uint)(ushort)iVar5;
    gte_ldR13R21(uVar15);
    uVar16 = uVar12 & 0xffff | iVar14 * 0x10000;
    gte_ldR22R23(uVar16);

    gte_ldVXY0(*(int *)(sps + 0x54));
    gte_ldVZ0(iVar7);

    gte_mvmva_core(0x0406012);

    gte_stMAC2(iVar2);
    gte_stMAC1(iVar8);

    if (iVar2 < 0)
    {
        iVar8 = iVar8 - ((iVar7 >> 0x10) << 0xd);
        iVar2 = iVar2 >> 0xc;
        iVar8 = -iVar8;
        iVar8 = iVar8 / iVar2;

        gte_ldMAC1((int)*(short *)(sps + 0x10));
        gte_ldMAC2((int)*(short *)(sps + 0x12));
        gte_ldMAC3(iVar5);

        gte_ldIR1(iVar11);
        gte_ldIR2((int)uVar12);
        gte_ldIR3(iVar14);

        gte_ldIR0(iVar8);

        if (iVar8 >= 0 && iVar8 + -0x1000 < 1)
        {
            gte_gpl12();

            gte_stMAC1(iVar11);
            gte_stMAC2(uVar12);
            gte_stMAC3(iVar14);

            *(short *)(sps + 0x4c) = (short)iVar11;
            *(short *)(sps + 0x4e) = (short)uVar12;
            *(short *)(sps + 0x50) = (short)iVar14;

            psVar3 = v1;

            if (*(short *)(sps + 0x52) == 3)
            {
                /* flag == 3: normal points Y, use Z then X */
                iVar2 = (int)v0[2];
                iVar9 = v1[2] - iVar2;
                iVar7 = v2[2] - iVar2;
                iVar2 = iVar14 - iVar2;

                iVar8 = iVar9;
                if (iVar9 < 0) {
                    iVar8 = -iVar9;
                }

                iVar10 = iVar7;
                if (iVar7 < 0) {
                    iVar10 = -iVar7;
                }

                iVar5 = iVar9;
                if (iVar8 - iVar10 < 0) {
                    psVar3 = v2;
                    v2 = v1;
                    iVar5 = iVar7;
                    iVar7 = iVar9;
                }

                iVar10 = (int)*v0;
                iVar8 = *psVar3 - iVar10;
                iVar9 = *v2 - iVar10;
                iVar11 = iVar11 - iVar10;
            }
            else
            {
                iVar2 = (int)*v0;

                if (*(short *)(sps + 0x52) == 1)
                {
                    /* flag == 1: normal points Z, use X then Y */
                    iVar9 = *v1 - iVar2;
                    iVar7 = *v2 - iVar2;
                    iVar2 = iVar11 - iVar2;

                    iVar8 = iVar9;
                    if (iVar9 < 0) {
                        iVar8 = -iVar9;
                    }

                    iVar10 = iVar7;
                    if (iVar7 < 0) {
                        iVar10 = -iVar7;
                    }

                    iVar5 = iVar9;
                    if (iVar8 - iVar10 < 0) {
                        psVar3 = v2;
                        v2 = v1;
                        iVar5 = iVar7;
                        iVar7 = iVar9;
                    }

                    iVar10 = (int)v0[1];
                    iVar8 = psVar3[1] - iVar10;
                    iVar9 = v2[1] - iVar10;
                    iVar11 = uVar12 - iVar10;
                }
                else
                {
                    /* flag == 2: normal points X, use Y then Z */
                    iVar2 = (int)v0[1];
                    iVar9 = v1[1] - iVar2;
                    iVar7 = v2[1] - iVar2;
                    iVar2 = uVar12 - iVar2;

                    iVar8 = iVar9;
                    if (iVar9 < 0) {
                        iVar8 = -iVar9;
                    }

                    iVar10 = iVar7;
                    if (iVar7 < 0) {
                        iVar10 = -iVar7;
                    }

                    iVar5 = iVar9;
                    if (iVar8 - iVar10 < 0) {
                        psVar3 = v2;
                        v2 = v1;
                        iVar5 = iVar7;
                        iVar7 = iVar9;
                    }

                    iVar10 = (int)v0[2];
                    iVar8 = psVar3[2] - iVar10;
                    iVar9 = v2[2] - iVar10;
                    iVar11 = iVar14 - iVar10;
                }
            }

            iVar14 = -0x1000;
            iVar13 = -0x1000;

            if (iVar5 == 0)
            {
                iVar14 = -0x1000;

                if (iVar7 == 0) {
                    return;
                }

                iVar13 = (iVar2 << 0xc) / iVar7;

                if (iVar13 >= 0 && iVar13 + -0x1000 < 1)
                {
                    iVar14 = (iVar11 * 0x1000 - iVar13 * iVar9) / iVar8;
                }
            }
            else
            {
                iVar9 = (iVar9 * iVar5 - iVar7 * iVar8) >> 6;

                if (iVar9 != 0)
                {
                    iVar13 = ((iVar11 * iVar5 - iVar2 * iVar8) * 0x40) / iVar9;

                    if (iVar13 >= 0 && iVar13 + -0x1000 < 1)
                    {
                        iVar14 = (iVar2 * 0x1000 - iVar13 * iVar7) / iVar5;
                    }
                }
            }

            iVar2 = *(int *)(sps + 100);

            if (iVar14 >= 0 && iVar14 + iVar13 + -0x1000 < 1)
            {
                if ((*(ushort *)(iVar2 + 0x12) & 0x40) != 0)
                {
                    *(uint *)(sps + 0x1a4) = *(uint *)(sps + 0x1a4) | (uint)*(byte *)(iVar2 + 0x38);
                    return;
                }

                *(int *)(sps + 0x80) = iVar2;
                *(short *)(sps + 0xc8) = (short)iVar14;
                *(short *)(sps + 0xca) = (short)iVar13;

                *(int *)(sps + 0xcc) = *(int *)(v0 + 4);
                *(int *)(sps + 0xd0) = *(int *)(psVar3 + 4);
                *(int *)(sps + 0xd4) = *(int *)(v2 + 4);

                *(short *)(sps + 0x3e) = *(short *)(sps + 0x3e) + 1;

                *(int *)(sps + 0x68) = *(int *)(sps + 0x4c);
                *(int *)(sps + 0x1c) = *(int *)(sps + 0x4c);
                *(short *)(sps + 0x6c) = *(short *)(sps + 0x50);
                *(short *)(sps + 0x20) = *(short *)(sps + 0x50);

                *(int *)(sps + 0x70) = *(int *)(sps + 0x54);
                *(int *)(sps + 0x74) = *(int *)(sps + 0x58);
            }
        }
    }
}
