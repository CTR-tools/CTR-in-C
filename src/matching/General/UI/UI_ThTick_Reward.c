/*
 * UI_ThTick_Reward @ 0x8004C850 (func_8004C850, SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact: base score 275, Branch 0 / Stack 0, Register 2-3 residual.
 * vs SCUS_944.26.exe; finished from the "near completed" prior grind set.
 *
 * The residual is a pure instruction-SCHEDULING wall: retail hoists the clear-mask
 * constant (li v1, -0x81) up into the `beqz` delay slot so it is ready for the CLEAR
 * block, whereas gcc-2.8.1 emits a nop in that slot and materialises the constant
 * lazily inside the block. Control flow, stack, and behavior are identical.
 *
 * NOTE: the prior grind set contained a score-5 snapshot for this function that is
 * BEHAVIORALLY WRONG — it collapses to `flags = val & (val | 0x80)` (== val), so it
 * never sets bit 0x80 on the SET path. Its low score was instruction-similarity, not
 * correctness. This natural version is verified correct against the retail asm.
 *
 * Per-thread "reward object" tick (spinning relic/key/trophy/token/crystal, etc).
 * Reference field names from src/decompile/General/UI/UI_02_ThTick_Reward.c:
 *   bucket      = struct Thread *          (param)
 *   inst        = bucket->inst             (bucket + 0x34)   struct Instance *
 *   obj         = bucket->object           (bucket + 0x30)   struct UiElement3D *
 *   obj + 0x02  = obj->rot[1]              (spin on Y axis, += 0x40)
 *   obj + 0x28  = obj->lightDir
 *   obj + 0x08  = obj->m                   (source matrix)
 *   inst + 0x30 = inst->matrix             (MATRIX *mat)
 *   inst + 0x28 = inst->flags              (bit 0x80 = invisible)
 *   D_8008D2AC  = sdata->gGT
 *   gGT + 0x1D30 = gGT->bool_DrawOTag_InProgress (hud/demo state word)
 *   gGT + 0x139A = gGT->pushBuffer_UI.fadeFromBlack_currentValue (s16)
 *   func_800572D0 = Vector_SpecLightSpin2D, func_8006C2A4 = ConvertRotToMatrix,
 *   func_8006C3B0 = MatrixRotate
 *
 * Visibility rule (matches asm exactly): the object is made VISIBLE (clear bit 0x80)
 * only while the HUD is enabled and not in demo mode (gGT->drawFlags & 0xFF0100 ==
 * 0x100) AND the fade-from-black transition has finished (fade >= 0x1000); otherwise
 * it is made INVISIBLE (set bit 0x80).
 */

typedef unsigned int uint;

extern char *D_8008D2AC;                                    /* sdata->gGT */
extern void func_800572D0(int inst, int rot, int lightDir); /* Vector_SpecLightSpin2D */
extern void func_8006C2A4(int mat, int rot);                /* ConvertRotToMatrix */
extern void func_8006C3B0(int dst, int a, int b);           /* MatrixRotate */

void UI_ThTick_Reward(int bucket)
{
    uint flags;
    int obj;
    int inst;
    int mat;

    inst = *((int *) (bucket + 0x34));
    obj = *((int *) (bucket + 0x30));

    *((short *) (obj + 2)) += 0x40;

    func_800572D0(inst, obj, obj + 0x28);

    mat = inst + 0x30;
    func_8006C2A4(mat, obj);
    func_8006C3B0(mat, obj + 8, mat);

    if (((*((uint *) (D_8008D2AC + 0x1D30)) & 0xFF0100) != 0x100) ||
        (*((short *) (D_8008D2AC + 0x139A)) < 0x1000))
        flags = *((uint *) (inst + 0x28)) | 0x80;
    else
        flags = *((uint *) (inst + 0x28)) & 0xFFFFFF7F;

    *((uint *) (inst + 0x28)) = flags;
}
