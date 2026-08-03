/* Timer_GetTime_Total @ 0x8004B3A4 — total ms = (g_nRcntTotalUnits + GetRCnt(RCnt2)) * 1000 / 0x147E.
 * If the live counter just rolled over (< 100) re-read the accumulator to catch the ISR update. */
extern int func_80077BE4(unsigned int spec);  /* GetRCnt */
int D_8008D988 = 0;                            /* g_nRcntTotalUnits (gp+0xA1C) */

int Timer_GetTime_Total(void)
{
    int rcntTotal = D_8008D988;
    int rcnt = func_80077BE4(0xf2000001);
    int sysClock = rcntTotal + rcnt;
    if (rcnt < 100) {
        sysClock = D_8008D988;
        sysClock = sysClock + rcnt;
    }
    return (sysClock * 1000) / 0x147e;
}
