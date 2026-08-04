/*
 * UI_VsWaitForPressX @ 0x800552A4 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg122, score 8330 — strength-reduction + register-coloring
 * wall: the per-viewport rect base, the gamepad-tapped IV, and the driver-index IV compete for
 * callee-saved regs; retail spills the driver index (local_30) and keeps the gamepad byte-offset in
 * $fp, ours colors them the other way, plus a few short-field lh/lhu coin-flips. All non-behavioral.)
 * Real types. (Previously ⏸ deferred; brought to Br/St 0 this pass.)
 *
 * Battle end-of-race per-viewport "press X to continue" + attack-tally screen. Per-player flags in
 * g_abBattleEndPressX[i]: bit&1 = show YOU-HIT(0x157) vs HIT-YOU(0x158), bit&2 = this player done.
 * For each viewport i, while NOT done: D-pad L/R (tap & 4|8) toggles the view bit; Cross/Start
 * (tap & 0x1010, once gGT->timerEndOfRaceVS < 0x78) toggles done. In battle mode (gameMode1 & 0x20)
 * draws the header lngStrings[0x157/0x158] centered in the viewport (FONT_CREDITS, color -0x7FFC),
 * then one "pN:NN" line per other player j = currDriver->numTimesAttackingPlayer[j] (you-hit) or
 * numTimesAttackedByPlayer[j] (hit-you), positioned from g_aBattleEndTextPos2P (2 players) or the
 * 3P4P template, colored by drivers[j]->BattleHUD.teamID (FONT_SMALL, (teamID+0x18)|0x8000). When a
 * player IS done: memset a 4-byte black colorBuf and draw a clear box over the viewport
 * (CTR_Box_DrawClearBox), Square (tap & 0x8000) re-opens, ready++. When ALL players are done:
 * timerEndOfRaceVS = 0 and clear g_abBattleEndPressX[0..3] (backward byte loop from [3]).
 *
 * NOTE(claude) vs in-repo decomp (REFERENCE; 926 asm authoritative): the 926 diverges from UI_52 —
 * the sprintf buffer is a STACK local (auStack_58), not scratchpad 0x1F800000; the clear box copies
 * the rect to a stack RECT (local_48) and CTR_Box_DrawClearBox takes 5 args (+&backBuffer->primMem);
 * the 3P4P text-position template and the 2P table both copy to stack locals via lwl/lwr (16B from
 * D_800116EC / 8B from g_aBattleEndTextPos2P); and the final reset clears the 4 flag bytes via a
 * backward byte loop, not a single word write. The per-viewport rect is reached off pushBuffer[0]'s
 * matrix_ViewProj base at stride 0x110 (rect@matrix-0xC), matching the binary.
 *
 * Phase B (saphi game/UI/UI_VsQuip.c UI_VsWaitForPressX): CLEAN — no bugs. Loop, flag toggles, header
 * + stat DecalFont lines, clear box, and reset all match. Divergences are behavior-equivalent: caches
 * numPlyr vs re-reading (invariant), combines the L/R and Cross/Start button masks, `r->w >> 1` vs
 * `(w<<16)>>17` (positive width), CTR_Box_DrawClearBox is a 4-arg native re-impl (primMem via macro,
 * same as SolidBox), reset via a single int write vs the 926's byte loop (identical effect). The
 * DecalFont color `(teamID+0x18)|0x8000` = 0x801x (saphi) vs the retail sign-extended 0xFFFF801x is
 * equivalent (DecalFont uses only the low 16 bits). Notably saphi already sprintf's to a STACK buffer
 * (matching the 926) rather than the decomp's scratchpad 0x1F800000.
 */
#include "../uitypes.h"

#define BATTLE_MODE  0x20
#define FONT_SMALL   2
#define FONT_CREDITS 3

/* gGamepads entry (stride 0x50); only the tapped-button mask named. */
struct Gamepad { char _pad_0000[0x14]; unsigned int buttonsTapped; char _pad_0018[0x50 - 0x18]; };

/* g_aBattleEndTextPos2P — 8 bytes (2 {short x, short y} entries), copied to a stack local. */
struct TextPos2 { short v[4]; };

extern struct GameTracker *D_8008D2AC;   /* sdata_gGT (absolute) */
extern struct Gamepad *D_8008D2B0;       /* sdata_gGamepads */
extern char D_8008D564[];                /* g_abBattleEndPressX (byte[]; &1 view, &2 done) */
extern char **D_8008D878;                /* sdata_lngStrings */
extern char D_800116EC[16];              /* g_aBattleEndTextPos3P4P template (16 bytes) */
extern char D_800116FC[];                /* sprintf fmt "p%d:%2.02d" */
extern struct TextPos2 D_8008D568;       /* g_aBattleEndTextPos2P */

extern void func_80022878(char *s, short x, short y, int font, int color);  /* DecalFont_DrawLine */
extern int  func_80078348(char *buf, char *fmt, int a, int b);              /* sprintf */
extern void *func_800718BC(void *p, int c, int n);                          /* memset */
extern void func_8002177C(struct RECT *r, unsigned int *color, int a,
                          void *ot, struct PrimMem *pm);                     /* CTR_Box_DrawClearBox */
extern void *memcpy(void *dst, const void *src, unsigned int n);

/* rect = pushBuffer[i].rect, reached off the matrix_ViewProj base at stride 0x110 (iVar8 = i*0x110) */
#define RECTB(off) ((char *)psVar5->pushBuffer[0].matrix_ViewProj.m + iVar8 + (off))

