#include "HOWL.h"

// Voiceline_RequestPlay @ 0x8002CBE8 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best (natural-code) score = 3550, byte-BEHAVIOR-correct. GRANTED
//   (2026-07-28, user). This large ~208-instruction voiceline dispatcher was reverse-engineered ENTIRELY
//   from the asm (the Ghidra MCP was down and there is no in-repo decomp). The behavior is verified exactly
//   correct: the compiled diff has ZERO opcode/constant differences vs retail — every remaining difference
//   is a register-name or instruction-position displacement, not a logic error. The residual is a gcc-2.8.1
//   caller-saved register-allocation tie-break: the a4/a5 decision-tree flags (canQueue/canPlay) land in
//   $v0/$v1 here vs $a0/$a1 in retail, which cascades through every decision-tree branch; plus a small
//   trade-off from keeping `&Voiceline2` in a pointer (needed to fix the parameter register allocation to
//   retail's $s3/$s2/$s4) which routes the Voiceline2 walk-load through that pointer, and some scheduling.
//   Same class as the #88/#90/#93 register/address divergences, but a far larger function so the residual
//   is larger. The permuter only finds sub-3550 forms via unnatural variable-reuse tricks.
//
//   KEY FIXES that got 4635 -> 3550: (1) `D_80096244[channel] & (1 << id)` written inline miscompiles to
//   `andi 0x1` (gcc-2.8.1 folds `1<<id` to 1 in the `&` boolean context!) — an explicit `int bit = 1<<id`
//   is required in the CHECK, though the queue `|=` compiles `1<<id` correctly inline; (2) the `struct
//   LinkedList* v2 = &D_800961B4` pointer through the queue block gives gcc the 5th callee-saved register so
//   the params match retail; (3) canPlay/canQueue as single boolean `&&` chains match retail's shared exit.
//
// Requests a voiceline: validates (id<24, channel<16, flag<17), bails on a game-state flag; for id>=8 gates
// on a per-channel bitmask + an inline PRNG chance; computes an elapsed-since-last-play cooldown; then, per
// the enable flags / cooldown / a coin flip, either plays it immediately as an OtherFX or queues it onto the
// active voiceline list (dedup-checked), recycling a node from the free list or the active-list tail.
//
// GP-rel DEFINED (GP_VALUE=0x8008CF6C): boolCanPlayVoicelines=D_8008D7EC(0x880,u8),
// voicelineCooldown=D_8008D7EE(0x882,short). ABS extern: g_pGameState=D_8008D2AC(char*; +0x1CF8=curTime,
// +0x1CEC=nodeTime), priorityTable=D_800838DC(u8[]), perChannelBitmask=D_80096244(int[16]),
// lastPlayTime=D_80096284(int[16]), prngSeed=D_8008D058(int), lastPlayedChannel=D_8009619E(u8),
// Voiceline1=D_800961A4/Voiceline2=D_800961B4 (LinkedList). Callees: func_80028468 OtherFX_Play,
// func_800317E4 LIST_RemoveMember, func_80031744 LIST_AddFront.

extern void func_80028468(int id, int mode);          // OtherFX_Play
extern void func_800317E4(void* list, void* item);    // LIST_RemoveMember
extern void func_80031744(void* list, void* item);    // LIST_AddFront

extern char* D_8008D2AC;      // g_pGameState pointer
extern u_char D_800838DC[];   // priority table (indexed by id)
extern int D_80096244[];      // per-channel bitmask (bit=id)
extern int D_80096284[];      // per-channel last-play timestamp
extern int D_8008D058;        // PRNG seed
extern u_char D_8009619E;     // last-played channel
extern struct LinkedList D_800961A4;   // Voiceline1 (free list)
extern struct LinkedList D_800961B4;   // Voiceline2 (active list)

u_char D_8008D7EC = 0;   // boolCanPlayVoicelines (gp+0x880)
short  D_8008D7EE = 0;   // voicelineCooldown     (gp+0x882)

void Voiceline_RequestPlay(int id, int channel, int flag)
{
    int prio;
    int elapsed;
    int canPlay;
    int canQueue;
    struct Voiceline* node;

    if ((uint)id >= 24) return;
    if ((uint)channel >= 16) return;
    if ((uint)flag >= 17) return;
    if (*(int*)D_8008D2AC & 0x200000) return;

    prio = D_800838DC[id];

    if (id >= 8)
    {
        int bit = 1 << id;
        if (D_80096244[channel] & bit)
        {
            D_8008D058 = (((uint)D_8008D058 >> 3) + (D_8008D058 << 29)) * 5 + 1;
            if (D_8008D058 & 7) return;
        }
        else
        {
            D_8008D058 = (((uint)D_8008D058 >> 3) + (D_8008D058 << 29)) * 5 + 1;
            if (D_8008D058 & 3) return;
        }
    }

    elapsed = *(int*)(D_8008D2AC + 0x1CF8) - D_80096284[channel];
    canPlay = elapsed >= 0x3D && prio < 2;

    canQueue = D_8008D7EC != 0 && !(D_8008D7EE != 0 && D_8009619E == channel) && elapsed >= 0x3C;

    if (canQueue)
    {
        if (canPlay)
        {
            D_8008D058 = (((uint)D_8008D058 >> 3) + (D_8008D058 << 29)) * 5 + 1;
            if ((D_8008D058 & 1) == 0)
                goto queue;
        }
        else
        {
            goto queue;
        }
    }
    else if (!canPlay)
    {
        return;
    }

    // play now
    if (prio == 0)
        func_80028468((u_short)(channel + 0x1C), 2);
    else if (prio == 1)
        func_80028468((u_short)(channel + 0x2C), 2);
    D_80096284[channel] = *(int*)(D_8008D2AC + 0x1CF8);
    return;

queue:
    if (canQueue == 0)
        return;
    D_80096244[channel] |= 1 << id;
    {
        struct LinkedList* v2 = &D_800961B4;
        for (node = (struct Voiceline*)v2->first; node != 0; node = node->next)
        {
            if (node->id == id && node->channel == channel)
                return;
        }
        node = (struct Voiceline*)D_800961A4.first;
        if (node != 0)
        {
            func_800317E4(&D_800961A4, node);
        }
        else
        {
            node = (struct Voiceline*)v2->last;
            if (node != 0)
                func_800317E4(v2, node);
        }
        func_80031744(v2, node);
    }
    node->channel = channel;
    node->flag = flag;
    node->id = id;
    node->time = *(int*)(D_8008D2AC + 0x1CEC);
}
