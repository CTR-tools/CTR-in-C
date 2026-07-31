// LOAD_InitCD @ 0x8007c208  (vs SCUS_944.26.exe)  [the 2nd LOAD_InitCD symbol —
// the low-level CD/CDSYS init body that CDSYS_Init(1) drives, NOT the 0x80031c58
// wrapper]. Reconstructed from asm (no saphi ref; Ghidra decomp truncates after
// the printf, so this is a from-scratch decompile).
// BEHAVIOR-EXACT (Branch/Stack 0; base 1090, grind floor ~975).
// Residual: whole-function register allocation on a 120-instruction libcd routine
// — retail materializes each CdControlB zero-arg from $zero (gcc-2.8.1 CSEs them
// as move a2,a1) plus scheduling/reg-assignment drift. Behavior identical.
// Flow: print "CD_init:"/addr, zero the CD state globals, install the CD callback
// (func_800771F4 mode 2 -> func_8007C748), pulse CD_REG(0)=1 and spin clearing
// pending CD interrupts (CD_REG(3)&7), stage the command buffer, set the timing
// slot (0x1325), run CdControlB(1) [+ conditional retry if flag&0x10], then
// CdControlB(0xA)/(0xC)/CdSync and return 0 on CdSync==2 else -1.
#include "CTR.h"

extern char D_80012380[];   /* "CD_init:"    */
extern char D_8001238C[];   /* "addr=%08x\n" */
extern int  D_8008C6FC;     /* g_eCdSyncStatus (address printed) */

extern unsigned char D_8008C439;
extern unsigned char D_8008C438;
extern int D_8008C420;
extern int D_8008C41C;
extern int D_8008C42C;
extern int D_8008C428;

extern volatile unsigned char *D_8008C6E0;   /* CD_REG(0) */
extern volatile unsigned char *D_8008C6E4;   /* CD_REG(1) */
extern volatile unsigned char *D_8008C6E8;   /* CD_REG(2) */
extern volatile unsigned char *D_8008C6EC;   /* CD_REG(3) */
extern volatile unsigned char D_8008C6F8[]; /* command byte buffer */
extern int *D_8008C6F0;             /* timing/mode slot ptr */

extern void func_8007C9A8(char *s);                  /* SDK_DebugPrint */
extern void func_8007C820(char *fmt, void *arg);     /* printf */
extern void func_800771C4(void);
extern void func_800771F4(int mode, void *handler);
extern int  func_8007BC38(int cmd, int a1, int a2, int a3);   /* CdControlB */
extern int  func_8007B6F0(int a0, int a1);                    /* CdSync */
extern void func_8007C748(void);                              /* CD callback handler */

int LOAD_InitCD(void)
{
    func_8007C9A8(D_80012380);
    func_8007C820(D_8001238C, &D_8008C6FC);

    D_8008C439 = 0;
    D_8008C438 = 0;
    D_8008C420 = 0;
    D_8008C41C = 0;
    D_8008C42C = 0;
    D_8008C428 = 0;

    func_800771C4();
    func_800771F4(2, func_8007C748);

    *D_8008C6E0 = 1;
    while (*D_8008C6EC & 7)
    {
        *D_8008C6E0 = 1;
        *D_8008C6EC = 7;
        *D_8008C6E8 = 7;
    }

    D_8008C6F8[2] = 0;
    D_8008C6F8[1] = D_8008C6F8[2];
    D_8008C6F8[0] = 2;
    *D_8008C6E0 = 0;
    *D_8008C6EC = 0;

    *D_8008C6F0 = 0x1325;
    func_8007BC38(1, 0, 0, 0);

    if (D_8008C428 & 0x10)
        func_8007BC38(1, 0, 0, 0);

    if (func_8007BC38(0xA, 0, 0, 0) != 0)
        return -1;

    if (func_8007BC38(0xC, 0, 0, 0) != 0)
        return -1;

    if (func_8007B6F0(0, 0) != 2)
        return -1;

    return 0;
}
