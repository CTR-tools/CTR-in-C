/*
 * VehStuckProc_Warp_AddDustPuff2 @ 0x80068644 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact (Branch 0 / Stack 0) but a LARGE residual — score 35170, Register 238
 * + many instruction insert/delete diffs. This is an early-stage WIP match of a big GTE +
 * scratchpad particle-builder (~567 asm insns): it perspective-transforms (gte_rtpt) a ring
 * of "warp dust puff" vertices per the 6 rings, using the PS1 scratchpad (0x1F8001xx) as the
 * GTE working set, then writes POLY prims (0xE1000A20 / 0x3A000000 header words, 0x7F1F3F
 * colours) into the player's pushBuffer and links them into the OT. Reaching byte-exact needs
 * dedicated hand-matching (permuter alone can't bridge the insert/deletes); NOT byte-close.
 *
 * FIXES applied to the aibox seed:
 *   - permuter (pycparser) could not parse `+` inside GTE inline-asm operands; hoisted
 *     `puVar20 + 0x28` and `puVar16 + 3` into matrixPtr/szPtr temps (codegen-neutral).
 *   - BEHAVIORAL BUG in the seed (Ghidra `extraout_var` artifact): the visible-branch endpoint
 *     update added the FULL MATH_Sin/Cos value; retail asm @80068A08 does `sra $v0,$v0,8; addu`
 *     = endpoint.vx/vz += (Sin/Cos >> 8). Corrected here to match retail.
 *
 * saphi game/Vehicle/VehStuckProc.c:1355 VehStuckProc_Warp_AddDustPuff2 audited CLEAN vs this
 * asm + Ghidra — it correctly uses AddHalf(endpoint, Sin>>8); the >>8-drop bug was only in the
 * matching seed, never in saphi. Generic Ghidra locals left as-is (large fn; full de-rename is a
 * future pass). vs SCUS_944.26.exe; from aibox staging.
 */
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef char undefined;
typedef short undefined2;
typedef int undefined4;

#define CONCAT22(a, b) (((unsigned int)(unsigned short)(a) << 16) | (unsigned short)(b))

#define SCR_SHORT(addr)  (*(volatile short *)(addr))
#define SCR_USHORT(addr) (*(volatile ushort *)(addr))
#define SCR_INT(addr)    (*(volatile int *)(addr))
#define SCR_UINT(addr)   (*(volatile uint *)(addr))

#define gte_SetRotMatrix(r0) __asm__ volatile( \
    "lw $12, 0(%0);" "lw $13, 4(%0);" \
    "ctc2 $12, $0;" "ctc2 $13, $1;" \
    "lw $12, 8(%0);" "lw $13, 12(%0);" "lw $14, 16(%0);" \
    "ctc2 $12, $2;" "ctc2 $13, $3;" "ctc2 $14, $4" \
    :: "r"(r0) : "$12", "$13", "$14")

#define gte_SetTransMatrix(r0) __asm__ volatile( \
    "lw $12, 20(%0);" "lw $13, 24(%0);" \
    "ctc2 $12, $5;" \
    "lw $14, 28(%0);" \
    "ctc2 $13, $6;" "ctc2 $14, $7" \
    :: "r"(r0) : "$12", "$13", "$14")

#define gte_ldv0(p) __asm__ volatile("lwc2 $0, 0(%0)\nlwc2 $1, 4(%0)" :: "r"(p))
#define gte_ldv1(p) __asm__ volatile("lwc2 $2, 0(%0)\nlwc2 $3, 4(%0)" :: "r"(p))
#define gte_ldv2(p) __asm__ volatile("lwc2 $4, 0(%0)\nlwc2 $5, 4(%0)" :: "r"(p))
#define gte_rtpt()  __asm__ volatile("nop\nnop\ncop2 0x0280030")
#define gte_stsxy3c(p) __asm__ volatile("swc2 $12, 0(%0)\nswc2 $13, 4(%0)\nswc2 $14, 8(%0)" :: "r"(p) : "memory")
#define gte_stSZ1(p)   __asm__ volatile("swc2 $17, 0(%0)" :: "r"(p) : "memory")

extern byte *PTR_DAT_8008d2ac;
extern void FUN_800685b0(void *p);
extern int FUN_8003d184(int angle);
extern int FUN_8003d1c0(int angle);
extern void FUN_800683f4(void *p, int a, int b, void *c);

