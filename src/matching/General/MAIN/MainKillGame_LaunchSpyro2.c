// MainKillGame_LaunchSpyro2 @ 0x8003c480  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// Tears down CTR and chain-loads the Spyro 2 playable demo (CTR demo disc feature):
// CTR_ErrorScreen(0,0,0) blank, Music_Stop, howl_StopAudio(1,1,1), Bank_DestroyAll,
// howl_Disable, VSync(0x1e), MainKillGame_StopCTR, then libcd _96_remove/_96_init and
// LoadExec("cdrom:\SPYRO2.EXE;1", 0x801fff00, 0). Does not return.
#include "CTR.h"

extern char D_80011458[];   /* "cdrom:\SPYRO2.EXE;1" */

extern void func_80021C94(int, int, int);   /* CTR_ErrorScreen */
extern void func_8002E4EC(void);            /* Music_Stop */
extern void func_8002C8A8(int, int, int);   /* howl_StopAudio */
extern void func_800298E4(void);            /* Bank_DestroyAll */
extern void func_8002AC94(void);            /* howl_Disable */
extern void func_80075350(int);             /* VSync */
extern void func_8003C41C(void);            /* MainKillGame_StopCTR */
extern void func_80077D10(void);            /* _96_remove */
extern void func_80077D28(void);            /* _96_init */
extern void func_8007780C(char *, int, int);/* LoadExec */

void MainKillGame_LaunchSpyro2(void)
{
    func_80021C94(0, 0, 0);
    func_8002E4EC();
    func_8002C8A8(1, 1, 1);
    func_800298E4();
    func_8002AC94();
    func_80075350(0x1e);
    func_8003C41C();
    func_80077D10();
    func_80077D28();
    func_8007780C(D_80011458, 0x801fff00, 0);
}
