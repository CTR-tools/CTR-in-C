/*
 * UI_DrawRaceClock @ 0x8004EDAC (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg404, score 32720 — the BIGGEST UI fn at 698 insns; residual
 * is the register-COLORING wall: `driver` colors to a3 vs retail s8 and the whole s-register file
 * renumbers, cascading across the 16 time-divisions, the lap loop, and the relic block. Non-behavioral
 * — every division idiom, block, and global store is verified. Block-order reduction applied: the
 * RELIC/LAP-loop swap brought it 40775→32720.) Real types. (Previously ⏸ monster, Br/St-0 staged.)
 *
 * The main race-time HUD/results clock (in-race timer + TT/relic results). Returns if driver==NULL.
 * flags: 0=in-race vs TT-TOTAL results; 1=relic/show-laps gate; 2=flash TOTAL; 3/4/5=flash lap 1/2/3.
 * Decomposes driver->timeElapsedInRace into MM:SS:cc digits (÷0x8ca00 10min, 0xe100 1min, 0x2580 10s,
 * 0x3c0 1s, 0x60 tenths; %10/%6; centisecond via the explicit 64-bit ×0x66666667), clamped to
 * 99:59:99 (or 9:59:99 when numLaps<7). Draws a header label (TIME/TIME TRIAL/TOTAL/YOUR TIME), the
 * main time string (g_szTimeStringMMSScc or g_szRaceClockStr), then either the relic goal-medal
 * (SAPPHIRE 0xc6 / GOLD 199 / PLATINUM 200 from g_abAdvProgress bits at levelID +0x16/+0x28/+0x3a, with
 * the NEW_RELIC=0x2000000 branch) + relic time, or the per-lap breakdown (Arcade/TT/Adv): per completed
 * lap UI_SaveLapTime fills g_lapTimes, then draws the "Ln"/lap-number label + that lap's time from the
 * g_lapTimes digit planes.
 *
 * NOTE(claude) vs decomp UI_23 (926 authoritative): the retail lays the RELIC block before the LAP
 * loop (inverted `if (relic && !(flags&2))`). g_lapTimes = struct LapTimes @ D_8009AC00 (5×char[0x38]
 * digit planes, indexed driverID*7+lap). Strings g_szTimeStringMMSScc@D_8008D504 / g_szRaceClockStr@
 * D_8008D50C / g_szLn@D_8008D510; fonts g_awFontCharPixHeight/Width @ D_80082360/68; relic-time globals
 * gp-rel @0xA44/4C/68/74/7C (shared with InitAll). DecalFont=func_80022878, UI_SaveLapTime=func_8004C55C.
 *
 * Phase B (saphi game/UI/UI_Clock.c UI_DrawRaceClock): CLEAN — no bugs. Time-decompose matches (the
 * goto-setRestOfTime structure; every digit formula, incl. (t*10)/0x3c0 == t/0x60; the 99:59:99-default
 * vs my 9:59:99 numLaps<7 clamp is display-equivalent as RaceClockStr never shows the tens digit), and
 * the relic-medal reward-bit logic matches exactly across both NEW_RELIC branches + the medal select.
 */
#include "../uitypes.h"

#define TIME_TRIAL     0x20000
#define RELIC_RACE     0x4000000
#define ARCADE_MODE    0x400000
#define ADVENTURE_MODE 0x80000
#define NEW_RELIC      0x2000000
#define FONT_SMALL 2
#define FONT_BIG   1

extern struct GameTracker *D_8008D2AC;   /* sdata_gGT */
extern char **D_8008D878;                /* sdata_lngStrings */
extern char D_8008D504[];                /* g_szTimeStringMMSScc "MM:SS:cc" */
extern char D_8008D50C[];                /* g_szRaceClockStr "M:SS:cc" */
extern char D_8008D510[];                /* g_szLn "Ln" */
extern char D_800116D4;                  /* g_szFormatInt ("%d") */
extern short D_80082360[];               /* g_awFontCharPixHeight */
extern short D_80082368[];               /* g_awFontCharPixWidth */
extern unsigned int D_8008FBA4[];        /* g_abAdvProgress */
extern struct LapTimes D_8009AC00;       /* g_lapTimes */

