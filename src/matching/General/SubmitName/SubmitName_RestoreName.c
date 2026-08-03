/*
 * SubmitName_RestoreName @ 0x8004AA08  (vs SCUS_944.26.exe / NTSC-U 926) — score 0 (byte-exact).
 * Prep the OSK name-entry screen: store mode, copy prevNameEntered->currNameEntered (0x11),
 * cursor on SAVE (0x3E9) if a name exists else 0. Uses real GameTracker types from driver.h.
 * Phase B (saphi SubmitName_0_RestoreName.c): CLEAN.
 */
#include "../gametracker.h"

extern void func_80077E38(void *dst, void *src, int n);   /* memmove */
extern struct GameTracker *D_8008D2AC;                    /* gGT */
short D_8008D906 = 0;                                     /* g_nNameEntryMode (gp+0x99A) */

void SubmitName_RestoreName(short mode)
{
    short cursor;
    D_8008D906 = mode;
    func_80077E38(D_8008D2AC->currNameEntered, D_8008D2AC->prevNameEntered, 0x11);
    cursor = 0;
    if (D_8008D2AC->currNameEntered[0] != 0) {
        cursor = 0x3e9;
    }
    D_8008D2AC->TitleOSK_CursorPosition = cursor;
}
