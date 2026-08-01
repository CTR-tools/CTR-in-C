/*
 * COLL_MOVED_TRIANGL_TestPoint @ 0x8001FC40 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact reference: base score 35765, Branch 0 / Stack 0, Register 221.
 * MOVED (dynamic-geometry) point-in-triangle collision test — the moving sibling of
 * COLL_FIXED_TRIANGL_TestPoint @0x8001EF50 (delivered 19505). Retail is a hand-scheduled
 * GTE routine whose integer register allocation is hand-tuned across the whole function;
 * the GTE cop2 sequence is inline asm (per user: inline asm is fine for GTE code). The
 * large residual is a whole-function register-coloring/scheduling wall a permuter grind
 * cannot close — delivered as a verified-correct behavior-exact reference, consistent with
 * COLL_FIXED_TRIANGL_TestPoint (19505), VehPhysCrash_AnyTwoCars (57535), VehGroundShadow_Main (19850).
 *
 * base.c written from Ghidra (0x8001FC40) + saphi game/COLL.c:1870 (audited CLEAN). All GTE
 * encodings + door gate + divisions opcode-confirmed vs retail. FALSE-FRIEND FIXED: the Ghidra
 * source reuses one local across the gpf-commit stMAC1/stMAC3 reads (rendering hitPos.x as MAC3);
 * the retail asm subtracts each mfc2 before the next load, so x=pos-MAC1 / y=pos-MAC2 / z=pos-MAC3
 * (distinct) — corrected here (saphi already had it right).
 *
 * Behavior (verified vs asm + Ghidra): door/terrain gate quad->flags&0x400 scriptable ->
 * if (terrain & doorAccessFlags) return; rtv0-project (cop2 0x0486012) -> lineDot=MAC2/planeDot=MAC1;
 * if MAC2<0 flip plane normal (unless TriggerScript quad flag 0x40); gpf12 (cop2 0x0198003D)
 * interpolate the near hit point; mvmva (cop2 0x0406012) distance vs shortest; barycentric via
 * COLL_MOVED_TRIANGL_ReorderNormals (func_8001F928); on a nearer in-bounds hit (quad flag 0x10)
 * commit Set1->Set2 + gpf12 hit position; killplane 0x200 / trigger 0x40 set stepFlags @0x1a4.
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
#define gte_ldIR0(r) __asm__ volatile("mtc2 %0, $8" : : "r"(r))
#define gte_ldIR1(r) __asm__ volatile("mtc2 %0, $9" : : "r"(r))
#define gte_ldIR2(r) __asm__ volatile("mtc2 %0, $10" : : "r"(r))
#define gte_ldIR3(r) __asm__ volatile("mtc2 %0, $11" : : "r"(r))
#define gte_gpf12() __asm__ volatile("nop\nnop\ncop2 0x0198003D")

extern int D_8008D728;                                        /* doorAccessFlags */
extern int func_8001F928(int set1, short *v0, short *v1, short *v2);   /* COLL_MOVED_TRIANGL_ReorderNormals */

