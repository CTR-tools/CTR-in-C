/*
 * MEMCARD_InitCard @ 0x8003D7D8  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Opens+enables the 8 memcard events (Sw/Hw x IOE/ERROR/TIMEOUT/NEW) in a critical section, then InitCARD(0)/StartCARD()/_bu_init(), and sets g_dwMemcardStatusFlags=1 (card present).
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_Card.c audited CLEAN vs this asm + Ghidra.
 */
extern void func_80076990(void);   /* EnterCriticalSection */
extern int  func_80078048(unsigned int spec, int mode, int mask, void *fn);  /* OpenEvent */
extern void func_80077FC8(int ev); /* EnableEvent */
extern void func_800767D0(void);   /* ExitCriticalSection */
extern void func_80077EE8(int b);  /* InitCARD */
extern void func_80077F54(void);   /* StartCARD */
extern void func_80077FD8(void);   /* _bu_init */
int D_8008D8D4=0, D_8008D8C4=0, D_8008D8E0=0, D_8008D8C0=0;  /* Sw IOE/Err/TO/New */
int D_8008D8C8=0, D_8008D8B4=0, D_8008D8DC=0, D_8008D8BC=0;  /* Hw IOE/Err/TO/New */
int D_8008D8CC=0;  /* statusFlags 0x960 */
void MEMCARD_InitCard(void)
{
    func_80076990();
    D_8008D8D4 = func_80078048(0xf4000001, 4,      0x2000, 0);
    D_8008D8C4 = func_80078048(0xf4000001, 0x8000, 0x2000, 0);
    D_8008D8E0 = func_80078048(0xf4000001, 0x100,  0x2000, 0);
    D_8008D8C0 = func_80078048(0xf4000001, 0x2000, 0x2000, 0);
    D_8008D8C8 = func_80078048(0xf0000011, 4,      0x2000, 0);
    D_8008D8B4 = func_80078048(0xf0000011, 0x8000, 0x2000, 0);
    D_8008D8DC = func_80078048(0xf0000011, 0x100,  0x2000, 0);
    D_8008D8BC = func_80078048(0xf0000011, 0x2000, 0x2000, 0);
    func_80077FC8(D_8008D8D4);
    func_80077FC8(D_8008D8C4);
    func_80077FC8(D_8008D8E0);
    func_80077FC8(D_8008D8C0);
    func_80077FC8(D_8008D8C8);
    func_80077FC8(D_8008D8B4);
    func_80077FC8(D_8008D8DC);
    func_80077FC8(D_8008D8BC);
    func_800767D0();
    func_80077EE8(0);
    func_80077F54();
    func_80077FD8();
    D_8008D8CC = 1;
}
