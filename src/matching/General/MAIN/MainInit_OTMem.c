// MainInit_OTMem @ 0x8003b334  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 1415, Reg 27; 66 vs 67 insns).
// Residual: gcc tail-merges BOTH per-branch MainDB_OTMem calls (retail shares only
// the second) + a saved-reg-count detail. Behavior identical.
// Sizes the OT memory for both draw buffers by game mode: MAIN_MENU(0x2000)->0x1800,
// ADVENTURE_ARENA(0x100000)->0x2c00, BATTLE_MODE(0x20)->0x8000, 1P/2P->0x2000,
// 3P/4P->0x3000 (per-branch MainDB_OTMem(db[0].otMem@0xA8) + MainDB_OTMem(db[1].otMem@0x14C)),
// then allocs the two OT-swapchain buffers (numPlyr<<12 | 0x18) into otSwapchainDB[0/1]@0x18C8.
#include "CTR.h"

#define MAIN_MENU       0x2000
#define ADVENTURE_ARENA 0x100000
#define BATTLE_MODE     0x20

struct GameTracker {
    int gameMode1;                                  /* 0x0    */
    char pad_A8[0xA8 - 0x4];
    char db0_otMem[0x10];                           /* 0xA8   db[0].otMem */
    char pad_14C[0x14C - (0xA8 + 0x10)];
    char db1_otMem[0x10];                           /* 0x14C  db[1].otMem */
    char pad_18C8[0x18C8 - (0x14C + 0x10)];
    void *otSwapchainDB[2];                         /* 0x18C8 / 0x18CC */
    char pad_1CA8[0x1CA8 - (0x18C8 + 8)];
    unsigned char numPlyrCurrGame;                  /* 0x1CA8 */
};

extern void  func_80034A28(void *otMem, int size);   /* MainDB_OTMem */
extern void *func_8003E874(int size, char *name);    /* MEMPACK_AllocMem */
extern char D_8008D2FC[];   /* "ot1" */
extern char D_8008D304[];   /* "ot2" */

void MainInit_OTMem(struct GameTracker *gGT)
{
    int gameMode = gGT->gameMode1;
    int size;

    if ((gameMode & MAIN_MENU) != 0)
    {
        func_80034A28(gGT->db0_otMem, 0x1800);
        func_80034A28(gGT->db1_otMem, 0x1800);
    }
    else if ((gameMode & ADVENTURE_ARENA) != 0)
    {
        func_80034A28(gGT->db0_otMem, 0x2c00);
        func_80034A28(gGT->db1_otMem, 0x2c00);
    }
    else if ((gameMode & BATTLE_MODE) != 0)
    {
        func_80034A28(gGT->db0_otMem, 0x8000);
        func_80034A28(gGT->db1_otMem, 0x8000);
    }
    else if (gGT->numPlyrCurrGame < 3)
    {
        func_80034A28(gGT->db0_otMem, 0x2000);
        func_80034A28(gGT->db1_otMem, 0x2000);
    }
    else
    {
        func_80034A28(gGT->db0_otMem, 0x3000);
        func_80034A28(gGT->db1_otMem, 0x3000);
    }

    size = (gGT->numPlyrCurrGame << 0xC) | 0x18;
    gGT->otSwapchainDB[0] = func_8003E874(size, D_8008D2FC);
    gGT->otSwapchainDB[1] = func_8003E874(size, D_8008D304);
}
