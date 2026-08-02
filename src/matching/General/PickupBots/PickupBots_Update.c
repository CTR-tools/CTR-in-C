/*
 * PickupBots_Update @ 0x800408B8  (vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact: Branch 0 / Stack 0 (control flow + stack layout match exactly).
 * Residual is pure register-coloring/scheduling (~783 insns, two reused-register code
 * paths). The per-frame boss/arcade AI weapon dispatcher: arcade fires bot weapons at the
 * player (RNG item rolls, cooldowns); boss path runs the boss-weapon-meta state machine
 * (path request, juice counter, weapon select + Voiceline + ShootNow).
 * Field access via offset macros (offsets are lw/sw immediates — register/score only,
 * never Branch/Stack). Signedness taken from asm (lb/lh signed, lbu/lhu unsigned, sll/sra
 * = sign-extend). Faithful transcription of the Ghidra decompile.
 *
 * Phase B (saphi game/PickupBots/PickupBots_1_Update.c): CLEAN
 */
/* PickupBots_Update @ 0x800408B8 — boss/arcade AI weapon dispatcher.
   Faithful transcription of the Ghidra decompile; field access via offset macros
   (offset-exact where known — offsets are lw/sw immediates, so they affect register/
   score only, never Branch/Stack). */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define FB(p,o)  (*(unsigned char *)((char *)(p)+(o)))
#define FSB(p,o) (*(signed char  *)((char *)(p)+(o)))
#define FH(p,o)  (*(short         *)((char *)(p)+(o)))
#define FHU(p,o) (*(unsigned short*)((char *)(p)+(o)))
#define FW(p,o)  (*(int           *)((char *)(p)+(o)))
#define FP(p,o)  (*(void *        *)((char *)(p)+(o)))

extern int  func_8003EA28(void);                 /* MixRNG_Scramble */
extern u32  func_8006C684(void *rng);            /* RNG_Random */
extern void func_8002CBE8(int voice, int cA, int cB); /* Voiceline_RequestPlay */
extern void func_8006540C(void *driver, int weaponID, int flags); /* VehPickupItem_ShootNow */

extern char *D_8008D2AC;      /* gGT */
extern void *D_800859D0[];    /* g_apBossWeaponMeta */
extern short D_80083A80[];    /* metaDataLEV (stride 0x18, hubID@0) */
extern char  D_8008D668;      /* g_mainRNG */
extern short D_8008FBA4[];    /* g_awCharacterIDs */
extern signed char D_80086E84[]; /* g_abAdvProgress */
void *D_8008D8E8 = 0;         /* g_pBossWeaponMeta (gp+0x97C) */
int   D_8008D8E4 = 0;         /* boss weapon timer   (gp+0x978) */
short D_8008D428 = 0;         /* bossPathRequestTimer (gp+0x4BC) */
short D_8008D42A = 0;         /* bossPathRequestPhase (gp+0x4BE) */
short D_8008D42C = 0;         /* bossJuiceCounter     (gp+0x4C0) */

#define METALEV_STRIDE 0x18

