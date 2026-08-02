/*
 * PlayLevel_UpdateLapStats @ 0x800414F4  (vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact: Branch 0 / Stack 0 (control flow + stack layout match exactly).
 * Residual is pure register-coloring/scheduling (~484 insns). Per-frame lap/rank/race-end:
 * find lead human, run VehLap_UpdateProgress + lap-cross detection (lap counter, SaveLapTime,
 * final-lap timer, race-finish/confetti/robotcar-convert), sort unsorted drivers by lap/progress,
 * rebuild driversInRaceOrder, overtake voicelines, and the multi-mode race-end check.
 * Field access via offset macros. Signedness from asm (lb/lh/slti/blez/bltz signed;
 * lbu/lhu/sltu/srl unsigned). Faithful transcription of the Ghidra decompile (non-online retail).
 *
 * Phase B (saphi game/PlayLevel/PlayLevel_0_UpdateLapStats.c): CLEAN
 */
/* PlayLevel_UpdateLapStats @ 0x800414F4 — per-frame lap/rank/race-end update.
   Faithful transcription of the Ghidra decompile; field access via offset macros
   (offsets are lw/sw immediates -> register/score only, never Branch/Stack).
   Signedness from asm: lb/lh signed, lbu/lhu unsigned, slti/slt/blez/bltz signed, sltu/srl unsigned. */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define FB(p,o)  (*(unsigned char *)((char *)(p)+(o)))
#define FSB(p,o) (*(signed char  *)((char *)(p)+(o)))
#define FH(p,o)  (*(short         *)((char *)(p)+(o)))
#define FHU(p,o) (*(unsigned short*)((char *)(p)+(o)))
#define FW(p,o)  (*(int           *)((char *)(p)+(o)))
#define FP(p,o)  (*(void *        *)((char *)(p)+(o)))

extern void func_8005CA24(void *drv);              /* VehLap_UpdateProgress */
extern void func_8004C55C(int lap, int t, int id); /* UI_SaveLapTime */
extern void func_80028468(int id, int ch);         /* OtherFX_Play */
extern void func_8002CAA8(void);                   /* Voiceline_ClearTimeStamp */
extern void func_80017318(void *drv);              /* BOTS_Driver_Convert */
extern void func_8002CBE8(int v, int cA, int cB);  /* Voiceline_RequestPlay */
extern void func_80064568(void *drv, int st, void *by, int at); /* VehPickState_NewState */
extern void func_8003A3FC(void);                   /* MainGameEnd_Initialize */

extern char  *D_8008D2AC;   /* gGT */
extern short  D_8008D2A0[]; /* finalLapTextTimer */
extern u8     D_8008D69C[]; /* g_abKartSpawnOrder */
extern short  D_80086E84[]; /* g_awCharacterIDs */
short D_8008D2AA = 0;        /* numPlayersFinishedRace */

