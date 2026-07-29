#include "HOWL.h"

// Music_LoadBanks @ 0x8002DD74 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH.
// Sets the reverb mode then (re)loads the SPU banks for the current level: intro-race (0x1E)->bank 0x22,
// N.Dog crate (0x29)->bank 0x21, else toggle the two-stage load flags. Verified vs Ghidra decomp + h108.

struct Bank
{
    short    bankID;
    u_short  flags;
    u_short  min;
    u_short  max;
};

extern void func_8002DCAC(int levelID, int isBossRace, int bossID);  // Audio_SetReverbMode (#106)
extern void func_800298E4(void);                                    // Bank_DestroyAll
extern void func_800297A0(int bankID, struct Bank* bank);           // Bank_Load
extern void func_80029870(int index);                               // Bank_DestroyUntilIndex

extern char* D_8008D2AC;   // sdata->gGT

u_char D_8008D834 = 0;   // gp+0x8C8 audioDefaults[7] (arr[0])
u_char D_8008D835 = 0;   // gp+0x8C9 audioDefaults[8] (arr[1], loading state)

void Music_LoadBanks(void)
{
    struct Bank thisBank;
    int bankID;
    char* gGT;
    int level;

    gGT = D_8008D2AC;
    func_8002DCAC(*(int*)(gGT + 0x1A10), *(int*)gGT & 0x80000000, *(int*)(gGT + 0x1EB8));

    level = *(int*)(D_8008D2AC + 0x1A10);
    if (level == 0x1E)
    {
        func_800298E4();
        bankID = 0x22;
    }
    else if (level == 0x29)
    {
        func_800298E4();
        bankID = 0x21;
    }
    else
    {
        goto load_state;
    }
    D_8008D834 = 0;
    func_800297A0(bankID, &thisBank);
    D_8008D835 = 3;
    return;

load_state:
    if (D_8008D834 != 0)
    {
        func_80029870(0);
    }
    else
    {
        func_800298E4();
        func_800297A0(0, &thisBank);
        D_8008D834 = 1;
    }
    D_8008D835 = 0;
}
