// MainFrame_VisMemFullFrame @ 0x800357b8  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 7555, Register 131; target 350 insns vs 325).
// Residual: whole-function register allocation on a high-pressure loop — retail
// keeps all s-regs live and RELOADS gGT->visMem1 (0x1A38) on every access; gcc-2.8.1
// allocates/reloads differently. Behavior identical. (grind floor ~6985 non-natural.)
// Per-frame per-camera visibility update: for each active player camera (folded base
// gGT+0x1498+i*0xdc, an irreducible db[] fold — accessed via char* arithmetic),
// refresh LEAF/FACE/INST/VERT visibility lists from the camera source or the
// under-driver PVS, packing via memcpy (src&1==0, ceil(count/32)*4 bytes) or
// CTR_VisMem_RleCopy (src&~3). FACE change decides own-path vs following-driver via
// the quad-visible test (quadIndex = (underDriver - mesh->ptrQuadBlockArray), sizeof
// QuadBlock=0x5C → div magic 0xE9BD37A7) and flag 0x2000/0x4000 + ApplyDriverPVS.
// All struct offsets traced from asm; gGT is param a0 (not gp-relative here).
// (Phase B: saphi MainFrame_VisMemFullFrame verified CLEAN — the VERT null-guard is a
//  documented intentional native-port safety adaptation, not a bug.)
#include "CTR.h"

struct PVS {
    void *visLeafSrc;   /* 0x0 */
    void *visFaceSrc;   /* 0x4 */
    void *visInstSrc;   /* 0x8 */
};

struct QuadBlock {
    char pad0[0x44];
    struct PVS *pvs;    /* 0x44 */
    char pad1[0x5C - 0x48];
};                      /* size 0x5C */

struct Driver {
    char pad0[0x350];
    struct QuadBlock *underDriver;   /* 0x350 */
};

struct MeshInfo {
    int numQuadBlock;               /* 0x0  */
    char pad_C[0xC - 0x4];
    struct QuadBlock *ptrQuadBlockArray;  /* 0xC  */
    char pad_1C[0x1C - 0x10];
    int numBspNodes;                /* 0x1C */
};

struct Level {
    struct MeshInfo *ptr_mesh_info; /* 0x0   */
    char pad_28[0x28 - 0x4];
    void *unk5;                     /* 0x28  */
    char pad_34[0x34 - 0x2C];
    int numWaterVertices;           /* 0x34  */
    char pad_DC[0xDC - 0x38];
    int configFlags;                /* 0xDC  */
    char pad_170[0x170 - 0xE0];
    void *unk_170;                  /* 0x170 */
    int numSCVert;                  /* 0x174 */
};

struct VisMem {
    void *visLeafList[4];   /* 0x0  */
    void *visFaceList[4];   /* 0x10 */
    void *visOVertList[4];  /* 0x20 */
    void *visSCVertList[4]; /* 0x30 */
    void *visLeafSrc[4];    /* 0x40 */
    void *visFaceSrc[4];    /* 0x50 */
    void *visOVertSrc[4];   /* 0x60 */
    void *visSCVertSrc[4];  /* 0x70 */
};

struct GameTracker {
    char pad_160[0x160];
    struct Level *level1;           /* 0x160 */
    char pad_1A38[0x1A38 - 0x164];
    struct VisMem *visMem1;         /* 0x1A38 */
    char pad_1CA8[0x1CA8 - 0x1A3C];
    unsigned char numPlyrCurrGame;  /* 0x1CA8 */
};

/* folded camDC base: gGT + 0x1498 + i*0xdc (overlaps other fields, so char* access) */
#define CAM(gGT, cbo, off, T) (*(T *)((char *)(gGT) + (cbo) + (off)))

extern void func_80021DA0(void *dst, void *srcMasked);              /* CTR_VisMem_RleCopy */
extern void func_80035684(struct GameTracker *gGT, int playerIdx); /* CTR_VisMem_ApplyDriverPVS */
extern void func_80077CB8(void *dst, void *src, int nbytes);       /* memcpy */

