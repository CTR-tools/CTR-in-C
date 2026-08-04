/*
 * UI_INSTANCE_BirthWithThread @ 0x8004CAE8 (SCUS_944.26 / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (reg-coloring on a 247-insn, 8-branch dispatch; base 246 ~ tgt 247).
 * Real types (uitypes.h: UiElement3D/Instance + gGT.modelPtr@0x2160/threadBucket0Thread@0x1B2C).
 * Spawns a 3D HUD instance+thread per driver (threadBuckets[0]); per model->id sets colorRGBA/lightDir/
 * flags: BIG1(0x38)->driver->instBigNum, FRUITDISP(0x37)->instFruitDisp, gem/relic/key/crystal (+spec
 * 0x20000), C/T/R letters (spin vel -4/0/+4, +0x30000), token (color from AdvCupColor[metaDataLEV
 * [levelID].ctrTokenGroupID], +0x30000). Position from g_apHudLayout[numPlyr-1][idx] via UI_ConvertX/Y_2
 * (or (0,0,0x200)+flag 0x100 if a pushBuffer is given -> InstDrawPerPlayer @ inst+0x74); scale from the
 * layout; optional faceCamera tilt (ratan2); inits UiElement3D.m (ConvertRotToMatrix), rot[3]=0x1000.
 * Phase B (saphi UI_Instance.c:4): CLEAN — behaviorally equivalent across all 8 model branches +
 * position/scale/rot; differences (flat if/else chain, vel=(id-T)*4, cached scale, AdvCups struct) neutral.
 */
#include "../uitypes.h"

extern struct GameTracker *D_8008D2AC;      /* sdata->gGT */
extern struct UiElement2D *D_8008625C[];    /* g_apHudLayout[numPlyrCurrGame-1] */
extern char D_80083A80[];                   /* data.metaDataLEV (stride 0x18; ctrTokenGroupID @ +0x12) */
extern char D_80084114[];                   /* g_aAdvCupColor (stride 8; colors @ +2/+4/+6) */

extern struct Thread    *func_8004205C(int spec, void *threadParam, char *name, struct Thread *parent); /* PROC_BirthWithObject */
extern struct Instance  *func_800308E4(struct Model *model, char *a1, struct Thread *th);               /* INSTANCE_Birth2D */
extern int  func_8004CAA8(int x, int z);    /* UI_ConvertX_2 */
extern int  func_8004CAC8(int y, int z);    /* UI_ConvertY_2 */
extern long func_8007173C(int y, int x);    /* ratan2 */
extern void func_8006C2A4(MATRIX *m, short *rot);  /* ConvertRotToMatrix */

struct Instance *UI_INSTANCE_BirthWithThread(int modelIndex, void *threadParam, int hudElementIndex, int faceCamera,
                               struct Instance *pushBuffer, char *name)
{
    struct Model *model;
    struct UiElement2D *pHudLayout;
    struct Instance *pInst;
    struct Thread *pDriverThread;
    struct Driver *pDriver;
    struct Thread *th;
    struct UiElement3D *pElem;
    int modelId;
    unsigned int uColor;
    short sScale;
    short rot[3];