void UI_VsWaitForPressX(void)
{
    unsigned char bVar1;
    short sVar2, sVar3;
    int iVar4;
    struct GameTracker *psVar5;
    int *puVar6;
    unsigned char *puVar7;
    int iVar8;
    unsigned int uVar9;
    int iVar10, iVar12, iVar13, iVar14, iVar15;
    unsigned char *pbVar11;
    short posY;
    char shortArr3P4P[16];
    struct TextPos2 textPos2P;
    unsigned int auStack_58[2];
    short local_50, local_4e;
    struct RECT local_48;
    unsigned short local_40;
    unsigned char *local_38;
    unsigned int *local_34;
    int local_30;

    local_40 = 0;
    memcpy(shortArr3P4P, D_800116EC, 16);
    textPos2P = D_8008D568;
    iVar14 = 0;
    if (D_8008D2AC->numPlyrCurrGame != 0) {
        local_34 = auStack_58;
        iVar15 = 0;
        local_30 = 0;
        iVar8 = 0;
        do {
            psVar5 = D_8008D2AC;
            iVar8 = (iVar8 + iVar14) * 0x10;
            pbVar11 = (unsigned char *)&D_8008D564[iVar14];
            iVar13 = *(int *)((char *)D_8008D2AC->drivers + local_30);
            if ((*pbVar11 & 2) == 0) {
                uVar9 = *(unsigned int *)((char *)D_8008D2B0 + iVar15 + 0x14);
                if ((uVar9 & 4) != 0 || (uVar9 & 8) != 0) {
                    *pbVar11 = *pbVar11 ^ 1;
                }
                if ((*(unsigned int *)((char *)D_8008D2B0 + iVar15 + 0x14) & 0x1010) != 0 &&
                    D_8008D2AC->timerEndOfRaceVS < 0x78) {
                    D_8008D564[iVar14] = D_8008D564[iVar14] ^ 2;
                }
                iVar10 = 0x157;
                if ((D_8008D564[iVar14] & 1) != 0) {
                    iVar10 = 0x158;
                }
                if ((D_8008D2AC->gameMode1 & BATTLE_MODE) != 0) {
                    func_80022878(D_8008D878[iVar10],
                                  (short)((unsigned short)*(unsigned short *)RECTB(-0xc) +
                                          ((int)((unsigned int)*(unsigned short *)RECTB(-8) << 0x10) >> 0x11)),
                                  *(short *)RECTB(-10) + 0x23, FONT_CREDITS, -0x7FFC);
                    iVar10 = 0;
                    if (D_8008D2AC->numPlyrCurrGame != 0) {
                        local_38 = (unsigned char *)&D_8008D564[iVar14];
                        iVar12 = 1;
                        do {
                            iVar4 = (unsigned short)D_8008D2AC->drivers[iVar10]->BattleHUD.teamID;
                            puVar6 = (int *)shortArr3P4P;
                            if (D_8008D2AC->numPlyrCurrGame == 2) {
                                puVar6 = (int *)&textPos2P;
                            }
                            sVar3 = *(short *)(puVar6 + iVar10);
                            local_50 = sVar3;
                            local_4e = *(short *)((int)(puVar6 + iVar10) + 2);
                            sVar2 = *(short *)RECTB(-0xc);
                            posY = *(short *)RECTB(-10) + local_4e;
                            if ((*local_38 & 1) == 0) {
                                bVar1 = *(unsigned char *)(iVar13 + 0x50c);
                            } else {
                                bVar1 = *(unsigned char *)(iVar13 + 0x560);
                            }
                            func_80078348((char *)auStack_58, D_800116FC, iVar12, (unsigned int)bVar1);
                            func_80022878((char *)auStack_58, sVar2 + sVar3, posY, FONT_SMALL,
                                          (short)(iVar4 + 0x18 | 0x8000));
                            iVar13 = iVar13 + 1;
                            iVar10 = iVar10 + 1;
                            iVar12 = iVar12 + 1;
                        } while (iVar10 < (int)(unsigned char)D_8008D2AC->numPlyrCurrGame);
                    }
                }
            } else {
                func_800718BC((unsigned char *)local_34, 0, 4);
                local_48.x = *(short *)RECTB(-0xc);
                local_48.y = *(short *)RECTB(-10);
                local_48.w = *(short *)RECTB(-8);
                local_48.h = *(short *)RECTB(-6);
                func_8002177C(&local_48, local_34, 0, D_8008D2AC->backBuffer->otMem_startPlusFour,
                              &D_8008D2AC->backBuffer->primMem);
                if ((*(unsigned int *)((char *)D_8008D2B0 + iVar15 + 0x14) & 0x8000) != 0) {
                    *pbVar11 = *pbVar11 ^ 2;
                }
                local_40 = local_40 + 1;
            }
            iVar15 = iVar15 + 0x50;
            local_30 = local_30 + 4;
            iVar14 = iVar14 + 1;
            iVar8 = iVar14 * 0x10;
        } while (iVar14 < (int)(unsigned char)D_8008D2AC->numPlyrCurrGame);
    }
    iVar14 = 3;
    if (local_40 == (unsigned char)D_8008D2AC->numPlyrCurrGame) {
        D_8008D2AC->timerEndOfRaceVS = 0;
        puVar7 = (unsigned char *)&D_8008D564[3];
        do {
            *puVar7 = 0;
            iVar14 = iVar14 + -1;
            puVar7 = puVar7 + -1;
        } while (-1 < iVar14);
    }
}
