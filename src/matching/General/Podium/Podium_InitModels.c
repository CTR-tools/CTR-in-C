/*
 * Podium_InitModels @ 0x80041C84  (vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact: Branch 0 / Stack 0. Residual = register + gcc basic-block ordering
 * (retail moves the rank==1/rank==2 handlers out-of-line; ~score 1550, no correctness gap).
 * Picks victory-podium models from final results: podium_modelIndex_[First/Second/Third] =
 * charID + 0x7E (STATIC_CRASHDANCE) by driverRank; the winner (rank 0) also selects the Tawna
 * companion by character. Uses real 926 struct types from driver.h.
 *
 * Phase B (saphi game/Podium/Podium_0_InitModels.c): CLEAN
 */
#include "../gametracker.h"

extern short D_80086E84[];   /* g_awCharacterIDs */

void Podium_InitModels(struct GameTracker *gGT)
{
    struct Driver **pDrivers;
    struct Driver *driver;
    int iDriver;
    short sRank;
    unsigned char uPodiumModel;

    iDriver = 0;
    pDrivers = gGT->drivers;
    gGT->podium_modelIndex_First = 0;
    gGT->podium_modelIndex_Second = 0;
    gGT->podium_modelIndex_Third = 0;
    gGT->podium_modelIndex_tawna = 0x8f;
    do {
        driver = *pDrivers;
        if (driver != 0) {
            sRank = driver->driverRank;
            if (sRank == 1) {
                gGT->podium_modelIndex_Second = (char)D_80086E84[driver->driverID] + 0x7e;
                goto next;
            }
            if (sRank < 2) {
                if (sRank == 0) {
                    uPodiumModel = (char)D_80086E84[driver->driverID] + 0x7e;
                    gGT->podium_modelIndex_First = uPodiumModel;
                    switch (uPodiumModel) {
                    case 0x7e: case 0x81: gGT->podium_modelIndex_tawna = 0x90; break;
                    case 0x7f: case 0x82: gGT->podium_modelIndex_tawna = 0x92; break;
                    case 0x84: case 0x85: gGT->podium_modelIndex_tawna = 0x91; break;
                    }
                    goto next;
                }
            } else if (sRank == 2) {
                gGT->podium_modelIndex_Third = (char)D_80086E84[driver->driverID] + 0x7e;
                goto next;
            }
        }
    next:
        iDriver = iDriver + 1;
        pDrivers = pDrivers + 1;
    } while (iDriver < 8);
}
