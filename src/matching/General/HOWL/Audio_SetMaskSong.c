#include "HOWL.h"

// Audio_SetMaskSong @ 0x8002D554 (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH — score 160, NOT score-0. Behavior is byte-correct (identical opcode multiset apart from
// one delay-slot nop); the residual is a pure gcc scheduling/register tie-break.
//
// If any player is holding the Aku/Uka mask weapon, switch to the matching mask song (Uka=2 if the
// UKA_SONG bit is set, else Aku=1 if AKU_SONG set) via Music_Adjust; otherwise Music_Adjust(0,..) and
// clear the mask-song bits. Reversed from asm + in-repo decomp h103.
//
// MATCH NOTE — residual 160: the false branch's `Music_Adjust(0,tempo,0,0)` first arg (a0=0) — retail
// leaves the isMaskUsed `beqz` delay slot as `nop` and materializes a0=0 inside the false branch; my
// gcc fills the delay slot with that `move a0,zero` (1 fewer nop). Pure delay-slot scheduling +
// register-naming; behavior identical. Permuter plateaued at 160 over 90 iters; manual int/u_char/
// short/separate-pointer/walker forms explored (short is the floor: int=435, u_char=360, short=160).
//
// Raw-pointer offsets on sdata_gGT (D_8008D2AC): numPlyrCurrGame@0x1CA8, drivers[]@0x24EC (Driver*),
// driver->actionsFlagSet@0x2C8 (mask bit 0x800000), gameMode1@0x0 (UKA_SONG=0x2000000, AKU_SONG=
// 0x1000000). Music_Adjust = func_8002E350.

extern void func_8002E350(int songID, int tempo, void* a, int b);  // Music_Adjust

extern char* D_8008D2AC;   // sdata->gGT pointer

void Audio_SetMaskSong(int tempo)
{
    char* gGT;
    int i;
    short isMaskUsed;
    int songID;
    int gameMode;

    gGT = D_8008D2AC;
    isMaskUsed = 0;
    for (i = 0; i < *(u_char*)(gGT + 0x1CA8); i++)
    {
        if (*(int*)(*(int*)(gGT + 0x24EC + i * 4) + 0x2C8) & 0x800000)
            isMaskUsed = 1;
    }

    if (isMaskUsed)
    {
        gameMode = *(int*)D_8008D2AC;
        songID = 2;
        if ((gameMode & 0x2000000) != 0 || (songID = 1, (gameMode & 0x1000000) != 0))
            func_8002E350(songID, tempo, 0, 0);
    }
    else
    {
        func_8002E350(0, tempo, 0, 0);
        gGT = D_8008D2AC;
        gameMode = *(int*)gGT;
        if (gameMode & 0x2000000)
        {
            *(int*)gGT = gameMode & 0xFDFFFFFF;
            gGT = D_8008D2AC;
            gameMode = *(int*)gGT;
        }
        if (gameMode & 0x1000000)
            *(int*)gGT = gameMode & 0xFEFFFFFF;
    }
}
