// MainFrame_TogglePauseAudio @ 0x80034b48  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 1020, grind floor 920; same 29 insns).
// Residual: gcc-2.8.1 vs retail branch/register scheduling within the two if-arms.
// Pause/unpause audio (idempotent via boolSoundPaused @gp+0x938): pause →
// OtherFX_Stop2(1)+howl_PauseAudio; unpause → howl_StopAudio(0,0,1)+howl_UnPauseAudio.
#include "CTR.h"

int D_8008D8A4 = 0;   /* sdata->boolSoundPaused (gp+0x938) */

extern void func_80028844(int);          /* OtherFX_Stop2 */
extern void func_8002C510(void);         /* howl_PauseAudio */
extern void func_8002C8A8(int, int, int);/* howl_StopAudio */
extern void func_8002C784(void);         /* howl_UnPauseAudio */

void MainFrame_TogglePauseAudio(int bool_pause)
{
    if (bool_pause == 0)
    {
        if (D_8008D8A4 != 0)
        {
            func_8002C8A8(0, 0, 1);
            func_8002C784();
            D_8008D8A4 = 0;
        }
    }
    else if (D_8008D8A4 == 0)
    {
        func_80028844(1);
        func_8002C510();
        D_8008D8A4 = 1;
    }
}
