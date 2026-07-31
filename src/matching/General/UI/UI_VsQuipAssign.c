/*
 * UI_VsQuipAssign @ 0x80054A78 (func_80054A78, SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact: base score 90, Branch 0 / Stack 0, Register 18 residual.
 * The 90 is EXACTLY 18*5 — a pure register-allocation wall, no reordering or
 * inserted/deleted instructions. vs SCUS_944.26.exe; finished from the
 * "near completed" prior grind set (this base.c had NO false-friend corruption —
 * behavior verified 1:1 against the retail asm and the ASM-verified saphi source).
 *
 * Picks the highest-priority quip (voice line) from a meta list for a driver:
 *   - Skip if driver is NULL, or if the quip set is "self-only" ((meta->flags & 4)
 *     != 0) and this driver isn't the bestDriver.
 *   - Scan meta->quips[0..n): keep the max-priority entry; on a tie, switch with
 *     ~25% probability ((MixRNG_Scramble() >> 3) & 0xff < 0x40).
 *   - If the driver already has an active quip, only override when the new priority
 *     strictly beats it, or ties AND wins a second RNG roll (<= 0x3f); restore the
 *     old quip's priority sign first.
 *   - Negate the chosen quip's priority (marks it active/consumed), store it and the
 *     characterID on the driver, and arm the VS-quip display timer.
 *
 * Field map (from asm + saphi game/UI/UI_VsQuip.c, ASM-verified):
 *   meta:  uint[0]=ptrQuipStrCurr, uint[1]=ptrQuipStrNext, +0x0A: s16 flags (bit 4)
 *   QuipStr stride 8, +4 = s32 priority
 *   driver + 0x56C = EndOfRaceComment_ptrQuip (current quip)
 *   driver + 0x570 = EndOfRaceComment_characterID
 *   D_8008D2AC     = sdata->gGT; gGT + 0x00 = gameMode1 (bit 0x20 = BATTLE_MODE)
 *   gGT + 0x1D36   = timerEndOfRaceVS (s16): 0x96 (150) battle, 0x12C (300) otherwise
 *   func_8003EA28  = MixRNG_Scramble
 *
 * Phase B (saphi bug-check): CLEAN — game/UI/UI_VsQuip.c:UI_VsQuipAssign is
 * behaviorally identical (verified vs asm twice).
 */

typedef unsigned int uint;
typedef unsigned short ushort;

extern char *D_8008D2AC;              /* sdata->gGT */
extern int func_8003EA28(void);       /* MixRNG_Scramble */

void UI_VsQuipAssign(int driver, uint *meta, int bestDriver, int characterID)
{
    char *gGT;
    ushort timer;
    int rngBucket;
    uint curr;
    int oldPriority;
    uint selected;

    if ((driver != 0) &&
        ((((*((ushort *) (((int) meta) + 10))) & 4) == 0) || (driver == bestDriver)))
    {
        selected = *meta;
        curr = selected + 8;
        if (curr < meta[1])
        {
            do
            {
                rngBucket = 0x40;
                if (((*((int *) (selected + 4))) < (*((int *) (curr + 4)))) ||
                    (((*((int *) (curr + 4))) == (*((int *) (selected + 4)))) &&
                     ((oldPriority = func_8003EA28(), ((unsigned char) (oldPriority >> 3)) < rngBucket))))
                {
                    selected = curr;
                }
                curr = curr + 8;
            } while (curr < meta[1]);
        }
        if ((*((int *) (driver + 0x56C))) != 0)
        {
            oldPriority = *((int *) ((*((int *) (driver + 0x56C))) + 4));
            if ((*((int *) (selected + 4))) <= oldPriority)
            {
                if ((*((int *) (selected + 4))) != oldPriority)
                {
                    return;
                }
                oldPriority = func_8003EA28();
                rngBucket = (unsigned char) (oldPriority >> 3);
                if (rngBucket > 0x3F)
                {
                    return;
                }
            }
            oldPriority = *((int *) (driver + 0x56C));
            if ((oldPriority != 0) && ((*((int *) (oldPriority + 4))) < 0))
            {
                *((int *) (oldPriority + 4)) = -(*((int *) (oldPriority + 4)));
            }
        }
        if (0 < (*((int *) (selected + 4))))
        {
            *((int *) (selected + 4)) = -(*((int *) (selected + 4)));
        }
        gGT = D_8008D2AC;
        *((uint *) (driver + 0x56C)) = selected;
        *((int *) (driver + 0x570)) = characterID;
        timer = 0x96;
        if (((*((uint *) gGT)) & 0x20) == 0)
        {
            timer = 300;
        }
        *((ushort *) (gGT + 0x1D36)) = timer;
    }
}
