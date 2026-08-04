/*
 * UI_VsQuipAssignAll @ 0x80054BFC (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg107, score 22675 — monster-tier rule engine; residual is a
 * register-COLORING wall across the aging/leader/rule loops: rule/thread/driver/scratchInt/leaderDriver
 * color to different callee-saved regs than retail (s6/s5/s0/s8/s7), cascading through the 10-case
 * jump-table switch. Non-behavioral — the whole structure, every rule case, the folded cursors, the
 * leader-tie carry, and all globals are verified exact.) Real types. (Previously ⏸ deferred monster.)
 *
 * At the end of a multiplayer VS/battle race, assigns each driver an end-of-race "quip" by evaluating a
 * data-driven rule table (g_aVsQuipRulesRace[39] normal / g_aVsQuipRulesAdv[12] when gameMode1&0x20).
 * Only runs for 2+ players. STEPS: (1) AGING — +1 to every candidate line's nWeight across all rules
 * (older/unused lines grow more likely). (2) LEADER detect — scan driver threads, clear each
 * EndOfRaceComment, find leaderDriver = max numLives (or battleSetup.pointsPerTeam[teamID] in team mode,
 * gameMode1&0x4000); leaderTieFlag + perDriverScore[] track ties. (3) Per rule: threshold = rule->
 * threshold (*|numLaps| if flags&1); scan drivers, switch(ruleType) picks the best 'candidate' via
 * UI_VsQuipReadDriver(driver, statIndex1, statIndex2) carried across the loop:
 *   0 strictly-max stat>threshold; 1 same but stat>=0 && >=threshold; 3 biggest-rival (max
 *   numTimesAttackedByPlayer[i]-acc, stores attacker index); 4/5 min-side band variants (5 inits
 *   acc=INT_MAX); 6 stat==numTimesAttacking; 7 leader (thr0) / tied-for-lead (thr1); 8 stat==threshold;
 *   9 default quip for any driver still without one.
 * If flags&0xC assigns mid-loop per match; always assigns the final bestDriver after the loop, all via
 * UI_VsQuipAssignToDriver(driver, rule, leaderDriver, rivalAttackerIndex).
 *
 * NOTE(claude): no decomp .c (stub). pRuleFields (short* = &rule->nFlags) is a folded parallel field
 * cursor kept in sync with `rule` (pRuleFields[-1]=ruleType, *=nFlags, [1]=threshold, [3/5]=statIndex1/2);
 * case 3 walks numTimesAttackedByPlayer[i] via a folded driver base ((int)driver->funcPtrs+i-0x53). Rule
 * table ends computed as base+0x3A8 (39) / base+0x120 (12). g_aVsQuipRulesRace=D_8008664C,
 * g_aVsQuipRulesAdv=D_80086C0C; ReadDriver=func_80054A08, AssignToDriver=func_80054A78.
 *
 * Phase B (saphi game/UI/UI_VsQuip.c UI_VsQuipAssignAll): CLEAN — no bugs. The whole engine matches:
 * aging (curr->priority++ = nWeight++), leader-detect (saphi's if/else-if score>best/==best cleanly
 * reproduces the retail comma-operator leaderDriver/secondScore carry), all 10 switch cases (case 3
 * uses natural numTimesAttackedByPlayer[i] = the retail folded attackerWalk; case 7 bestDriver/
 * secondScore = leaderDriver/leaderTieFlag), the flags&0xC mid-loop assign, the per-driver carry, and
 * the final assign. Divergences (data-blob+offset tables vs direct globals, if/else-if vs comma carries,
 * natural indexing vs folded cursors) are all representational/equivalent.
 */
#include "../uitypes.h"

#define BATTLE_MODE 0x20
#define POINT_LIMIT 0x4000

extern struct GameTracker *D_8008D2AC;   /* sdata_gGT (absolute) */
extern struct VsQuipRule D_8008664C[];   /* g_aVsQuipRulesRace[39] */
extern struct VsQuipRule D_80086C0C[];   /* g_aVsQuipRulesAdv[12] */

extern unsigned int func_80054A08(struct Driver *d, int statIndex1, int statIndex2); /* UI_VsQuipReadDriver */
extern void func_80054A78(struct Driver *d, struct VsQuipRule *rule,
                          struct Driver *leader, int rival);                          /* UI_VsQuipAssignToDriver */