void MainFrame_VisMemFullFrame(struct GameTracker *gGT, struct Level *level)
{
    struct MeshInfo *mesh;
    int playerIndex;
    int camByteOfs;

    if (gGT->visMem1 == 0)
        return;
    if (level == 0)
        return;
    if (gGT->numPlyrCurrGame == 0)
        return;

    mesh = level->ptr_mesh_info;
    camByteOfs = 0x1498;
    for (playerIndex = 0; playerIndex < gGT->numPlyrCurrGame; playerIndex++, camByteOfs += 0xdc)
    {
        struct Driver *driver = *(struct Driver **)((char *)gGT + playerIndex * 4 + 0x24EC);
        void *leafSrc = CAM(gGT, camByteOfs, 0x20, void *);
        void *faceSrc;
        void *ovSrc;
        int flags;

        CAM(gGT, camByteOfs, 0x70, int) &= ~0x4000;

        /* ---- LEAF vis ---- */
        if (leafSrc == 0)
        {
            if (driver->underDriver != 0 && driver->underDriver->pvs != 0 &&
                driver->underDriver->pvs->visLeafSrc != 0)
            {
                void *src = driver->underDriver->pvs->visLeafSrc;
                gGT->visMem1->visLeafSrc[playerIndex] = src;
                if (((unsigned int)src & 1) != 0)
                    func_80021DA0(gGT->visMem1->visLeafList[playerIndex], (void *)((unsigned int)src & ~3));
                else
                    func_80077CB8(gGT->visMem1->visLeafList[playerIndex], src, ((mesh->numBspNodes + 0x1f) >> 5) << 2);
            }
        }
        else if (gGT->visMem1->visLeafSrc[playerIndex] != leafSrc)
        {
            gGT->visMem1->visLeafSrc[playerIndex] = leafSrc;
            if (((unsigned int)leafSrc & 1) != 0)
                func_80021DA0(gGT->visMem1->visLeafList[playerIndex], (void *)((unsigned int)leafSrc & ~3));
            else
                func_80077CB8(gGT->visMem1->visLeafList[playerIndex], leafSrc, ((mesh->numBspNodes + 0x1f) >> 5) << 2);
        }

        /* ---- FACE vis ---- */
        faceSrc = CAM(gGT, camByteOfs, 0x24, void *);
        if (faceSrc == 0)
        {
            if (driver->underDriver != 0 && driver->underDriver->pvs != 0 &&
                driver->underDriver->pvs->visFaceSrc != 0)
            {
                void *src = driver->underDriver->pvs->visFaceSrc;
                gGT->visMem1->visFaceSrc[playerIndex] = src;
                if (((unsigned int)src & 1) == 0)
                    func_80077CB8(gGT->visMem1->visFaceList[playerIndex], src, ((mesh->numQuadBlock + 0x1f) >> 5) << 2);
                else
                    func_80021DA0(gGT->visMem1->visFaceList[playerIndex], (void *)((unsigned int)src & ~3));
            }
        }
        else if (gGT->visMem1->visFaceSrc[playerIndex] != faceSrc)
        {
            struct QuadBlock *ud = driver->underDriver;
            gGT->visMem1->visFaceSrc[playerIndex] = faceSrc;
            if (((unsigned int)faceSrc & 1) == 0)
                func_80077CB8(gGT->visMem1->visFaceList[playerIndex], faceSrc, ((mesh->numQuadBlock + 0x1f) >> 5) << 2);
            else
                func_80021DA0(gGT->visMem1->visFaceList[playerIndex], (void *)((unsigned int)faceSrc & ~3));

            if (ud == 0 || ud->pvs == 0 || ud->pvs->visLeafSrc == 0 || ud->pvs->visFaceSrc == 0 ||
                ud->pvs->visInstSrc == 0 ||
                (((unsigned int *)gGT->visMem1->visFaceList[playerIndex])
                     [(ud - gGT->level1->ptr_mesh_info->ptrQuadBlockArray) >> 5] &
                 (1 << ((ud - gGT->level1->ptr_mesh_info->ptrQuadBlockArray) & 0x1f))) != 0)
            {
                CAM(gGT, camByteOfs, 0x70, int) &= ~0x2000;
            }
            else
            {
                CAM(gGT, camByteOfs, 0x70, int) |= 0x2000;
            }

            if ((CAM(gGT, camByteOfs, 0x70, int) & 0x2000) != 0)
            {
                func_80035684(gGT, playerIndex);
                CAM(gGT, camByteOfs, 0x70, int) |= 0x4000;
            }
        }

        if ((CAM(gGT, camByteOfs, 0x70, int) & 0x5000) == 0x1000)
            func_80035684(gGT, playerIndex);

        /* ---- INST vis ---- */
        if (CAM(gGT, camByteOfs, 0x9A, short) == 0 &&
            (CAM(gGT, camByteOfs, 0x70, int) & 0x2000) != 0 &&
            driver->underDriver != 0 && driver->underDriver->pvs != 0 &&
            driver->underDriver->pvs->visInstSrc != 0)
        {
            CAM(gGT, camByteOfs, 0x28, void *) = driver->underDriver->pvs->visInstSrc;
        }

        /* ---- VERT vis ---- */
        if ((level->configFlags & 4) == 0)
        {
            ovSrc = CAM(gGT, camByteOfs, 0x2C, void *);
            if (gGT->visMem1->visOVertSrc[playerIndex] != ovSrc)
            {
                gGT->visMem1->visOVertSrc[playerIndex] = ovSrc;
                if (((unsigned int)ovSrc & 1) == 0)
                    func_80077CB8(gGT->visMem1->visOVertList[playerIndex], ovSrc, ((level->numWaterVertices + 0x1f) >> 5) << 2);
                else
                    func_80021DA0(gGT->visMem1->visOVertList[playerIndex], (void *)((unsigned int)ovSrc & ~3));
            }
            else if (gGT->visMem1->visOVertSrc[playerIndex] == 0)
            {
                func_80077CB8(gGT->visMem1->visOVertList[playerIndex], level->unk5, ((level->numWaterVertices + 0x1f) >> 5) << 2);
            }
        }
        else
        {
            ovSrc = CAM(gGT, camByteOfs, 0x30, void *);
            if (gGT->visMem1->visSCVertSrc[playerIndex] != ovSrc)
            {
                gGT->visMem1->visSCVertSrc[playerIndex] = ovSrc;
                if (((unsigned int)ovSrc & 1) != 0)
                    func_80021DA0(gGT->visMem1->visSCVertList[playerIndex], (void *)((unsigned int)ovSrc & ~3));
                else
                    func_80077CB8(gGT->visMem1->visSCVertList[playerIndex], ovSrc, ((level->numSCVert + 0x1f) >> 5) << 2);
            }
            else if (gGT->visMem1->visSCVertSrc[playerIndex] == 0)
            {
                func_80077CB8(gGT->visMem1->visSCVertList[playerIndex], level->unk_170, ((level->numSCVert + 0x1f) >> 5) << 2);
            }
        }
        (void)flags;
    }
}
