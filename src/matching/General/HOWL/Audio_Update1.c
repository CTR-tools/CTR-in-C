#include "HOWL.h"

// Audio_Update1 @ 0x8002D67C (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH — score 195, NOT score-0. The LARGEST HOWL function (~372 insns): the per-frame audio
// state machine (jump-table switch, 16 cases on unkAudioState-1). Behavior is byte-correct; the residual
// is gcc codegen/register tie-breaks. Reversed from asm + in-repo decomp h104 (retail config:
// non-REBUILD_PS1, non-USE_ONLINE). Raw-pointer offsets on sdata_gGT (D_8008D2AC).
//
// MATCH NOTE — residual 195 (drove 3490 -> 195 via 5 natural structural cracks: shared-goto case-14
// N-Tropy tree; driver loop commits d only on break via temp p; d=0 inited once in prologue; numLaps in
// a caller-saved local; inline D_8008D2AC re-read for numLaps). Remaining is two gcc tie-breaks: (a)
// JUMP-TABLE RANGE — retail builds a 16-entry table (`addiu -1; sltiu 16`, index state-1), my gcc
// tightens to [5,15] (`addiu -6; sltiu 11`) because the lowest case is 5; unforceable without adding an
// artificial low case, and behavior is identical. (b) case-14 register naming (gGT in v1 vs a0, the
// xaVoice literals in v0 vs v1) + one nop. Permuter plateaued at 195 over 86 iters. Auto-granted as a
// <300 clean-codegen plateau.

extern void  func_8002D4CC(int state);      // Audio_SetState_Safe (#101)
extern void  func_800304B8(void);           // Garage_LerpFX
extern void  func_8002D554(int tempo);      // Audio_SetMaskSong (#103)
extern void  func_8002D0F8(void);           // Voiceline_Update (#97)
extern void  func_8002EBE4(void);           // Level_AmbientSound
extern void  func_8001CC18(int, int, int);  // CDSYS_XASeek
extern void  func_8002E46C(void);           // Music_RaiseVolume
extern void  func_80028468(int, int);       // OtherFX_Play (#2)
extern short func_80026AE4(int);            // GAMEPROG_CheckGhostsBeaten

extern int     D_8008D708;      // XA_State
extern int     D_8008D6E4;      // XA_Playing_Category
extern int     D_8008D6EC;      // XA_PauseFrame
extern int     D_8008D6F8;      // XA_CurrOffset
extern char*   D_8008D2AC;      // sdata->gGT
extern u_short D_80083908[];    // nTropyXA table

short  D_8008D7F0 = 0;   // gp+0x884 unkAudioState
short  D_8008D7F8 = 0;   // gp+0x88C desiredXA_RaceEndIndex
short  D_8008D7FA = 0;   // gp+0x88E nTropyVoiceCount
u_char D_8008D7FC = 0;   // gp+0x890 boolNeedXASeek

