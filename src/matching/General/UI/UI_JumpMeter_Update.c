/*
 * UI_JumpMeter_Update @ 0x80051C64 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 480, Branch 0 / Stack 0, Register 8 —
 * a genuine gcc-2.8.1 register-allocation wall. Finished from aibox staging
 * (removed a behavior-neutral do{}while(0) wrapper + dead empty statements;
 * the seed was already behaviorally correct — NOT a false-friend).
 *
 * Per-frame jump/charge-meter update, keyed on airborne state:
 *  - airborne now (actionsFlagSet & ACTION_AIRBORNE 0x80000):
 *      if jump_LandingBoost(0x3FC) > 0x480 and jumpMeter(0x46) < 0x481, request a
 *      landing voice line (Voiceline table D_80086E84[driverID(0x4A)]); set
 *      jumpMeter = jump_LandingBoost (cap 0x960); reset jumpMeterTimer(0x48) = 0x5A0.
 *  - was airborne last frame (actionsFlagSetPrevFrame 0x2CC & 0x80000):
 *      if jumpMeter > 0x150, timeSpentJumping(0x53C) += jumpMeter; track
 *      highestJump(0x550) = max; and at meter tiers 0x5A0/0x3C0/0x280 grant turbo
 *      reserves via VehFire_Increment(driver, 0x3C0, 2, 0x100/0x80/0).
 *  - neither: decay jumpMeterTimer by gGT->elapsedTimeMS (gGT+0x1D04), clamp >= 0;
 *      when the timer reaches 0, reset jumpMeter = 0.
 *
 * saphi UI_Meter.c UI_JumpMeter_Update audited CLEAN vs this asm + Ghidra
 * (saphi's nested if(!airborne){...}else form is logically identical to the flat
 * if/else-if here; all fields/constants match).
 */
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned short undefined2;
typedef unsigned char byte;

extern char *D_8008D2AC;                                   /* gGT */
extern short D_80086E84;                                    /* characterIDs[] voice-line table */
extern void func_8005ABFC(int driver, int p2, int p3, int p4);  /* VehFire_Increment */
extern void func_8002CBE8(int p1, int p2, int p3);              /* Voiceline_RequestPlay */

void UI_JumpMeter_Update(int driver)
{
    short landingBoost;
    ushort timerRemain;
    int jumpMeter;

    if ((*((uint *) (driver + 0x2C8)) & 0x80000) != 0)                 /* actionsFlagSet & ACTION_AIRBORNE */
    {
        if ((0x480 < (*((short *) (driver + 0x3FC)))) &&               /* jump_LandingBoost */
            ((*((short *) (driver + 0x46))) < 0x481))                  /* jumpMeter */
        {
            func_8002CBE8(7, (int) ((short) (&D_80086E84)[*((byte *) (driver + 0x4A))]), 0x10);  /* [driverID] */
        }
        *((short *) (driver + 0x46)) = (landingBoost = *((short *) (driver + 0x3FC)));
        if (0x960 < landingBoost)
        {
            *((undefined2 *) (driver + 0x46)) = 0x960;
        }
        *((undefined2 *) (driver + 0x48)) = 0x5A0;                     /* jumpMeterTimer */
    }
    else if ((*((uint *) (driver + 0x2CC)) & 0x80000) != 0)           /* actionsFlagSetPrevFrame & ACTION_AIRBORNE */
    {
        jumpMeter = (int) (*((short *) (driver + 0x46)));
        if (0x150 < jumpMeter)
        {
            *((int *) (driver + 0x53C)) = (*((int *) (driver + 0x53C))) + jumpMeter;   /* timeSpentJumping */
        }
        if ((*((short *) (driver + 0x550))) < ((int) (*((short *) (driver + 0x46)))))  /* highestJump */
        {
            *((undefined2 *) (driver + 0x550)) = *((undefined2 *) (driver + 0x46));
        }
        landingBoost = *((short *) (driver + 0x46));
        if (0x5A0 <= landingBoost)
        {
            func_8005ABFC(driver, 0x3C0, 2, 0x100);
        }
        else if (0x3C0 <= landingBoost)
        {
            func_8005ABFC(driver, 0x3C0, 2, 0x80);
        }
        else if (0x280 <= landingBoost)
        {
            func_8005ABFC(driver, 0x3C0, 2, 0);
        }
    }
    else
    {
        if ((*((short *) (driver + 0x48))) == 0)
        {
            *((undefined2 *) (driver + 0x46)) = 0;
        }
        else
        {
            timerRemain = *((ushort *) (driver + 0x48));
            jumpMeter = ((uint) timerRemain) - ((uint) (*((ushort *) (D_8008D2AC + 0x1D04))));  /* elapsedTimeMS */
            *((undefined2 *) (driver + 0x48)) = (short) jumpMeter;
            if ((jumpMeter * 0x10000) < 0)
            {
                *((undefined2 *) (driver + 0x48)) = 0;
            }
        }
    }
}