void PlayLevel_UpdateLapStats(void)
{
    char *gGT = D_8008D2AC;
    char *psVar8 = gGT;
    void *firstRank = 0;
    void *driver, *psVar3;
    int iVar12 = 0, iVar14 = 0, iVar10 = 0, iVar11;
    int iVar4, iVar7;
    u32 uVar2, uVar9, uVar5 = 0;

    /* loop 1: farthest-ahead human */
    firstRank = 0;
    for (iVar10 = 0; iVar10 < 8; iVar10++) {
        psVar3 = FP(gGT + iVar10 * 4, 0x250C);         /* driversInRaceOrder[iVar10] */
        if ((psVar3 != 0) && ((FW(psVar3, 0x2C8) & 0x100000) == 0)) {
            firstRank = psVar3;
            break;
        }
    }

    iVar11 = 0;
    iVar10 = 0;
    do {
        driver = *(void **)(psVar8 + 0x24EC + iVar10);
        *(int *)(psVar8 + 0x250C + iVar10) = 0;
        if (driver != 0) {
            uVar9 = FW(driver, 0x488);
            func_8005CA24(driver);
            uVar2 = FW(driver, 0x490) + (FW(driver, 0x488) - uVar9);
            FW(driver, 0x490) = uVar2;
            if ((int)uVar2 < 0)         FW(driver, 0x490) = 0;
            else if (1000 < (int)uVar2) FW(driver, 0x490) = 1000;

            if (((int)uVar9 < 0x4b0) && (32000 < (int)FW(driver, 0x488))) {
                FW(driver, 0x490) = 0;
                if ((FW(driver, 0x2C8) & 0x1000000) != 0) {
                    FW(driver, 0x2C8) = FW(driver, 0x2C8) & 0xfeffffff;
                    goto LAB_800418b4;
                }
                FW(driver, 0x48C) = FW(driver, 0x488);
                if ((int)FSB(gGT, 0x1D33) < (int)(FB(driver, 0x44) + 1)) {
                LAB_80041720:
                    uVar5 = FB(driver, 0x44);
                } else {
                    if (((FW(gGT, 0x0) & 0x4a0000) != 0) &&
                        (FH(FP(FP(driver, 0x1C), 0x6C), 0x44) == 0x18)) {
                        func_8004C55C(FB(driver, 0x44), FW(gGT, 0x1D10) - FW(driver, 0x40), FB(driver, 0x4A));
                        FW(gGT, 0x1D14 + FB(driver, 0x44) * 4) = FW(gGT, 0x1D10) - FW(driver, 0x40);
                    }
                    FW(driver, 0x40) = FW(gGT, 0x1D10);
                    FB(driver, 0x44) = FB(driver, 0x44) + 1;
                    if (driver == firstRank) {
                        func_80028468(0x66, 1);
                        func_8002CAA8();
                    }
                    uVar5 = FB(driver, 0x44);
                    if (uVar5 == (u32)((int)FSB(gGT, 0x1D33) - 1)) {
                        if ((FW(driver, 0x2C8) & 0x100000) == 0)
                            D_8008D2A0[iVar11] = 0x5a;
                        goto LAB_80041720;
                    }
                }
                if (uVar5 != (u32)(int)FSB(gGT, 0x1D33)) goto LAB_800418b4;
                if ((FW(driver, 0x2C8) & 0x2000000) == 0) {
                    FW(driver, 0x2C8) = FW(driver, 0x2C8) | 0x2000000;
                    if ((FW(gGT, 0x0) & 0x200000) == 0) {
                        FH(driver, 0x482) = D_8008D2AA;
                        D_8008D2AA = D_8008D2AA + 1;
                    }
                    FB(driver, 0x36) = 0xf;
                    if ((FW(driver, 0x2C8) & 0x100000) == 0) {
                        if (FH(driver, 0x482) == 0) {
                            FH(gGT, 0x1B04) = 0xfa;
                            FH(gGT, 0x1B06) = 0xfa;
                            FW(gGT, 0x2558) = FB(driver, 0x4A);
                            FW(gGT, 0x2568) = 1;
                            FH(gGT, FB(driver, 0x4A) * 0x110 + 0x17A) = 0x1fff;
                            FH(gGT, FW(gGT, 0x2558) * 0x110 + 0x17C) = 0x1000;
                            FH(gGT, FW(gGT, 0x2558) * 0x110 + 0x17E) = -0x88;
                        }
                        if (FH(driver, 0x3C) != 0) {
                            FH(driver, 0x3C) = 0;
                            FB(driver, 0x36) = 0xf;
                        }
                        func_80017318(driver);
                    }
                    goto LAB_800418b4;
                }
            } else {
                if (((int)FW(driver, 0x488) < 0x4b0) && (32000 < (int)uVar9)) {
                    FW(driver, 0x490) = 600;
                    FW(driver, 0x2C8) = FW(driver, 0x2C8) | 0x1000000;
                } else if (((FW(driver, 0x2C8) & 0x1000000) == 0) &&
                           ((u32)(FW(driver, 0x48C) - FW(driver, 0x488)) <=
                            (u32)(((u32)FHU(FP(FP(gGT, 0x160), 0x14C), 6) >> 2) << 3))) {
                    FW(driver, 0x48C) = FW(driver, 0x488);
                }
            LAB_800418b4:
                if ((FW(driver, 0x2C8) & 0x2000000) == 0) {
                    FH(driver, 0x482) = -1;
                    goto LAB_80041910;
                }
            }
            if (FH(FP(FP(driver, 0x1C), 0x6C), 0x44) == 0x18) iVar14 = iVar14 + 1;
            iVar10 = FH(driver, 0x482) + 1;
            if (iVar12 < iVar10) iVar12 = iVar10;
        }
    LAB_80041910:
        iVar11 = iVar11 + 1;
        iVar10 = iVar11 * 4;
        if (7 < iVar11) {
            /* ---- sort unsorted drivers by lap/progress ---- */
            for (; iVar12 < 8; iVar12++) {
                uVar2 = 0x3fffffff;
                iVar11 = -1;
                iVar10 = -10;
                for (iVar7 = 0; iVar7 < 8; iVar7++) {
                    psVar3 = FP(gGT + iVar7 * 4, 0x24EC);
                    if ((psVar3 != 0) && (FH(psVar3, 0x482) == -1) &&
                        (iVar4 = (int)FB(psVar3, 0x44) - (FB(psVar3, 0x2CB) & 1),
                         (iVar10 < iVar4) ||
                         ((iVar4 == iVar10) && ((int)FW(psVar3, 0x488) < (int)uVar2)))) {
                        uVar2 = FW(psVar3, 0x488);
                        iVar10 = iVar4;
                        iVar11 = iVar7;
                    }
                }
                if (iVar11 != -1) {
                    if (FW(gGT, 0x1D0C) < 1) {
                        FH(FP(gGT + iVar11 * 4, 0x24EC), 0x482) = (short)iVar12;
                    } else {
                        FH(FP(gGT + iVar11 * 4, 0x24EC), 0x482) = D_8008D69C[iVar11];
                        FB(gGT, 0x257A + iVar11) = D_8008D69C[iVar11];
                    }
                }
            }
            /* ---- assign driversInRaceOrder ---- */
            for (iVar12 = 0; iVar12 < 8; iVar12++) {
                psVar3 = FP(gGT + iVar12 * 4, 0x24EC);
                if ((psVar3 != 0) && (-1 < FH(psVar3, 0x482)))
                    FP(gGT + FH(psVar3, 0x482) * 4, 0x250C) = psVar3;
            }
            /* ---- overtake voicelines ---- */
            if (FB(gGT, 0x1CA8) != 0) {
                for (iVar12 = 0; iVar12 < (int)(u32)FB(gGT, 0x1CA8); iVar12++) {
                    psVar3 = FP(gGT + iVar12 * 4, 0x24EC);
                    if (psVar3 != 0) {
                        iVar10 = FH(psVar3, 0x482);
                        if ((-1 < iVar10) && (0x4b00 < FW(gGT, 0x1D10)) &&
                            ((int)FSB(gGT, 0x257A + iVar12) < iVar10)) {
                            func_8002CBE8(8, D_80086E84[FB(FP(gGT + (iVar10 + 7) * 4, 0x24EC), 0x4A)], 0x10);
                        }
                        FB(gGT, 0x257A + iVar12) = (u8)FH(psVar3, 0x482);
                    }
                }
            }
            /* ---- race-end check ---- */
            if ((FW(gGT, 0x0) & 0x200000) == 0) {
                uVar5 = (u32)FB(gGT, 0x1CA8);
                if ((((uVar5 == 1) && (0 < iVar14)) ||
                     (((int)(uVar5 - 1) <= iVar14) && (1 < uVar5) && ((FW(gGT, 0x0) & 0x400000) == 0))) ||
                    (((FW(gGT, 0x0) & 0x400000) != 0) && ((int)(u32)FB(gGT, 0x1CA8) <= iVar14))) {
                    for (iVar12 = 0; iVar12 < 8; iVar12++) {
                        psVar3 = FP(gGT + iVar12 * 4, 0x24EC);
                        if ((psVar3 != 0) && ((FW(psVar3, 0x2C8) & 0x2000000) == 0)) {
                            FW(psVar3, 0x2C8) = FW(psVar3, 0x2C8) | 0x2000000;
                            FB(psVar3, 0x36) = 0xf;
                            if ((FW(psVar3, 0x2C8) & 0x100000) == 0) {
                                func_80064568(psVar3, 2, psVar3, 0);
                                FB(psVar3, 0x560 + FB(psVar3, 0x4A)) = FB(psVar3, 0x560 + FB(psVar3, 0x4A)) - 1;
                                FB(psVar3, 0x50C + FB(psVar3, 0x4A)) = FB(psVar3, 0x50C + FB(psVar3, 0x4A)) - 1;
                            }
                        }
                    }
                    func_8003A3FC();
                }
            }
            return;
        }
    } while (1);
}
