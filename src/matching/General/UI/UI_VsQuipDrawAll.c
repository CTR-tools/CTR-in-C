/*
 * UI_VsQuipDrawAll @ 0x800550F4 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg41, score 2185 — register-allocation cascade + retail
 * hoisting the constant 3 into a callee-saved reg; call-heavy). Real types.
 *
 * Draws each racer's post-race "quip" comment bubble on the VS results screen. Loops the driver
 * threads (threadBuckets[0]); per thread index i, skips players who already pressed X
 * (g_abBattleEndPressX[i] & 2). For each driver reads EndOfRaceComment_ptrQuip (@0x56C) — a short[]
 * {stringIndex, configBits, ...}; zeroes [2]/[3] (one word). If the conjoined bit (config & 1) is set,
 * sprintf-combines lngStrings[stringIndex] (a %s format) with another character's name
 * (lngStrings[g_anMetaDataCharacters[EndOfRaceComment_characterID@0x570].name_LNG_long]); else uses
 * lngStrings[stringIndex] directly. Draws centered in the player's viewport (tileView[i].rect:
 * x + w/2, y + h/8; stride 0x110/thread) via RECTMENU_DrawQuip(..., 0, 3, -0x8000, 4).
 *
 * NOTE(claude) vs in-repo decomp (REFERENCE; 926 asm authoritative): indexes the pressed-X array and
 * the viewport rect by THREAD ORDER (i, walking pointer), not driverID; sprintf writes a STACK buffer
 * (not 0x1F800000). Levers: single `*(int*)&quip[2]=0` word clear; two RECTMENU_DrawQuip calls
 * (per-branch, tail-merged); invert conjoined-if (conjoined = fall-through, beqz→plain). The pushBuffer
 * struct uses element-base 0x168 (rect@0x1C, matrix@0x28, ptrOT@0xF4; array base gGT+0x168) — this
 * function reads rect via &pushBuffer[0].rectH (abs 0x18A) and strides +0x88 shorts (0x110/thread).
 * All absolute field addresses are preserved and every pushBuffer user re-verified Br/St 0.
 * Phase B (saphi): see note below.
 */
#include "../uitypes.h"

extern struct GameTracker *D_8008D2AC;      /* gGT (absolute) */
extern char **D_8008D878;                   /* sdata->lngStrings */
extern char D_8008D564[];                   /* g_abBattleEndPressX (per-thread pressed-X flags) */
extern struct MetaDataCharacter D_80086D84[];/* g_anMetaDataCharacters */
extern void func_8004568C(char *s, short x, short y, int a3, int a4, int a5, int a6); /* RECTMENU_DrawQuip */
extern int func_80078348(char *buf, char *fmt, char *arg);  /* sprintf */

void UI_VsQuipDrawAll(void)
{
    struct Thread *thread;
    short *psVar2;
    struct Driver *driver;
    unsigned short *puVar1;
    int iVar5, iVar3;
    short sVar6;
    char acStack_a0[128];

    thread = D_8008D2AC->threadBucket0Thread;
    sVar6 = 0;
    if (thread != 0) {
        psVar2 = (short *)&D_8008D2AC->pushBuffer[0].rectH;
        do {
            iVar5 = (int)psVar2[-3] + ((int)((unsigned int)(unsigned short)psVar2[-1] << 0x10) >> 0x11);
            iVar3 = (int)psVar2[-2] + ((int)((unsigned int)(unsigned short)psVar2[0] << 0x10) >> 0x13);
            driver = thread->object;
            if ((D_8008D564[sVar6] & 2) == 0 &&
                (puVar1 = (unsigned short *)driver->EndOfRaceComment_ptrQuip, puVar1 != 0)) {
                *(int *)&puVar1[2] = 0;
                if ((puVar1[1] & 1) != 0) {
                    func_80078348(acStack_a0, D_8008D878[puVar1[0]],
                                  D_8008D878[D_80086D84[driver->EndOfRaceComment_characterID].name_LNG_long]);
                    func_8004568C(acStack_a0, (short)iVar5, (short)iVar3, 0, 3, -0x8000, 4);
                } else {
                    func_8004568C(D_8008D878[puVar1[0]], (short)iVar5, (short)iVar3, 0, 3, -0x8000, 4);
                }
            }
            sVar6 = sVar6 + 1;
            thread = thread->siblingThread;
            psVar2 = psVar2 + 0x88;
        } while (thread != 0);
    }
}