void UI_VsQuipAssignAll(void)
{
    struct Driver *leaderDriver;
    int scratchInt;
    unsigned int stat;
    struct Driver *attackerWalk;
    int *pLineWeight;
    int livesOrPoints;
    unsigned int attackCount;
    struct VsQuipLine *agingLine;
    struct VsQuipLine **pAgingLinesEnd;
    struct VsQuipRule *rule;
    struct Driver *bestDriver;
    struct Driver *driver;
    struct Driver *candidate;
    unsigned int threshold;
    unsigned int thresholdNext;
    short *pRuleFields;
    unsigned int accumulator;
    unsigned int accumulatorNext;
    struct Thread *thread;
    struct VsQuipRule *agingRule;
    int tieScratch;
    int perDriverScore[8];
    struct VsQuipRule *ruleTableEnd;
    int rivalAttackerIndex;
    int cmpFlag;
    struct GameTracker *gGT;
    int leaderTieFlag;

    rivalAttackerIndex = 0;
    if (1 < (unsigned char)D_8008D2AC->numPlyrCurrGame) {
        if ((D_8008D2AC->gameMode1 & BATTLE_MODE) == 0) {
            rule = D_8008664C;
            ruleTableEnd = (struct VsQuipRule *)((char *)D_8008664C + 0x3a8);
        } else {
            rule = D_80086C0C;
            ruleTableEnd = (struct VsQuipRule *)((char *)D_80086C0C + 0x120);
        }
        if (rule < ruleTableEnd) {
            pAgingLinesEnd = &rule->pLinesEnd;
            agingRule = rule;
            do {
                agingLine = agingRule->pLinesStart;
                if (agingLine < *pAgingLinesEnd) {
                    pLineWeight = &agingLine->nWeight;
                    do {
                        agingLine = agingLine + 1;
                        *pLineWeight = *pLineWeight + 1;
                        pLineWeight = pLineWeight + 2;
                    } while (agingLine < *pAgingLinesEnd);
                }
                agingRule = agingRule + 1;
                pAgingLinesEnd = pAgingLinesEnd + 6;
            } while (agingRule < ruleTableEnd);
        }
        gGT = D_8008D2AC;
        leaderTieFlag = 0;
        leaderDriver = 0;
        scratchInt = 0;
        for (thread = D_8008D2AC->threadBucket0Thread; thread != 0; thread = thread->siblingThread) {
            bestDriver = thread->object;
            bestDriver->EndOfRaceComment_ptrQuip = 0;
            if ((gGT->gameMode1 & POINT_LIMIT) == 0) {
                livesOrPoints = bestDriver->BattleHUD.numLives;
            } else {
                livesOrPoints = gGT->pointsPerTeam[bestDriver->BattleHUD.teamID];
            }
            perDriverScore[bestDriver->driverID] = livesOrPoints;
            tieScratch = scratchInt;
            if ((livesOrPoints <= scratchInt) &&
                (cmpFlag = livesOrPoints == scratchInt, livesOrPoints = scratchInt,
                 bestDriver = leaderDriver, tieScratch = leaderTieFlag, cmpFlag)) {
                bestDriver = 0;
                tieScratch = scratchInt;
            }
            leaderTieFlag = tieScratch;
            leaderDriver = bestDriver;
            scratchInt = livesOrPoints;
        }
        pRuleFields = &rule->nFlags;
        if (rule < ruleTableEnd) {
            do {
                accumulator = 0;
                if (pRuleFields[-1] == 5) {
                    accumulator = 0x7fffffff;
                }
                threshold = *(unsigned int *)(pRuleFields + 1);
                if ((*pRuleFields & 1) != 0) {
                    scratchInt = D_8008D2AC->numLaps;
                    if (scratchInt < 0) {
                        scratchInt = -scratchInt;
                    }
                    threshold = threshold * scratchInt;
                }
                bestDriver = 0;
                for (thread = D_8008D2AC->threadBucket0Thread; thread != 0; thread = thread->siblingThread) {
                    driver = thread->object;
                    candidate = bestDriver;
                    thresholdNext = threshold;
                    accumulatorNext = accumulator;
                    switch (pRuleFields[-1]) {
                    case 0:
                        accumulator = func_80054A08(driver, *(int *)(pRuleFields + 3), *(int *)(pRuleFields + 5));
                        candidate = driver;
                        thresholdNext = accumulator;
                        if ((int)accumulator <= (int)threshold) {
LAB_80054f44:
                            candidate = bestDriver;
                            thresholdNext = threshold;
                            if (accumulator == threshold) {
                                candidate = 0;
                            }
                        }
                        break;
                    case 1:
                        accumulator = func_80054A08(driver, *(int *)(pRuleFields + 3), *(int *)(pRuleFields + 5));
                        if ((-1 < (int)accumulator) &&
                            (candidate = driver, thresholdNext = accumulator, (int)threshold <= (int)accumulator)) {
                            goto LAB_80054f44;
                        }
                        break;
                    case 3:
                        attackerWalk = driver;
                        scratchInt = 0;
                        do {
                            attackCount = (unsigned int)(unsigned char)attackerWalk->numTimesAttackedByPlayer[0];
                            if ((int)threshold < (int)(attackCount - accumulator)) {
                                rivalAttackerIndex = scratchInt;
                                bestDriver = driver;
                                accumulator = attackCount;
                            } else if (((int)-threshold < (int)(attackCount - accumulator)) &&
                                       (bestDriver = 0, (int)accumulator < (int)attackCount)) {
                                accumulator = attackCount;
                            }
                            livesOrPoints = scratchInt + 1;
                            attackerWalk = (struct Driver *)((int)driver->funcPtrs + scratchInt + -0x53);
                            scratchInt = livesOrPoints;
                            candidate = bestDriver;
                            accumulatorNext = accumulator;
                        } while (livesOrPoints < 8);
                        break;
                    case 4:
                        stat = func_80054A08(driver, *(int *)(pRuleFields + 3), *(int *)(pRuleFields + 5));
                        candidate = driver;
                        accumulatorNext = stat;
                        if (((int)(stat - accumulator) <= (int)threshold) &&
                            (cmpFlag = (int)accumulator < (int)stat, candidate = bestDriver,
                             accumulatorNext = accumulator, (int)-threshold < (int)(stat - accumulator))) {
LAB_80054fd0:
                            candidate = 0;
                            accumulatorNext = accumulator;
                            if (cmpFlag) {
                                accumulatorNext = stat;
                            }
                        }
                        break;
                    case 5:
                        stat = func_80054A08(driver, *(int *)(pRuleFields + 3), *(int *)(pRuleFields + 5));
                        if (((-1 < (int)stat) &&
                             (candidate = driver, accumulatorNext = stat,
                              (int)(accumulator - stat) <= (int)threshold)) &&
                            (cmpFlag = (int)stat < (int)accumulator, candidate = bestDriver,
                             accumulatorNext = accumulator, (int)-threshold < (int)(accumulator - stat))) {
                            goto LAB_80054fd0;
                        }
                        break;
                    case 6:
                        accumulator = func_80054A08(driver, *(int *)(pRuleFields + 3), *(int *)(pRuleFields + 5));
                        if (accumulator == (unsigned char)driver->numTimesAttacking) {
                            candidate = driver;
                        }
                        break;
                    case 7:
                        if (threshold == 0) {
                            if (driver == leaderDriver) {
                                candidate = leaderDriver;
                            }
                        } else if ((threshold == 1) && (leaderTieFlag != 0) &&
                                   (perDriverScore[driver->driverID] == leaderTieFlag)) {
                            candidate = driver;
                        }
                        break;
                    case 8:
                        accumulator = func_80054A08(driver, *(int *)(pRuleFields + 3), *(int *)(pRuleFields + 5));
                        if (accumulator == threshold) {
                            candidate = driver;
                        }
                        break;
                    case 9:
                        if (driver->EndOfRaceComment_ptrQuip == 0) {
                            func_80054A78(driver, rule, leaderDriver, 0);
                        }
                    }
                    if ((candidate != 0) && ((*pRuleFields & 0xc) != 0)) {
                        func_80054A78(candidate, rule, leaderDriver, rivalAttackerIndex);
                    }
                    bestDriver = candidate;
                    threshold = thresholdNext;
                    accumulator = accumulatorNext;
                }
                func_80054A78(bestDriver, rule, leaderDriver, rivalAttackerIndex);
                rule = rule + 1;
                pRuleFields = pRuleFields + 0xc;
            } while (rule < ruleTableEnd);
        }
    }
}
