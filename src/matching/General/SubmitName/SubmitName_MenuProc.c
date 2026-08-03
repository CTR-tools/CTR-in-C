/*
 * SubmitName_MenuProc @ 0x8004B144  (vs SCUS_944.26.exe / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (residual = gcc block-ordering + register coloring).
 * RECTMENU callback for the OSK name screen: draws via SubmitName_DrawMenu(0x13f), routes
 * SAVE/CANCEL by name-entry mode. Uses real GameTracker types from driver.h.
 * Phase B (saphi SubmitName_2_MenuProc.c): CLEAN.
 */
#include "../gametracker.h"

struct MenuBox { char _pad[0x1A]; short rowSelected; };

extern short func_8004AA60(unsigned short saveStringId);  /* SubmitName_DrawMenu */
extern void *func_800B854C(void);                          /* CS_Garage_GetMenuPtr */
extern void  func_800B7784(int arg);                       /* CS_Garage_ZoomOut */
extern void  func_80048E2C(int mode);                      /* SelectProfile_ToggleMode */
extern struct GameTracker *D_8008D2AC;                     /* gGT */
extern char D_8008FBBC[];                                  /* g_abAdvProgress */
extern char D_80085B88;                                    /* data_menuBox_GhostSelection */
extern char D_800A0458;                                    /* menu224 */
extern char D_80085BB4;                                    /* data_menuBox_FourAdvProfiles */
short D_8008D906 = 0;                                       /* g_nNameEntryMode (gp+0x99A) */
void *D_8008D924 = 0;                                       /* g_pDesiredMenu (gp+0x9B8) */

void SubmitName_MenuProc(struct MenuBox *menu)
{
    int result = func_8004AA60(0x13f);
    menu->rowSelected = (short)result;
    if (result != 0) {
        if (D_8008D906 == 1) {
            if (result < 0) {
                D_8008D924 = &D_800A0458;
            } else {
                func_80048E2C(0x31);
                D_8008D924 = &D_80085B88;
            }
        } else if ((D_8008D906 < 2) && (D_8008D906 == 0)) {
            if (result < 0) {
                D_8008D924 = func_800B854C();
                func_800B7784(1);
            } else {
                char *prev = D_8008D2AC->prevNameEntered;
                *(int *)(D_8008FBBC + 0x18) = *(int *)prev;
                *(int *)(D_8008FBBC + 0x1c) = *(int *)(prev + 4);
                *(int *)(D_8008FBBC + 0x20) = *(int *)(prev + 8);
                *(int *)(D_8008FBBC + 0x24) = *(int *)(prev + 0xc);
                D_8008FBBC[0x28] = prev[0x10];
                func_80048E2C(1);
                D_8008D924 = &D_80085BB4;
            }
        }
    }
}
