/*
 * UI_CupStandings_UpdateCupRanks @ 0x80056220 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 1210, Branch 0 / Stack 0, Register 42 — a genuine
 * gcc-2.8.1 register-allocation/constant-hoist wall. From the "aibox" prior grind set.
 *
 * ⚠️ The aibox seed scored a deceptively-low 1160 by being BEHAVIORALLY WRONG: it reused
 * the driver-count local as a scratch temp inside the inner loop (`iVar2 = (short)uVar7`),
 * corrupting the OUTER loop bound `while (rankSlot < iVar2)` — so it would emit the wrong
 * number of ranks. (Branch/Stack 0 hid it: the branch structure is unchanged, only the
 * compared VALUE is wrong.) The permuter re-derives the same bug (a 1100 candidate does
 * `(numDrivers = (int)bestScore) <= ...`) — rejected per the correctness-over-score mandate.
 * This delivery uses a SEPARATE invariant `numDrivers` bound (matches retail $t2 + saphi).
 *
 * Selection-sort ranking: for each rank slot, scan drivers high-index→low, pick the
 * highest cup.points[] (@gGT+0x1E60, int) not already assigned (assignedMask bit), and
 * write its index to data.cupPositionPerPlayer[] (D_80086D44). assignedMask persists
 * across rank slots; bestScore/bestIndex reset each slot. numDrivers = numPlyrCurrGame
 * (@0x1CA8) + numBotsNextGame (@0x1CAB). Score read as (u16) for update, (s16) for compare.
 *
 * saphi UI_CupStandings_UpdateCupRanks (game/UI/UI_CupStandings.c) audited CLEAN vs this
 * asm + Ghidra — it uses the correct invariant numDrivers bound, i.e. it does NOT have the
 * seed's loop-bound-corruption bug.
 */

typedef unsigned int uint;
typedef unsigned char byte;

extern char *D_8008D2AC;   /* sdata->gGT */
extern int D_80086D44;     /* data.cupPositionPerPlayer[0] */

void UI_CupStandings_UpdateCupRanks(void)
{
    char *gGT = D_8008D2AC;
    int numDrivers = (int)(byte)gGT[0x1CAB] + (int)(byte)gGT[0x1CA8];
    uint assignedMask = 0;
    int rankSlot;

    if (numDrivers == 0)
        return;

    rankSlot = 0;
    do {
        short bestScore = 0;
        int bestIndex = -1;
        int driverIndex = numDrivers - 1;
        if (driverIndex >= 0) {
            do {
                if (((int)bestScore <= *((int *)(gGT + driverIndex * 4 + 0x1E60))) &&
                    ((((int)(assignedMask & 0xFF)) >> driverIndex) & 1) == 0) {
                    bestScore = *((unsigned short *)(gGT + driverIndex * 4 + 0x1E60));
                    if ((short)bestIndex != -1)
                        assignedMask = assignedMask & ~(1 << bestIndex);
                    bestIndex = driverIndex;
                    assignedMask = assignedMask | (1 << driverIndex);
                }
                driverIndex--;
            } while (driverIndex >= 0);
        }
        (&D_80086D44)[rankSlot] = (short)bestIndex;
        rankSlot++;
    } while (rankSlot < numDrivers);
}