void VehStuckProc_Warp_AddDustPuff2(int param_1, int param_2)
{
    short sVar1;
    short sVar2;
    ushort uVar3;
    short sVar4;
    short sVar5;
    short sVar6;
    uint *puVar7;
    int iVar8;
    uint uVar9;
    uint *puVar10;
    uint *puVar12;
    uint *puVar13;
    volatile short *psVar14;
    volatile short *psVar15;
    uint *puVar16;
    undefined4 *puVar17;
    int iVar18;
    uint *puVar19;
    byte *puVar20;
    byte *matrixPtr;
    uint *szPtr;
    int local_30;
    short extraout_var;
    short extraout_var_00;

    puVar20 = PTR_DAT_8008d2ac + (uint)*(byte *)(param_1 + 0x4a) * 0x110 + 0x168;

    matrixPtr = puVar20 + 0x28;
    gte_SetRotMatrix(matrixPtr);
    gte_SetTransMatrix(matrixPtr);

    sVar1 = *(short *)(puVar20 + 0x48);
    sVar2 = *(short *)(puVar20 + 0x4e);
    uVar3 = *(ushort *)(puVar20 + 0x54);

    SCR_SHORT(0x1f8001c0) = (short)(((int)sVar1 + (int)*(short *)(puVar20 + 0x4a)) >> 5);
    SCR_SHORT(0x1f8001c2) = (short)(((int)*(short *)(puVar20 + 0x4e) + (int)*(short *)(puVar20 + 0x50)) >> 5);
    SCR_SHORT(0x1f8001c4) = (short)(((int)*(short *)(puVar20 + 0x54) + (int)*(short *)(puVar20 + 0x56)) >> 5);

    puVar19 = *(uint **)(*(int *)(PTR_DAT_8008d2ac + 0x10) + 0x80);

    if ((*(uint *)(*(int *)(param_1 + 0x1c) + 0x28) & 0x80) != 0)
    {
        SCR_SHORT(0x1f800188) = (short)((uint)*(undefined4 *)(param_1 + 0x2d4) >> 8);
        SCR_SHORT(0x1f80018a) = (short)((uint)*(undefined4 *)(param_2 + 0x10) >> 8);
        SCR_SHORT(0x1f80018c) = (short)((uint)*(undefined4 *)(param_1 + 0x2dc) >> 8);
        FUN_800685b0((void *)0x1f800188);
    }

    local_30 = 0;
    do {
        iVar18 = (local_30 << 0xc) / 6;

        iVar8 = FUN_8003d184(iVar18 + *(int *)(param_2 + 0xc));

        SCR_UINT(0x1f800108) =
            CONCAT22((short)((uint)*(undefined4 *)(param_2 + 8) >> 8),
                     (short)((uint)*(undefined4 *)(param_1 + 0x2d4) >> 8) - (short)(iVar8 >> 5));

        iVar8 = FUN_8003d1c0(iVar18 + *(int *)(param_2 + 0xc));

        SCR_UINT(0x1f80010c) =
            SCR_UINT(0x1f80010c) & 0xffff0000 |
            (uint)(ushort)((short)((uint)*(undefined4 *)(param_1 + 0x2dc) >> 8) - (short)(iVar8 >> 5));

        SCR_SHORT(0x1f800188) = (short)((uint)*(undefined4 *)(param_1 + 0x2d4) >> 8);
        SCR_SHORT(0x1f80018a) = (short)((uint)*(undefined4 *)(param_2 + 0x10) >> 8);
        SCR_SHORT(0x1f80018c) = (short)((uint)*(undefined4 *)(param_1 + 0x2dc) >> 8);

        if ((*(uint *)(*(int *)(param_1 + 0x1c) + 0x28) & 0x80) == 0) {

            iVar8 = FUN_8003d184(iVar18 + *(int *)(param_2 + 0xc));

            SCR_UINT(0x1f800108) =
                SCR_UINT(0x1f800108) & 0xffff0000 | (uint)(ushort)(SCR_SHORT(0x1f800108) - (short)(iVar8 >> 6));

            iVar8 = FUN_8003d1c0(iVar18 + *(int *)(param_2 + 0xc));

            SCR_UINT(0x1f80010c) =
                SCR_UINT(0x1f80010c) & 0xffff0000 | (uint)(ushort)(SCR_SHORT(0x1f80010c) - (short)(iVar8 >> 6));

            iVar8 = FUN_8003d184(iVar18 + *(int *)(param_2 + 0xc));
            SCR_SHORT(0x1f800188) = SCR_SHORT(0x1f800188) + (short)(iVar8 >> 8);

            iVar8 = FUN_8003d1c0(iVar18 + *(int *)(param_2 + 0xc));
            SCR_SHORT(0x1f80018c) = SCR_SHORT(0x1f80018c) + (short)(iVar8 >> 8);
        }
        else {
            FUN_800685b0((void *)0x1f800108);
        }

        FUN_800683f4((void *)0x1f800108, 0x10, 0x100, (void *)0x1f8001c0);

        iVar18 = 1;
        puVar17 = (undefined4 *)0x1f800108;
        do {
            iVar8 = FUN_8003d184(iVar18 << 7);
            iVar18 = iVar18 + 1;
            *(short *)((char *)puVar17 + 10) = *(short *)((char *)puVar17 + 10) + (short)(iVar8 >> 7);
            puVar17 = puVar17 + 2;
        } while (iVar18 < 0x10);

        sVar4 = sVar1 >> 10;
        sVar5 = sVar2 >> 10;
        SCR_UINT(0x1f800190) = CONCAT22(SCR_SHORT(0x1f80010a) + sVar5, SCR_SHORT(0x1f800108) + sVar4);
        sVar6 = (short)uVar3 >> 10;
        SCR_UINT(0x1f800194) = SCR_UINT(0x1f800194) & 0xffff0000 | (uint)(ushort)(SCR_SHORT(0x1f80010c) + sVar6);
        uVar9 = (uint)(SCR_INT(0x1f80010c) & 0xffff) - (uint)((int)((uint)uVar3 << 0x10) >> 0x1a);
        SCR_UINT(0x1f800198) = CONCAT22(SCR_SHORT(0x1f80010a) - sVar5, SCR_SHORT(0x1f800108) - sVar4);
        SCR_UINT(0x1f80019c) = SCR_UINT(0x1f80019c) & 0xffff0000 | uVar9 & 0xffff;

        gte_ldv0((void *)0x1f800190);
        gte_ldv1((void *)0x1f800108);
        gte_ldv2((void *)0x1f800198);
        gte_rtpt();
        gte_stsxy3c((void *)0x1f8001a0);
        gte_stSZ1((void *)0x1f8001ac);

        iVar18 = 0;
        puVar12 = puVar19 + 0x11;
        puVar7 = (uint *)0x1f8001a0;
        psVar14 = (volatile short *)0x1f80010c;
        puVar16 = (uint *)0x1f8001b0;
        puVar17 = (undefined4 *)0x1f800108;
        do {
            puVar13 = puVar7;
            puVar17 = puVar17 + 2;
            psVar15 = psVar14 + 4;
            SCR_UINT(0x1f800190) = CONCAT22(psVar14[3] + sVar5, *(short *)puVar17 + sVar4);
            SCR_UINT(0x1f800194) = SCR_UINT(0x1f800194) & 0xffff0000 | (uint)(ushort)(psVar15[0] + sVar6);
            SCR_UINT(0x1f800198) = CONCAT22(psVar14[3] - sVar5, *(short *)puVar17 - sVar4);
            SCR_UINT(0x1f80019c) = SCR_UINT(0x1f80019c) & 0xffff0000 | (uint)(ushort)(psVar15[0] - sVar6);

            gte_ldv0((void *)0x1f800190);
            gte_ldv1(puVar17);
            gte_ldv2((void *)0x1f800198);
            gte_rtpt();
            gte_stsxy3c(puVar16);
            szPtr = puVar16 + 3;
            gte_stSZ1(szPtr);

            puVar12[-0x10] = 0xe1000a20;
            puVar12[-0xf] = 0x3a000000;
            puVar12[-0xd] = 0x7f1f3f;
            puVar12[-0xb] = 0;
            puVar12[-9] = 0x7f1f3f;
            puVar12[-0xe] = *puVar16;
            puVar12[-0xc] = puVar16[1];
            puVar12[-10] = *puVar13;
            uVar9 = puVar13[1];
            puVar12[-7] = 0x3a000000;
            puVar12[-5] = 0x7f1f3f;
            puVar12[-3] = 0;
            puVar12[-1] = 0x7f1f3f;
            puVar12[-8] = uVar9;
            puVar12[-6] = puVar16[2];
            puVar12[-4] = puVar16[1];
            iVar18 = iVar18 + 1;
            puVar12[-2] = puVar13[2];
            *puVar12 = puVar13[1];
            puVar10 = (uint *)(*(int *)(puVar20 + 0xf4) + ((int)puVar16[3] >> 6) * 4);
            puVar12 = puVar12 + 0x12;
            *puVar19 = *puVar10 | 0x11000000;
            *puVar10 = (uint)puVar19 & 0xffffff;
            puVar19 = puVar19 + 0x12;
            puVar7 = puVar16;
            psVar14 = psVar15;
            puVar16 = puVar13;
        } while (iVar18 < 0x10);

        local_30 = local_30 + 1;
    } while (local_30 < 6);

    *(uint **)(*(int *)(PTR_DAT_8008d2ac + 0x10) + 0x80) = puVar19;

    return;
}
