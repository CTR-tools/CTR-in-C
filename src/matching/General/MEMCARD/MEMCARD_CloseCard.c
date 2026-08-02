/*
 * MEMCARD_CloseCard @ 0x8003D95C  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * StopCARD() then closes the 8 memcard events in a critical section.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_Card.c audited CLEAN vs this asm + Ghidra.
 */
extern void func_80077F8C(void);   /* StopCARD */
extern void func_80076990(void);   /* EnterCriticalSection */
extern void func_80078328(long ev);/* CloseEvent */
extern void func_800767D0(void);   /* ExitCriticalSection */
int D_8008D8D4=0, D_8008D8C4=0, D_8008D8E0=0, D_8008D8C0=0;  /* Sw IOE/Err/TO/New */
int D_8008D8C8=0, D_8008D8B4=0, D_8008D8DC=0, D_8008D8BC=0;  /* Hw IOE/Err/TO/New */
void MEMCARD_CloseCard(void)
{
    func_80077F8C();
    func_80076990();
    func_80078328(D_8008D8D4);
    func_80078328(D_8008D8C4);
    func_80078328(D_8008D8E0);
    func_80078328(D_8008D8C0);
    func_80078328(D_8008D8C8);
    func_80078328(D_8008D8B4);
    func_80078328(D_8008D8DC);
    func_80078328(D_8008D8BC);
    func_800767D0();
}
