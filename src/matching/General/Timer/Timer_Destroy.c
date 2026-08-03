/* Timer_Destroy @ 0x8004B370 — stop RCnt2 (0xF2000001) inside a critical section. */
extern void func_80076990(void);              /* EnterCriticalSection */
extern void func_80077C4C(unsigned int spec); /* StopRCnt */
extern void func_800767D0(void);              /* ExitCriticalSection */

void Timer_Destroy(void)
{
    func_80076990();
    func_80077C4C(0xf2000001);
    func_800767D0();
}