void Audio_Update1(void)
{
    int   i;
    char* d;
    char* p;
    char* gGT;
    short xaVoice;
    int   rank;

    d = 0;
    switch ((short)((u_short)D_8008D7F0 - 1))
    {
    case 5:
        if (D_8008D708 == 0 || D_8008D6E4 != 0)
            func_8002D4CC(5);
        break;
    case 7:
        func_800304B8();
        break;
    case 8:
        if (D_8008D708 != 0) break;
        func_8002D4CC(0xA);
        break;
    case 9:
        if (*(int*)(D_8008D2AC + 0x1D0C) > 0) break;
        func_8002D4CC(0xB);
        break;
    case 10:
        func_8002D554(0);
        gGT = D_8008D2AC;
        for (i = 0; i < 8; i++)
        {
            p = *(char**)(gGT + 0x250C + i * 4);
            if (p != 0 && (*(int*)(p + 0x2C8) & 0x100000) == 0)
            {
                d = p;
                break;
            }
        }
        func_8002D0F8();
        func_8002EBE4();
        i = *(signed char*)(D_8008D2AC + 0x1D33);
        if (i < 3) break;
        if (*(u_char*)(d + 0x44) != i - 2) break;
        if (*(int*)(d + 0x488) < 9000)
            func_8002D4CC(0xC);
        break;
    case 11:
        func_8002D554(0);
        gGT = D_8008D2AC;
        for (i = 0; i < 8; i++)
        {
            p = *(char**)(gGT + 0x250C + i * 4);
            if (p != 0 && (*(int*)(p + 0x2C8) & 0x100000) == 0)
            {
                d = p;
                break;
            }
        }
        if (D_8008D7FC != 0 && D_8008D708 == 0
            && (u_int)(*(int*)(D_8008D2AC + 0x1CF8) - D_8008D6EC) >= 0xA)
        {
            if (*(int*)(d + 0x488) > 2000)
                func_8001CC18(1, 0, 6);
            D_8008D7FC = 0;
        }
        func_8002EBE4();
        if (*(u_char*)(d + 0x44) == *(signed char*)(D_8008D2AC + 0x1D33) - 1)
            func_8002D4CC(0xD);
        break;
    case 12:
        i = 0;
        if (D_8008D6F8 > 0xE1)
        {
            func_8002E46C();
            i = 20;
        }
        func_8002D554(i);
        func_8002EBE4();
        if (D_8008D708 == 0)
            func_8002D4CC(0xE);
        break;
    case 13:
        func_8002D554(0x14);
        gGT = D_8008D2AC;
        for (i = 0; i < 8; i++)
        {
            p = *(char**)(gGT + 0x250C + i * 4);
            if (p != 0 && (*(int*)(p + 0x2C8) & 0x100000) == 0)
            {
                d = p;
                break;
            }
        }
        func_8002D0F8();
        func_8002EBE4();
        if (*(u_char*)(d + 0x44) == *(signed char*)(D_8008D2AC + 0x1D33) - 1
            && *(int*)(d + 0x488) < 9000)
            func_8002D4CC(0xF);
        break;
    case 14:
        gGT = D_8008D2AC;
        rank = -1;
        for (i = 0; i < 8; i++)
        {
            p = *(char**)(gGT + 0x250C + i * 4);
            if (p != 0 && *(short*)(*(int*)(*(int*)(p + 0x1C) + 0x6C) + 0x44) == 0x18)
            {
                rank = i;
                d = p;
                break;
            }
        }
        if (D_8008D7FC != 0 && D_8008D708 == 0
            && (u_int)(*(int*)(D_8008D2AC + 0x1CF8) - D_8008D6EC) >= 0xA)
        {
            if (*(int*)(d + 0x488) > 2000)
                func_8001CC18(1, 0, 4);
            D_8008D7FC = 0;
        }
        func_8002EBE4();
        if ((*(int*)(d + 0x2C8) & 0x2000000) == 0)
            break;
        gGT = D_8008D2AC;
        if ((*(int*)(gGT + 0x1D44) & 0x8000) != 0)
        {
            func_80028468(0x5F, 0);
            xaVoice = 0x46;
            goto set_desired;
        }
        if ((*(int*)(gGT + 0x1D44) & 0x10000000) != 0)
        {
            if (func_80026AE4(1) != 0)
            {
                func_80028468(0x5F, 0);
                xaVoice = 0x43;
                goto set_desired;
            }
            func_80028468(0x5F, 0);
            xaVoice = D_80083908[D_8008D7FA];
            D_8008D7FA = D_8008D7FA + 1;
            D_8008D7F8 = xaVoice;
            if (D_8008D7FA > 5)
                D_8008D7FA = 0;
            goto do_setstate;
        }
        xaVoice = 5;
        if (rank == 0 || (*(int*)gGT & 0x10000000) != 0
            || (*(int*)(gGT + 8) & 0x10) != 0)
        {
            func_80028468(0x5F, 0);
            xaVoice = 4;
        }
    set_desired:
        D_8008D7F8 = xaVoice;
    do_setstate:
        func_8002D4CC(0x10);
        break;
    case 15:
        func_8002EBE4();
        if (D_8008D708 == 0)
            func_8002D554(0);
    }
}