void COLL_MOVED_TRIANGL_TestPoint(int sps, short *v0, short *v1, short *v2)
{
    short sVar1;
    ushort uVar2;
    int iVar3;
    uint uVar4;
    int uVar5;
    int uVar6;
    int iVar7;
    int iVar8;
    int iVar9;
    int iVar10;
    uint packed;
    int d1;
    int d2;
    int d3;

    sVar1 = v0[3];
    uVar5 = *(int *)(v0 + 6);
    uVar6 = *(int *)(v0 + 8);

    *(short *)(sps + 0x3c) = *(short *)(sps + 0x3c) + 1;
    *(short *)(sps + 0x52) = sVar1;
    *(int *)(sps + 0x54) = uVar5;
    *(int *)(sps + 0x58) = uVar6;

    iVar9 = *(int *)(sps + 0x64);

    if ((*(ushort *)(iVar9 + 0x12) & 0x400) == 0)
    {
        uVar5 = *(int *)(sps);
    }
    else
    {
        uVar5 = *(int *)(sps);
        if (((int)*(char *)(iVar9 + 0x38) & D_8008D728) != 0)
        {
            return;
        }
    }

    gte_ldR11R12(uVar5);
    packed = *(int *)(sps + 0x10) << 0x10 | (uint)*(ushort *)(sps + 4);
    gte_ldR13R21(packed);
    packed = (uint)*(int *)(sps + 0x10) >> 0x10 | (uint)*(ushort *)(sps + 0x14) << 0x10;
    gte_ldR22R23(packed);
    gte_ldVXY0(*(int *)(sps + 0x54));
    gte_ldVZ0(*(int *)(sps + 0x58));
    gte_mvmva_core(0x0486012);
    gte_stMAC1(iVar7);
    gte_stMAC2(iVar8);

    iVar7 = iVar7 + (int)*(short *)(sps + 0x5a) * -2;
    iVar8 = iVar8 + (int)*(short *)(sps + 0x5a) * -2;

    if (iVar8 < 0)
    {
        if ((*(ushort *)(iVar9 + 0x12) & 0x40) == 0)
        {
            sVar1 = *(short *)(sps + 0x54);
            if (-1 < *(int *)(iVar9 + 0x14)) goto LAB_8001fd38;
        }
        else
        {
            sVar1 = *(short *)(sps + 0x54);
        }

        iVar7 = -iVar7;
        iVar8 = -iVar8;

        *(short *)(sps + 0x54) = -sVar1;
        *(short *)(sps + 0x56) = -*(short *)(sps + 0x56);
        *(short *)(sps + 0x58) = -*(short *)(sps + 0x58);
        *(short *)(sps + 0x5a) = -*(short *)(sps + 0x5a);
    }

LAB_8001fd38:

    uVar2 = *(ushort *)(iVar9 + 0x12);

    *(short *)(sps + 0x3c) = *(short *)(sps + 0x3c) + 1;
    if (-1 < iVar7 - *(short *)(sps + 6))
    {
        return;
    }
    if (iVar8 < 0)
    {
        return;
    }

    if (((uVar2 & 0x40) == 0) && (0 < iVar7 - iVar8))
    {
        return;
    }

    if (-1 < iVar7)
    {
        gte_ldIR0(iVar7);
        gte_ldIR1((int)*(short *)(sps + 0x54));
        gte_ldIR2((int)*(short *)(sps + 0x56));
        gte_ldIR3((int)*(short *)(sps + 0x58));
    }
    else
    {
        d1 = (int)*(short *)(sps) - (int)*(short *)(sps + 0x10);
        gte_ldIR1(d1);
        d2 = (int)*(short *)(sps + 2) - (int)*(short *)(sps + 0x12);
        gte_ldIR2(d2);
        d3 = (int)*(short *)(sps + 4) - (int)*(short *)(sps + 0x14);
        gte_ldIR3(d3);
        d1 = (iVar7 * -0x1000) / (iVar8 - iVar7);
        gte_ldIR0(d1);
    }

    gte_gpf12();
    gte_stMAC1(uVar5);
    *(short *)(sps + 0x5c) = *(short *)(sps) - (short)uVar5;
    gte_stMAC2(uVar6);
    *(short *)(sps + 0x5e) = *(short *)(sps + 2) - (short)uVar6;
    gte_stMAC3(uVar5);
    *(short *)(sps + 0x60) = *(short *)(sps + 4) - (short)uVar5;

    *(int *)(sps + 0xd8) = (int)v0;
    *(int *)(sps + 0xdc) = (int)v1;
    *(int *)(sps + 0xe0) = (int)v2;

    iVar9 = func_8001F928(sps + 0x4c, v0, v1, v2);

    if (iVar9 < 0)
    {
        return;
    }

    if (-1 < iVar7)
    {
        *(short *)(sps + 0xe4) = *(short *)(sps) - *(short *)(sps + 0x4c);
        *(short *)(sps + 0xe6) = *(short *)(sps + 2) - *(short *)(sps + 0x4e);
        *(short *)(sps + 0xe8) = *(short *)(sps + 4) - *(short *)(sps + 0x50);
    }
    else
    {
        *(short *)(sps + 0xe4) = *(short *)(sps + 0x5c) - *(short *)(sps + 0x4c);
        *(short *)(sps + 0xe6) = *(short *)(sps + 0x5e) - *(short *)(sps + 0x4e);
        *(short *)(sps + 0xe8) = *(short *)(sps + 0x60) - *(short *)(sps + 0x50);
    }

    gte_ldR11R12(*(int *)(sps + 0xe4));
    gte_ldR13R21((int)*(short *)(sps + 0xe8));
    gte_ldVXY0(*(int *)(sps + 0xe4));
    gte_ldVZ0(*(int *)(sps + 0xe8));
    gte_mvmva_core(0x0406012);
    gte_stMAC1(iVar3);

    iVar10 = *(int *)(sps + 0x64);
    uVar2 = *(ushort *)(iVar10 + 0x12);

    if (0 < iVar3 - *(int *)(sps + 8))
    {
        return;
    }

    if ((uVar2 & 0x40) == 0)
    {
LAB_8001ff14:
        iVar8 = iVar8 - iVar7;
        if (iVar8 != 0)
        {
            iVar8 = 0x1000 - ((*(short *)(sps + 6) - iVar7) * 0x1000) / iVar8;
        }

        if (iVar8 - *(int *)(sps + 0x84) < 0)
        {
            if ((uVar2 & 0x10) == 0)
            {
                *(int *)(sps + 0x84) = iVar8;

                uVar5 = *(int *)(*(int *)(sps + 0xdc) + 8);
                uVar6 = *(int *)(*(int *)(sps + 0xe0) + 8);
                *(int *)(sps + 0xcc) = *(int *)(*(int *)(sps + 0xd8) + 8);
                *(int *)(sps + 0xd0) = uVar5;
                *(int *)(sps + 0xd4) = uVar6;

                *(int *)(sps + 0x68) = *(int *)(sps + 0x4c);
                *(int *)(sps + 0x6c) = *(int *)(sps + 0x50);
                *(int *)(sps + 0x70) = *(int *)(sps + 0x54);
                *(int *)(sps + 0x74) = *(int *)(sps + 0x58);
                *(int *)(sps + 0x78) = *(int *)(sps + 0x5c);
                *(int *)(sps + 0x7c) = *(int *)(sps + 0x60);
                *(int *)(sps + 0x80) = iVar10;

                *(byte *)(sps + 0x7f) = *(byte *)(sps + 0x63);

                *(char *)(sps + 0x7e) = (char)iVar9;
                if (iVar8 < 1)
                {
                    *(int *)(sps + 0x1c) = *(int *)(sps + 0x10);
                    *(short *)(sps + 0x20) = *(short *)(sps + 0x14);
                }
                else
                {
                    gte_ldIR0(iVar8);
                    d1 = (int)*(short *)(sps) - (int)*(short *)(sps + 0x10);
                    gte_ldIR1(d1);
                    d2 = (int)*(short *)(sps + 2) - (int)*(short *)(sps + 0x12);
                    gte_ldIR2(d2);
                    d3 = (int)*(short *)(sps + 4) - (int)*(short *)(sps + 0x14);
                    gte_ldIR3(d3);
                    gte_gpf12();
                    gte_stMAC1(uVar5);
                    *(short *)(sps + 0x1c) = *(short *)(sps + 0x10) + (short)uVar5;
                    gte_stMAC2(uVar6);
                    *(short *)(sps + 0x1e) = *(short *)(sps + 0x12) + (short)uVar6;
                    gte_stMAC3(uVar5);
                    *(short *)(sps + 0x20) = *(short *)(sps + 0x14) + (short)uVar5;
                }

                *(short *)(sps + 0x3e) = *(short *)(sps + 0x3e) + 1;
            }
            else
            {
                if ((uVar2 & 0x200) != 0)
                {
                    *(int *)(sps + 0x1a4) = *(int *)(sps + 0x1a4) | 0x4000;
                }
            }
        }
    }
    else
    {
        if (iVar7 < 0)
        {
            uVar4 = *(int *)(sps + 0x1a4);
        }
        else
        {
            uVar4 = *(int *)(sps + 0x1a4);

            if (-1 < (iVar7 - *(short *)(sps + 6) | iVar8 - *(short *)(sps + 6)))
                goto LAB_8001ff14;
        }

        *(int *)(sps + 0x1a4) = uVar4 | *(byte *)(iVar10 + 0x38);
    }
    return;
}