void PickupBots_Update(void)
{
    char *gGT = D_8008D2AC;
    u8 *pbVar15 = (u8 *)D_8008D8E8;
    int iVar6, iVar9, iVar10;
    u32 uVar7;
    u16 uVar14, uVar17;
    u8 bVar11;
    char cVar1, cVar12;
    short sVar2;
    void *psVar16, *psVar18;

    if ((FB(gGT, 0x1CAB) == 0) || (FW(gGT, 0x1D10) < 0x4b00)) {
        if (0 <= FW(gGT, 0x0)) return;
        if (FW(gGT, 0x1D10) < 0x12c0) return;
    }
    if ((FW(gGT, 0x0) & 0x80200000) != 0x80000000) {
        /* ---- ARCADE ---- */
        iVar6 = 0;
        if (FB(gGT, 0x1CA8) == 0) return;
        do {
            psVar16 = FP(gGT, 0x24EC + iVar6 * 4);            /* player = drivers[i] */
            if (FH(psVar16, 0x482) != 0) {                    /* driverRank != 0 */
                psVar18 = FP(gGT, 0x24EC + (FH(psVar16, 0x482) + 7) * 4); /* bot = drivers[rank+7] */
                if (((FW(psVar18, 0x2C8) & 0x2) != 0) &&
                    ((FW(psVar18, 0x5B0) & 0x2) == 0) &&
                    ((FW(psVar18, 0x2C8) & 0x2000000) == 0) &&
                    (FH(psVar18, 0x624) == 0) && (FP(psVar18, 0x18) == 0) &&
                    (FH(psVar18, 0xC) == 0) &&
                    (iVar9 = FW(FP(psVar16, 0x1C), 0x44) - FW(FP(psVar18, 0x1C), 0x44),
                     iVar10 = FW(FP(psVar16, 0x1C), 0x4C) - FW(FP(psVar18, 0x1C), 0x4C),
                     (u32)((iVar9 * iVar9 + iVar10 * iVar10) - 0x90001) < 0x13affff)) {
                    iVar9 = func_8003EA28() % 200;
                    if (iVar9 == 0) {
                        if ((FB(psVar18, 0x44) != 0) && (func_8003EA28() % 100 < 0x32)) {
                            FB(psVar18, 0x30) = 10;
                        }
                        if ((FW(gGT, 0x1D10) & 1) == 0) {
                            FB(psVar18, 0x36) = 4;
                            if ((FW(psVar16, 0x2C8) & 0x2) == 0)
                                func_8002CBE8(0xf, FH(D_8008FBA4, FB(psVar18, 0x4A) * 2),
                                                   FH(D_8008FBA4, FB(psVar16, 0x4A) * 2));
                            iVar9 = 4;
                        } else {
                            FB(psVar18, 0x36) = 3;
                            if ((FW(psVar16, 0x2C8) & 0x2) == 0)
                                func_8002CBE8(0xf, FH(D_8008FBA4, FB(psVar18, 0x4A) * 2),
                                                   FH(D_8008FBA4, FB(psVar16, 0x4A) * 2));
                            iVar9 = 3;
                        }
                        func_8006540C(psVar18, iVar9, 0);
                        FB(psVar18, 0x30) = 0;
                        FH(psVar18, 0x624) = (func_8003EA28() & 0xff) + 0xf0;
                    } else if (iVar9 == 1) {
                        FB(psVar18, 0x36) = 1;
                        if ((FW(psVar16, 0x2C8) & 0x2) == 0)
                            func_8002CBE8(0xa, FH(D_8008FBA4, FB(psVar18, 0x4A) * 2),
                                               FH(D_8008FBA4, FB(psVar16, 0x4A) * 2));
                        func_8006540C(psVar18, 2, 0);
                        FH(psVar18, 0x624) = (func_8003EA28() & 0xff) + 0xf0;
                    } else if (iVar9 == 2) {
                        FB(psVar18, 0x36) = 2;
                        if ((FW(psVar16, 0x2C8) & 0x2) == 0)
                            func_8002CBE8(0xb, FH(D_8008FBA4, FB(psVar18, 0x4A) * 2),
                                               FH(D_8008FBA4, FB(psVar16, 0x4A) * 2));
                        func_8006540C(psVar18, 2, 0);
                        FH(psVar18, 0x624) = (func_8003EA28() & 0xff) + 0xf0;
                    }
                    FB(psVar18, 0x36) = 0xf;
                }
            }
            if (FH(psVar16, 0x482) < 3) {
                psVar18 = FP(gGT, 0x250C + (FH(psVar16, 0x482) + 1) * 4); /* driversInRaceOrder[rank+1] */
                if (((FW(psVar18, 0x2C8) & 0x2) != 0) &&
                    ((FW(psVar18, 0x5B0) & 0x2) == 0) &&
                    ((FW(psVar18, 0x2C8) & 0x2000000) == 0) &&
                    (FH(psVar18, 0x624) == 0) && (FP(psVar18, 0x18) == 0) &&
                    (FH(psVar18, 0xC) == 0) &&
                    (((int)(u32)FB(psVar16, 0x44) < (int)FSB(gGT, 0x1D33)) ||
                     (16000 < FW(psVar16, 0x488))) &&
                    (iVar9 = FW(FP(psVar16, 0x1C), 0x44) - FW(FP(psVar18, 0x1C), 0x44),
                     iVar10 = FW(FP(psVar16, 0x1C), 0x4C) - FW(FP(psVar18, 0x1C), 0x4C),
                     (u32)((iVar9 * iVar9 + iVar10 * iVar10) - 0x90001) < 0x13affff)) {
                    iVar9 = func_8003EA28();
                    if (((iVar9 % 800 < 2) && (bVar11 = 2, (u32)FB(psVar18, 0x44) != (int)FSB(gGT, 0x1D33) - 1U)) ||
                        (bVar11 = 1, iVar9 % 800 < 4)) {
                        FB(psVar18, 0x36) = bVar11;
                        if ((FW(psVar16, 0x2C8) & 0x2) == 0)
                            func_8002CBE8(0xb, FH(D_8008FBA4, FB(psVar18, 0x4A) * 2),
                                               FH(D_8008FBA4, FB(psVar16, 0x4A) * 2));
                        func_8006540C(psVar18, 2, 0);
                        FH(psVar18, 0x624) = (func_8003EA28() & 0xff) + 0xf0;
                    }
                    FB(psVar18, 0x36) = 0xf;
                }
            }
            iVar6 = iVar6 + 1;
        } while (iVar6 < (int)(u32)FB(gGT, 0x1CA8));
        return;
    }

    /* ---- BOSS ---- */
    psVar16 = FP(gGT, 0x24F0);   /* drivers[1] = boss */
    psVar18 = FP(gGT, 0x24EC);   /* drivers[0] = player */
    if (((FW(psVar16, 0x5B0) & 0x2) != 0) || ((FW(psVar16, 0x2C8) & 0x2000000) != 0) ||
        (FP(psVar16, 0x18) != 0) || (FH(psVar16, 0xC) != 0) || (FW(psVar16, 0x5D4) < 0x1f41)) {
        uVar7 = func_8006C684(&D_8008D668);
        D_8008D8E4 = ((u16)uVar7 & 0x10) + FH(pbVar15, 4) + 0xc + FSB(D_80086E84, FW(gGT, 0x1F98) + 0x47) * 4;
        return;
    }
    if (pbVar15[9] == 0) {
        if ((int)((u32)FHU(FP(FP(gGT, 0x160), 0x14C), pbVar15[0] * 0xc + 6) << 3) < FW(psVar16, 0x488)) {
            uVar14 = 0xffff;
            if (((pbVar15[2] == 0x66) || (pbVar15[2] == 100)) && (D_8008D42C == 5))
                uVar14 = pbVar15[1];
            pbVar15 = (u8 *)D_800859D0[0];
            if (FW(gGT, 0x1A10) != 0xd)
                pbVar15 = (u8 *)D_800859D0[D_80083A80[FW(gGT, 0x1A10) * (METALEV_STRIDE/2)]];
            if (uVar14 != 0xffff) pbVar15[1] = (u8)uVar14;
        }
    } else if (FW(psVar16, 0x488) <
               (int)((u32)FHU(FP(FP(gGT, 0x160), 0x14C), pbVar15[8] * 0xc + 6) << 3)) {
        bVar11 = 0xff;
        iVar6 = -0x10000;
        if (((pbVar15[2] == 0x66) || (pbVar15[2] == 100)) && (D_8008D42C == 5)) {
            bVar11 = pbVar15[1];
            iVar6 = (u32)bVar11 << 0x10;
        }
        pbVar15 = pbVar15 + 8;
        if (iVar6 >> 0x10 != -1) pbVar15[1] = bVar11;
    }
    D_8008D8E8 = pbVar15;

    if (pbVar15[3] == 0) {
        if (D_8008D428 == 0x1e) {
            if ((FW(psVar16, 0x5B0) & 0x80) == 0) {
                if (D_8008D42A == 0) {
                    sVar2 = FH(psVar16, 0x5B8);
                    if (sVar2 == 2) goto path_setDesired1;
                    if (sVar2 == 1) {
                        FB(psVar16, 0x627) = 0;
                        D_8008D428 = 0;
                        D_8008D42A = sVar2;
                        FW(psVar16, 0x5B0) = FW(psVar16, 0x5B0) | 0x40;
                    }
                } else {
                    if (FH(psVar16, 0x5B8) == 0) {
                    path_setDesired1:
                        FB(psVar16, 0x627) = 1;
                        D_8008D428 = 0;
                        FW(psVar16, 0x5B0) = FW(psVar16, 0x5B0) | 0x40;
                    } else if (FH(psVar16, 0x5B8) == 1) {
                        FB(psVar16, 0x627) = 2;
                        D_8008D42A = 0;
                        D_8008D428 = 0;
                        FW(psVar16, 0x5B0) = FW(psVar16, 0x5B0) | 0x40;
                    }
                }
            }
        } else if ((FW(psVar16, 0x5B0) & 0x40) == 0) {
            D_8008D428 = D_8008D428 + 1;
        }
    }

    if (0 < D_8008D8E4) {
        D_8008D8E4 = D_8008D8E4 + -1;
        return;
    }
    uVar7 = func_8006C684(&D_8008D668);
    D_8008D8E4 = ((u16)uVar7 & 0x10) + FH(pbVar15, 4) + 0xc + FSB(D_80086E84, FW(gGT, 0x1F98) + 0x47) * 4;
    uVar14 = (u16)pbVar15[2];
    if (uVar14 == 100) uVar14 = 3;
    else if (uVar14 == 0x65) uVar14 = 1;
    else if (uVar14 == 0x66) uVar14 = 4;
    else if (uVar14 == 0xf) uVar14 = 0xffff;
    uVar17 = FHU(pbVar15, 6);
    if ((uVar17 & 2) == 0) {
    juice_reset:
        D_8008D42C = 0;
        sVar2 = D_8008D42C;
    } else {
        sVar2 = D_8008D42C + 1;
        if (D_8008D42C < 5) goto juice_apply;
        bVar11 = pbVar15[2];
        if (bVar11 == 100) {
            uVar14 = 3;
            if (pbVar15[1] != 3) {
                pbVar15[1] = 3;
                D_8008D42C = 5;
                FHU(pbVar15, 6) = uVar17 | 1;
                sVar2 = D_8008D42C;
                goto juice_apply;
            }
        } else if (bVar11 == 0x65) {
            uVar14 = 1;
            if ((uVar17 & 1) == 0) {
                FHU(pbVar15, 6) = uVar17 | 1;
                D_8008D42C = 5;
                uVar14 = 3;
                sVar2 = D_8008D42C;
                goto juice_apply;
            }
            FHU(pbVar15, 6) = uVar17 & 0xfffe;
            goto juice_reset;
        } else {
            sVar2 = D_8008D42C;
            if (bVar11 != 0x66) goto juice_apply;
            uVar14 = 4;
            if (pbVar15[1] != 3) {
                pbVar15[1] = 3;
                D_8008D42C = 5;
                FHU(pbVar15, 6) = FHU(pbVar15, 6) | 1;
                sVar2 = D_8008D42C;
                goto juice_apply;
            }
        }
        pbVar15[1] = 2;
        D_8008D42C = 0;
        FHU(pbVar15, 6) = FHU(pbVar15, 6) & 0xfffe;
        sVar2 = D_8008D42C;
    }
juice_apply:
    D_8008D42C = sVar2;
    bVar11 = pbVar15[1];
    uVar17 = (u16)(bVar11 == 2);
    cVar12 = 0;
    if (-1 < (int)((u32)uVar14 << 0x10)) {
        cVar1 = FB(psVar16, 0x30);
        if ((FHU(pbVar15, 6) & 1) != 0) cVar12 = 10;
        FB(psVar16, 0x30) = cVar12;
        FB(psVar16, 0x36) = (u8)uVar14;
        if ((u16)(uVar14 - 3) < 2) {
            func_8002CBE8(0xf, FH(D_8008FBA4, FB(psVar16, 0x4A) * 2),
                               FH(D_8008FBA4, FB(psVar18, 0x4A) * 2));
        } else {
            uVar17 = (bVar11 == 2) | 2;
            func_8002CBE8(0xa, FH(D_8008FBA4, FB(psVar16, 0x4A) * 2),
                               FH(D_8008FBA4, FB(psVar18, 0x4A) * 2));
        }
        if (FB(psVar16, 0x36) == 1) {
            func_8006540C(psVar16, 2, (short)uVar17);
        } else if ((FB(psVar16, 0x36) == 4) && (uVar17 == 1) && (FW(gGT, 0x1A10) == 0xd)) {
            func_8006540C(psVar16, (short)uVar14, 1);
            func_8006540C(psVar16, (short)uVar14, 1);
        } else {
            func_8006540C(psVar16, (short)uVar14, (short)uVar17);
            if ((FB(psVar16, 0x36) == 3) && (pbVar15[1] == 3) && (D_8008D42C != 5))
                D_8008D42C = 5;
        }
        FB(psVar16, 0x36) = 0xf;
        FB(psVar16, 0x30) = cVar1;
    }
}
