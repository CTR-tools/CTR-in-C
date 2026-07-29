#include "HOWL.h"

// Music_SetIntro @ 0x8002DD24 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH.
// Loads the intro-music bank + song ("start your engines...") and blocks until both are ready.
// Verified vs Ghidra decomp + h107.

struct Bank
{
    short    bankID;
    u_short  flags;
    u_short  min;
    u_short  max;
};

extern void func_800297A0(int bankID, struct Bank* bank);  // Bank_Load
extern int  func_800293B8(void);                           // Bank_AssignSpuAddrs
extern void func_80029C40(int song);                       // howl_SetSong
extern int  func_80029CA4(void);                           // howl_LoadSong

u_char D_8008D834 = 0;   // gp+0x8C8 audioDefaults[7]

void Music_SetIntro(void)
{
    struct Bank thisBank;

    D_8008D834 = 0;
    func_800297A0(0x21, &thisBank);
    while (func_800293B8() == 0) {}
    func_80029C40(0x1C);
    while (func_80029CA4() == 0) {}
}
