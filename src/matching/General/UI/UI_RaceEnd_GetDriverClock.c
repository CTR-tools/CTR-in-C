/*
 * UI_RaceEnd_GetDriverClock @ 0x8005572C (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 300, Branch 0 / Stack 0, Register 0 — a pure
 * gcc-2.8.1 instruction-scheduling wall (every instruction present with the correct
 * registers; only the emission order differs, which natural C can't steer). Cleaner
 * AND lower-scoring than the prior aibox seed (375, Reg 15) which used a do{}while(0)
 * pointer-advance wrapper + a divisor memory round-trip — both removed here (natural).
 * From Y:\aibox2 staging. vs SCUS_944.26.exe.
 *
 * Finalizes a driver's end-of-race stats (once, gated by ACTION_RACE_TIMER_FROZEN):
 *   - average speed = distanceDriven*100 / timeElapsedInRace
 *   - missile/attack ratio: <4 launches -> -1, else (numTimesAttacking<<12)/numTimesMissileLaunched
 *   - numTimesAttacked = sum of numTimesAttackedByPlayer[0..7]
 *   - if first place (driverRank==0): TimeWinningDriverSpentLastPlace = timeSpentInLastPlace
 *
 * saphi UI_RaceEnd_GetDriverClock (game/UI/UI_RaceFlow.c:4) audited CLEAN vs this asm + Ghidra.
 */
typedef unsigned int uint;
typedef unsigned char byte;

void UI_RaceEnd_GetDriverClock(int driver)
{
    int timeElapsed;
    int numTimesAttacked;
    int i;
    int p;

    if ((*(uint *)(driver + 0x2C8) & 0x40000) == 0)   /* ACTION_RACE_TIMER_FROZEN */
    {
        timeElapsed = *(int *)(driver + 0x514);        /* timeElapsedInRace */
        *(uint *)(driver + 0x2C8) |= 0x40000;
        if (timeElapsed != 0)
            *(int *)(driver + 0x518) =                 /* distanceDriven -> average speed */
                (*(int *)(driver + 0x518) * 100) / timeElapsed;

        if (*(byte *)(driver + 0x55C) < 4)             /* numTimesMissileLaunched < 4 */
            *(int *)(driver + 0x574) = -1;             /* NumMissilesComparedToNumAttacks */
        else
            *(int *)(driver + 0x574) =
                ((int)((uint)*(byte *)(driver + 0x559) << 0xC))   /* numTimesAttacking << 12 */
                    / (int)(uint)*(byte *)(driver + 0x55C);       /* / numTimesMissileLaunched */

        numTimesAttacked = 0;
        for (i = 0; i < 8; i++) {                       /* numTimesAttackedByPlayer[0..7] */
            p = driver + i;
            numTimesAttacked += (uint)*(byte *)(p + 0x560);
        }
        *(int *)(driver + 0x57C) = numTimesAttacked;    /* numTimesAttacked */

        if (*(short *)(driver + 0x482) == 0)            /* driverRank == 0 (first place) */
            *(int *)(driver + 0x578) =                  /* TimeWinningDriverSpentLastPlace */
                *(int *)(driver + 0x528);               /*   = timeSpentInLastPlace */
    }
}
