// MainDrawCb_Vsync @ 0x80034aa4  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 120, grind floor 100). Residual: gcc-2.8.1
// fills the bnez delay slot / schedules the ResetRCnt arg materialization slightly
// differently than retail. Behavior identical.
// VSync callback: frameTimer_VsyncCallback++, if !PAUSE (gameMode1 & 0xF)
// frameTimer_Confetti++, vsyncTillFlip--, vSync_between_drawSync++, accumulate
// GetRCnt(0xF2000001) into rcntTotalUnits + ResetRCnt, howl_PlayAudio_Update,
// GAMEPAD_PollVsync(gGamepads). gGT + gGamepads + vsyncTillFlip are gp-relative
// (gp+0x340/0x344/0x348 → declare defined `=0`); rcntTotalUnits is absolute.
#include "CTR.h"

struct GameTracker {
    int gameMode1;                    /* 0x0    */
    char pad_1CE0[0x1CE0 - 0x4];
    int vSync_between_drawSync;       /* 0x1CE0 */
    char pad_1CE4[0x1CE4 - 0x1CE4];
    int frameTimer_VsyncCallback;     /* 0x1CE4 */
    char pad_1CF0[0x1CF0 - 0x1CE8];
    int frameTimer_Confetti;          /* 0x1CF0 */
};

#define PAUSE_ALL 0xF

struct GameTracker *D_8008D2AC = 0;   /* sdata->gGT       (gp+0x340) */
void *D_8008D2B0 = 0;                 /* sdata->gGamepads (gp+0x344) */
int   D_8008D2B4 = 0;                 /* sdata->vsyncTillFlip (gp+0x348) */
extern int D_8008D988;                /* sdata->rcntTotalUnits (absolute) */

extern int  func_80077BE4(int);       /* GetRCnt   */
extern void func_80077C80(int);       /* ResetRCnt */
extern void func_8002C208(void);      /* howl_PlayAudio_Update */
extern void func_80025410(void *);    /* GAMEPAD_PollVsync */

void MainDrawCb_Vsync(void)
{
    struct GameTracker *gGT = D_8008D2AC;

    gGT->frameTimer_VsyncCallback++;
    if ((gGT->gameMode1 & PAUSE_ALL) == 0)
        gGT->frameTimer_Confetti++;

    D_8008D2B4--;
    D_8008D2AC->vSync_between_drawSync++;

    D_8008D988 += func_80077BE4(0xf2000001);
    func_80077C80(0xf2000001);
    func_8002C208();
    func_80025410(D_8008D2B0);
}