    model = D_8008D2AC->modelPtr[modelIndex];
    pHudLayout = D_8008625C[D_8008D2AC->numPlyrCurrGame - 1];
    pInst = 0;
    if (model != 0) {
        for (pDriverThread = D_8008D2AC->threadBucket0Thread; pDriverThread != 0;
             pDriverThread = pDriverThread->siblingThread) {
            pDriver = (struct Driver *)pDriverThread->object;
            th = func_8004205C(0x380310, threadParam, name, 0);
            pElem = (struct UiElement3D *)th->object;
            pInst = func_800308E4(model, 0, th);
            th->inst = pInst;
            if (model->id == 0x38) {
                pDriver->instBigNum = pInst;
            } else if (model->id == 0x37) {
                pDriver->instFruitDisp = pInst;
            }
            modelId = model->id;
            if (modelId == 0x5f) {          /* GEM */
                uColor = 0x6c08080;
            LAB_cc4c:
                pElem->lightDir[0] = -0xc98;
                pElem->lightDir[1] = 0x99f;
                pElem->lightDir[2] = 0x232;
            LAB_cc58:
                pInst->colorRGBA = uColor;
                pInst->flags |= 0x20000;
            } else {
                if (modelId == 0x61) {      /* RELIC */
                    uColor = 0x60a5ff0;
                    goto LAB_cc4c;
                }
                if (modelId == 0x60) {      /* CRYSTAL */
                    pElem->lightDir[0] = -0xb60;
                    pElem->lightDir[1] = 0xb60;
                    uColor = 0xd22fff0;
                    pElem->lightDir[2] = -0x2d8;
                    goto LAB_cc58;
                }
                if (modelId == 0x63) {      /* KEY */
                    uColor = 0xdca6000;
                    goto LAB_cc4c;
                }
                if ((unsigned short)(model->id - 0x93) < 3) {   /* C/T/R letters */
                    pElem->lightDir[0] = -0xc98;
                    pElem->lightDir[1] = 0x99f;
                    pElem->lightDir[2] = 0x232;
                    pElem->vel[1] = 0xc;
                    modelId = model->id;
                    if (modelId == 0x93) {
                        sScale = -4;
                    LAB_ccc8:
                        pElem->vel[0] = sScale;
                    } else if (modelId == 0x94) {
                        pElem->vel[0] = 0;
                    } else if (modelId == 0x95) {
                        sScale = 4;
                        goto LAB_ccc8;
                    }
                    pInst->colorRGBA = 0xffc8000;
                    pInst->flags |= 0x30000;
                } else if (modelId == 0x7d) {   /* TOKEN */
                    unsigned short tokenGroup = *(unsigned short *)(D_80083A80 + D_8008D2AC->levelID * 0x18 + 0x12);
                    int off;
                    pElem->lightDir[0] = -0xc98;
                    pElem->lightDir[1] = 0x99f;
                    pElem->lightDir[2] = 0x232;
                    off = (int)(tokenGroup << 0x10) >> 0xd;
                    pInst->flags |= 0x30000;
                    pInst->colorRGBA = ((int)*(short *)(D_80084114 + off + 2) << 0x14) |
                                       ((int)*(short *)(D_80084114 + off + 4) << 0xc) |
                                       ((int)*(short *)(D_80084114 + off + 6) << 4);
                }
            }
            if (pushBuffer == 0) {
                pInst->matrix.t[0] = func_8004CAA8(pHudLayout[hudElementIndex].x, pHudLayout[hudElementIndex].z);
                pInst->matrix.t[1] = func_8004CAC8(pHudLayout[hudElementIndex].y, pHudLayout[hudElementIndex].z);
                pInst->matrix.t[2] = pHudLayout[hudElementIndex].z;
            } else {
                unsigned int instFlags = pInst->flags;
                *(struct Instance **)((char *)pInst + 0x74) = pushBuffer;
                pInst->flags = instFlags | 0x100;
                pInst->matrix.t[0] = 0;
                pInst->matrix.t[1] = 0;
                pInst->matrix.t[2] = 0x200;
            }
            pInst->scale[0] = pHudLayout[hudElementIndex].scale;
            pInst->scale[1] = pHudLayout[hudElementIndex].scale;
            sScale = pHudLayout[hudElementIndex].scale;
            pInst->unk50 = 0x80;
            pInst->unk51 = 0x80;
            pInst->scale[2] = sScale;
            if (faceCamera == 0) {
                rot[0] = 0;
            } else {
                long lTilt = func_8007173C(pInst->matrix.t[1], pInst->matrix.t[2]);
                rot[0] = -(short)lTilt;
            }
            rot[1] = 0;
            rot[2] = 0;
            func_8006C2A4(&pElem->m, rot);
            pElem->rot[0] = 0;
            pElem->rot[1] = 0;
            pElem->rot[2] = 0;
            pElem->rot[3] = 0x1000;
            pHudLayout += 0x14;
        }
    }
    return pInst;
}
