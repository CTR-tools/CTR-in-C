/*
 * UI_SaveLapTime @ 0x8004C55C (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 540, Branch 0 / Stack 0, Register 0. The residual
 * is a pure gcc-2.8.1 instruction-scheduling wall — retail materialises the %hi/%lo of
 * the digit table (p1_Min1s) AFTER computing the player-lap index, whereas gcc-from-C
 * hoists it right after the leading multiply. A 180-iter permuter grind never beat 540,
 * and manual reordering only regresses it. No hacks. From `Y:\aibox2 staging`.
 *
 * Decomposes lapTime (in 1/57600-min ticks... i.e. NTSC 1/960 s units) into the
 * per-digit slot tables indexed by playerLapIndex = driverID*7 + numLaps (0..0xD;
 * 0-6 = player-1 laps 1-7, 7-0xD = player-2). Each table is int[]; the byte offset is
 * playerLapIndex*4. If minutes exceed 9 the display is rigged to 9:59.99.
 *
 * Digit tables (== saphi sdata->LapTimes.*):
 *   D_8009AC00 = p1_Min1s, D_8009ACA8 = p1_Sec10s, D_8009ACE0 = p1_Sec1s,
 *   D_8009AC38 = p1_Ms10s,  D_8009AC70 = p1_Ms1s.
 *
 * saphi UI_SaveLapTime (game/UI/UI_Reward.c:9) audited CLEAN vs this asm + Ghidra:
 *   Sec10s=(lapTime/0x2580)%6, Sec1s=(lapTime/0x3C0)%10, Ms10s=(lapTime*10/0x3C0)%10,
 *   Ms1s=(lapTime*100/0x3C0)%10 — all equivalent to the retail div-magic forms below
 *   (0xE100=6*0x2580, 0x2580=10*0x3C0). No bug.
 */

extern char D_8009AC00;   /* LapTimes.p1_Min1s[]  */
extern char D_8009AC38;   /* LapTimes.p1_Ms10s[]  */
extern char D_8009AC70;   /* LapTimes.p1_Ms1s[]   */
extern char D_8009ACA8;   /* LapTimes.p1_Sec10s[] */
extern char D_8009ACE0;   /* LapTimes.p1_Sec1s[]  */

void UI_SaveLapTime(int numLaps, int lapTime, short driverID)
{
    int digitOffset;
    int playerLapIndex;
    int sec10sScaled;

    *((int *) ((&D_8009AC00) + (((((int) driverID) * 7) + numLaps) * 4))) = lapTime / 0xE100;
    digitOffset = ((((int) driverID) * 7) + numLaps) * 4;

    if (9 < (lapTime / 0xE100))
    {
        /* minutes overflow -> rig display to 9:59.99 */
        playerLapIndex = (((int) driverID) * 7) + numLaps;
        *((int *) ((&D_8009AC00) + (playerLapIndex * 4))) = 9;   /* Min1s  = 9 */
        *((int *) ((&D_8009ACA8) + digitOffset)) = 5;            /* Sec10s = 5 */
        *((int *) ((&D_8009ACE0) + digitOffset)) = 9;            /* Sec1s  = 9 */
        *((int *) ((&D_8009AC38) + digitOffset)) = 9;            /* Ms10s  = 9 */
        *((int *) ((&D_8009AC70) + digitOffset)) = 9;            /* Ms1s   = 9 */
        return;
    }

    sec10sScaled = lapTime / 0x2580;
    *((int *) ((&D_8009ACA8) + digitOffset)) = sec10sScaled + ((lapTime / 0xE100) * (-6));                  /* Sec10s = (lapTime/0x2580)%6  */
    *((int *) ((&D_8009ACE0) + digitOffset)) = (lapTime / 0x3C0) + (sec10sScaled * (-10));                  /* Sec1s  = (lapTime/0x3C0)%10   */
    *((int *) ((&D_8009AC38) + digitOffset)) =
        (((((lapTime / 6) + (lapTime >> 0x1F)) >> 2) >> 2) - (lapTime >> 0x1F)) + ((lapTime / 0x3C0) * (-10)); /* Ms10s = (lapTime*10/0x3C0)%10 */
    *((int *) ((&D_8009AC70) + digitOffset)) = ((lapTime * 100) / 0x3C0) % 10;                              /* Ms1s   = (lapTime*100/0x3C0)%10 */
}
