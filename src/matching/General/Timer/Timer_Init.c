/* Timer_Init @ 0x8004B31C — start RCnt2 (0xF2000001) free-running inside a critical section. */
extern void func_80076990(void);                                    /* EnterCriticalSection */
extern void func_80077C4C(unsigned int spec);                       /* StopRCnt */
extern void func_80077B48(unsigned int spec, int target, int mode); /* SetRCnt */
extern void func_80077C1C(unsigned int spec);                       /* StartRCnt */
extern void func_800767D0(void);                                    /* ExitCriticalSection */

void Timer_Init(void)
{
    func_80076990();
    func_80077C4C(0xf2000001);
    func_80077B48(0xf2000001, 0xffff, 0x2000);
    func_80077C1C(0xf2000001);
    func_800767D0();
}