/* relic-time gp-rel globals (defined =0 so patch_gprel resolves; shared with InitAll) */
int D_8008D9B0 = 0;   /* gp+0xA44 g_nRelicTime_1min */
int D_8008D9B8 = 0;   /* gp+0xA4C g_nRelicTime_1sec */
int D_8008D9D4 = 0;   /* gp+0xA68 g_nRelicTime_10sec */
int D_8008D9E0 = 0;   /* gp+0xA74 g_nRelicTime_1ms */
int D_8008D9E8 = 0;   /* gp+0xA7C g_nRelicTime_10ms */

extern void func_80022878(char *s, short x, short y, int font, int color); /* DecalFont_DrawLine */
extern void func_8004C55C(int lap, int time, int driverID);                /* UI_SaveLapTime */
extern int  func_80078348(char *buf, char *fmt, int arg);                  /* sprintf */

void UI_DrawRaceClock(unsigned short paramX, unsigned short paramY, unsigned int flags, struct Driver *driver)
{
    unsigned int uLapIndex;
    unsigned int uRewardBit;
    int iCenti100;
    char *str;
    int levelId;
    unsigned short posX;
    short sTimeX;
    unsigned short posY;
    short sTimeY;
    int font;
    int timeElapsedInRace;
    char cCentiOnes;
    unsigned short posX_00;
    short *pFontHeight;
    char cCentiTens;
    char cSecOnes;
    char cSecTens;
    unsigned short uJustify;
    unsigned short uLapFlags;
    char cMinOnes;
    char cMinTens;
    int iScratchIdx;
    int iLapNum;
    char acStack_50[8];
    unsigned short baseX;
    unsigned short baseY;
    char *lapNumStr;
    int lapTextX;
    int lapTextXFixed;
    int iCentiHi;

    iScratchIdx = 0;
    cMinTens = 0;
    if (driver == 0) {
        return;
    }
    if (D_8008D2AC->numLaps == 7) {
        timeElapsedInRace = driver->timeElapsedInRace;
        iScratchIdx = timeElapsedInRace / 0x8ca00;
        if (9 < iScratchIdx) {
            cMinTens = 9;
            goto LAB_8004ef58;
        }
        iLapNum = timeElapsedInRace / 0xe100 + iScratchIdx * -10;
        cMinTens = (char)(timeElapsedInRace / 0x2580);
        cSecTens = cMinTens + (char)(timeElapsedInRace / 0xe100) * -6;
        iCenti100 = (timeElapsedInRace * 100) / 0x3c0;
        iCentiHi = (int)((unsigned long long)((long long)iCenti100 * 0x66666667) >> 0x20);
        cCentiTens = (char)(timeElapsedInRace / 0x3c0);
        cSecOnes = cCentiTens + cMinTens * -10;
        cCentiTens = (char)(timeElapsedInRace / 0x60) + cCentiTens * -10;
        goto LAB_8004f02c;
LAB_8004ef58:
        cMinOnes = 9;
        cSecTens = 5;
        cSecOnes = 9;
        cCentiTens = 9;
        cCentiOnes = 9;
    } else {
        timeElapsedInRace = driver->timeElapsedInRace;
        iLapNum = timeElapsedInRace / 0xe100;
        if (9 < iLapNum) goto LAB_8004ef58;
        cMinTens = (char)(timeElapsedInRace / 0x2580);
        cSecTens = cMinTens + (char)iLapNum * -6;
        iCenti100 = (timeElapsedInRace * 100) / 0x3c0;
        iCentiHi = (int)((unsigned long long)((long long)iCenti100 * 0x66666667) >> 0x20);
        cCentiTens = (char)(timeElapsedInRace / 0x3c0);
        cSecOnes = cCentiTens + cMinTens * -10;
        cCentiTens = (char)(timeElapsedInRace / 0x60) + cCentiTens * -10;
LAB_8004f02c:
        cMinOnes = (char)iLapNum;
        cMinTens = (char)iScratchIdx;
        cCentiOnes = (char)iCenti100 + ((char)(iCentiHi >> 2) - (char)(iCenti100 >> 0x1f)) * -10;
    }
    if ((flags & 1) == 0) {
        iScratchIdx = 0x12;
        if ((D_8008D2AC->gameMode1 & TIME_TRIAL) != 0) {
            iScratchIdx = 0x4d;
        }
        font = FONT_SMALL;
        uJustify = 0;
    } else {
        iScratchIdx = 0xc4;
        if ((D_8008D2AC->gameMode1 & RELIC_RACE) != 0) {
            iScratchIdx = 0xc5;
        }
        font = FONT_BIG;
        if (((flags & 4) == 0) || (uJustify = 0x4004, (D_8008D2AC->timer & 2) != 0)) {
            uJustify = 0x4000;
        }
    }
    baseX = paramX;
    baseY = paramY;
    func_80022878(D_8008D878[iScratchIdx], paramX, paramY, font, uJustify);
    uJustify = 1;
    if ((D_8008D2AC->frozenTimeRemaining == 0) && (uJustify = 0, (flags & 4) != 0)) {
        uJustify = (unsigned short)((D_8008D2AC->timer & 2) == 0) << 2;
    }
    if (D_8008D2AC->numLaps == 7) {
        str = D_8008D504;
        D_8008D504[0] = cMinTens + '0';
        D_8008D504[1] = cMinOnes + '0';
        D_8008D504[3] = cSecTens + '0';
        D_8008D504[4] = cSecOnes + '0';
        D_8008D504[6] = cCentiTens + '0';
        D_8008D504[7] = cCentiOnes + '0';
    } else {
        str = D_8008D50C;
        D_8008D50C[0] = cMinOnes + '0';
        D_8008D50C[2] = cSecTens + '0';
        D_8008D50C[3] = cSecOnes + '0';
        D_8008D50C[5] = cCentiTens + '0';
        D_8008D50C[6] = cCentiOnes + '0';
    }
    if ((flags & 1) == 0) {
        posY = baseY + 8;
        posX = baseX;
    } else {
        posX = baseX + 0x11;
        posY = baseY;
    }
    func_80022878(str, posX, posY, FONT_BIG, uJustify);
    if ((D_8008D2AC->gameMode1 & RELIC_RACE) != 0 && (flags & 2) == 0) {
        if ((D_8008D2AC->gameModeEnd & NEW_RELIC) == 0) {
            levelId = D_8008D2AC->levelID;
            if (((D_8008FBA4[(levelId + 0x3a) >> 5] >> (levelId + 0x3a & 0x1f) & 1) != 0) ||
                ((D_8008FBA4[(levelId + 0x28) >> 5] >> (levelId + 0x28 & 0x1f) & 1) != 0)) {
    LAB_8004f338:
                iScratchIdx = 200;
                uLapFlags = 0x16;
                goto LAB_8004f378;
            }
            uLapIndex = D_8008FBA4[(levelId + 0x16) >> 5] >> (levelId + 0x16 & 0x1f);
        } else {
            uRewardBit = D_8008D2AC->levelID + 0x3a;
            uLapIndex = D_8008D2AC->levelID + 0x28;
            if ((D_8008FBA4[(int)uRewardBit >> 5] >> (uRewardBit & 0x1f) & 1) != 0) goto LAB_8004f338;
            uLapIndex = D_8008FBA4[(int)uLapIndex >> 5] >> (uLapIndex & 0x1f);
        }
        if ((uLapIndex & 1) == 0) {
            iScratchIdx = 0xc6;
            uLapFlags = 0x11;
        } else {
            iScratchIdx = 199;
            uLapFlags = 0xe;
        }
    LAB_8004f378:
        font = FONT_BIG;
        if ((flags & 1) == 0) {
            font = FONT_SMALL;
            sTimeY = baseY + 0x18;
            sTimeX = baseY + 0x20;
            posX_00 = baseX;
        } else {
            uLapFlags = uLapFlags | 0x4000;
            sTimeY = baseY - 0x11;
            sTimeX = sTimeY;
            posX_00 = baseX + 0x11;
        }
        func_80022878(D_8008D878[iScratchIdx], baseX, sTimeY, font, uLapFlags);
        D_8008D50C[0] = D_8008D9B0 + '0';
        D_8008D50C[2] = D_8008D9D4 + '0';
        D_8008D50C[3] = D_8008D9B8 + '0';
        D_8008D50C[5] = D_8008D9E8 + '0';
        D_8008D50C[6] = D_8008D9E0 + '0';
        func_80022878(D_8008D50C, posX_00, sTimeX, FONT_BIG, uLapFlags & 0xbfff);
    } else {
        if ((D_8008D2AC->gameMode1 & (ARCADE_MODE | ADVENTURE_MODE | TIME_TRIAL)) == 0) {
            return;
        }
        uLapIndex = driver->lapIndex;
        iScratchIdx = 0;
        if (uLapIndex == 0xffffffff) {
            return;
        }
        lapNumStr = acStack_50;
        iLapNum = 1;
        lapTextXFixed = (unsigned int)baseX << 0x10;
        lapTextX = lapTextXFixed >> 0x10;
        do {
            if ((iScratchIdx <= (int)uLapIndex) && (iScratchIdx < D_8008D2AC->numLaps)) {
                func_8004C55C(uLapIndex, D_8008D2AC->elapsedEventTime - driver->lapTime,
                              (unsigned short)driver->driverID);
                D_8008D504[0] = ' ';
                D_8008D504[1] = D_8009AC00.pLapDigit_Min1s[(unsigned int)driver->driverID * 7 + iScratchIdx] + '0';
                D_8008D504[3] = D_8009AC00.pLapDigit_Sec10s[(unsigned int)driver->driverID * 7 + iScratchIdx] + '0';
                D_8008D504[4] = D_8009AC00.pLapDigit_Sec1s[(unsigned int)driver->driverID * 7 + iScratchIdx] + '0';
                D_8008D504[6] = D_8009AC00.pLapDigit_Ms10s[(unsigned int)driver->driverID * 7 + iScratchIdx] + '0';
                D_8008D504[7] = D_8009AC00.pLapDigit_Ms1s[(unsigned int)driver->driverID * 7 + iScratchIdx] + '0';
                if (((iScratchIdx == 0) && ((flags & 8) != 0)) ||
                    ((iScratchIdx == 1 && ((flags & 0x10) != 0))) ||
                    ((uLapFlags = 1, iScratchIdx == 2 && ((flags & 0x20) != 0)))) {
                    uLapFlags = ((unsigned short)((unsigned int)D_8008D2AC->timer >> 1) ^ 1) & 1;
                }
                if ((flags & 1) == 0) {
                    if ((D_8008D2AC->gameMode1 & ARCADE_MODE) != 0) goto LAB_8004f84c;
                    D_8008D510[1] = (char)iScratchIdx + '1';
                    sTimeY = (short)(((unsigned int)baseY + iLapNum * 8 + 0x10) << 16 >> 16);
                    func_80022878(D_8008D510, (short)((unsigned int)lapTextXFixed >> 0x10), sTimeY, FONT_SMALL, 3);
                    font = FONT_SMALL;
                    uLapFlags = 1;
                    sTimeX = baseX + 0x1a;
                } else {
                    timeElapsedInRace = 1;
                    if (3 < D_8008D2AC->numLaps) {
                        timeElapsedInRace = 2;
                    }
                    func_80078348(lapNumStr, &D_800116D4, iLapNum);
                    pFontHeight = D_80082360 + timeElapsedInRace;
                    font = timeElapsedInRace;
                    func_80022878(lapNumStr, (short)lapTextX,
                                  (short)(((unsigned int)baseY - (D_8008D2AC->numLaps - iScratchIdx) * (int)*pFontHeight) << 16 >> 16),
                                  font, 0x4003);
                    func_80022878(D_8008D878[0x18], baseX - D_80082368[timeElapsedInRace],
                                  (short)(((unsigned int)baseY - (D_8008D2AC->numLaps - iScratchIdx) * (int)*pFontHeight) << 16 >> 16),
                                  font, 0x4003);
                    sTimeX = (short)lapTextX;
                    sTimeY = (short)(((unsigned int)baseY - (D_8008D2AC->numLaps - iScratchIdx) * (int)*pFontHeight) << 16 >> 16);
                }
                func_80022878(D_8008D504, sTimeX, sTimeY, font, uLapFlags);
            }
LAB_8004f84c:
            uLapIndex = driver->lapIndex;
            iScratchIdx = iScratchIdx + 1;
            iLapNum = iLapNum + 1;
            if ((int)(uLapIndex + 1) <= iScratchIdx) {
                return;
            }
        } while (1);
    }
}