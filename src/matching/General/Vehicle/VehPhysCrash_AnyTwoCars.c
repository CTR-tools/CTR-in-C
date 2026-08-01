/*
 * VehPhysCrash_AnyTwoCars @ 0x8005D404 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact reference: base score 57535, Branch Differences 0, Stack
 * Differences 0, Register Differences 123. LARGE STRUCTURAL RESIDUAL — the 4
 * near-duplicate mass-weighted-impulse blocks (each with three fixed-point
 * divisions) schedule very differently from retail, so this is behavior-exact
 * and behaviorally VERIFIED but NOT byte-close; a dedicated hand-match would be
 * needed to approach score 0. Clean natural C (no hacks); the decompiler's
 * empty div-trap `if(x==0){}`/`if((x==-1)&&(n==INT_MIN)){}` blocks were removed
 * (codegen-neutral — the `/` operator emits those traps itself). From aibox staging.
 *
 * Behavior (param_1=thread, param_2=search, param_3=selfVel), verified vs asm +
 * Ghidra + saphi game/Vehicle/VehPhysCrash.c:283:
 *   distance = MATH_FastSqrt(search->bestDistSq); hitDir = distance? dist<<12/distance : (0,0,0x1000).
 *   hitStrength = (thread->hitRadius + otherThread->hitRadius) - distance; gate > 0.
 *   ACTION_BOT (0x100000) 4-way on self/other: both-human uses otherDriver->velocity
 *   directly; other-bot / self-bot paths ConvertSpeedToVec the bot's aiPhysics accel
 *   (xSpeed@0x3a0 + botAccel@0x5d8). Each path: WeightedVelocity(by weight@0x47c) ->
 *   BouncePair(2x BounceSelf) -> selfVel += hitDir*hitStrength>>8, otherVel -= same.
 *   self-not-bot tail: impact(D_8008D9F4)>0x200 -> PlayHumanFeedback (SFX/shock,
 *   flag 0x10000000) gated on frameTimer-lastTime>=3; then Attack(self,other,0)/Attack(other,self,1).
 *   self-bot path returns before the tail.
 *
 * saphi VehPhysCrash_AnyTwoCars audited CLEAN vs this asm + Ghidra.
 */
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef char undefined;
typedef short undefined2;
typedef int undefined4;

extern char *PTR_DAT_8008d2ac;
extern int DAT_8008d9f4;
extern int DAT_8008d838;
extern short DAT_80086e84[];
extern int func_8003D214(int, int);
extern int func_8005CF64(short *, int *, int *, int);
extern void func_8005D0D0(int, int *);
extern int func_8005D218(int, int, int, int);
extern void func_8005E104(int, int *);
extern void func_8005CD1C(int, int *, int);
extern uint func_80058F9C(int, int, int, int, int);
extern void func_8002E760(uint, int);
extern void func_8002CBE8(int, int, int);
extern void func_80026440(int, int, int);
extern void func_800264C0(int, int, int);
extern void func_800263A0(int, int, int);
extern void func_80016EC8(int, int);

void VehPhysCrash_AnyTwoCars(int param_1, int param_2, int *param_3)
{
  short uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int uVar9;
  uint uVar10;
  int local_48;
  int local_44;
  int local_40;
  int local_38;
  int local_34;
  int local_30;

  iVar2 = func_8003D214(*(undefined4 *)(param_2 + 0xc), 0);

  uVar1 = 0x1000;
  if (iVar2 == 0) {
    *(undefined2 *)(param_2 + 0x18) = 0;
    *(undefined2 *)(param_2 + 0x1a) = 0;
  }
  else {
    iVar8 = (int)*(short *)(param_2 + 0x10) << 0xc;
    iVar5 = (int)*(short *)(param_2 + 0x12) << 0xc;
    iVar3 = (int)*(short *)(param_2 + 0x14) << 0xc;
    uVar1 = (short)(iVar3 / iVar2);
    *(undefined2 *)(param_2 + 0x18) = (short)(iVar8 / iVar2);
    *(undefined2 *)(param_2 + 0x1a) = (short)(iVar5 / iVar2);
  }
  *(undefined2 *)(param_2 + 0x1c) = uVar1;

  iVar8 = *(int *)(*(int *)(param_2 + 8) + 0x30);
  iVar5 = *(int *)(param_1 + 0x30);

  iVar2 =
      ((int)*(short *)(param_1 + 0x42) +
       (int)*(short *)(*(int *)(*(int *)(iVar8 + 0x1c) + 0x6c) + 0x42))
      - iVar2;

  if (0 < iVar2) {
    DAT_8008d9f4 = 0;

    if ((*(uint *)(iVar5 + 0x2c8) & 0x100000) == 0) {
      iVar3 = param_2 + 0x18;

      if ((*(uint *)(iVar8 + 0x2c8) & 0x100000) == 0) {
        iVar7 = (int)*(short *)(iVar5 + 0x47c) + (int)*(short *)(iVar8 + 0x47c);
        iVar6 = *param_3 * (int)*(short *)(iVar5 + 0x47c) +
                *(int *)(iVar8 + 0x88) * (int)*(short *)(iVar8 + 0x47c);
        local_38 = iVar6 / iVar7;
        iVar7 = (int)*(short *)(iVar5 + 0x47c) + (int)*(short *)(iVar8 + 0x47c);
        iVar6 = param_3[1] * (int)*(short *)(iVar5 + 0x47c) +
                *(int *)(iVar8 + 0x8c) * (int)*(short *)(iVar8 + 0x47c);
        local_34 = iVar6 / iVar7;
        iVar7 = (int)*(short *)(iVar5 + 0x47c) + (int)*(short *)(iVar8 + 0x47c);
        iVar6 = param_3[2] * (int)*(short *)(iVar5 + 0x47c) +
                *(int *)(iVar8 + 0x90) * (int)*(short *)(iVar8 + 0x47c);
        local_30 = iVar6 / iVar7;

        iVar6 = func_8005CF64((short *)iVar3, &local_38, (int *)(iVar8 + 0x88), 1);
        if (iVar6 < 0) {
          DAT_8008d9f4 = 0;
        }
        iVar3 = func_8005CF64((short *)iVar3, &local_38, param_3, 0);
        if (0 < iVar3) {
          DAT_8008d9f4 = 0;
        }
        *param_3 = *param_3 + (*(short *)(param_2 + 0x18) * iVar2 >> 8);
        param_3[1] = param_3[1] + (*(short *)(param_2 + 0x1a) * iVar2 >> 8);
        param_3[2] = param_3[2] + (*(short *)(param_2 + 0x1c) * iVar2 >> 8);
        *(int *)(iVar8 + 0x88) = *(int *)(iVar8 + 0x88) - (*(short *)(param_2 + 0x18) * iVar2 >> 8);
        *(int *)(iVar8 + 0x8c) = *(int *)(iVar8 + 0x8c) - (*(short *)(param_2 + 0x1a) * iVar2 >> 8);
        *(int *)(iVar8 + 0x90) = *(int *)(iVar8 + 0x90) - (*(short *)(param_2 + 0x1c) * iVar2 >> 8);
      }
      else {
        local_48 = *(int *)(iVar8 + 0x3a0) + *(int *)(iVar8 + 0x5d8);
        local_44 = *(int *)(iVar8 + 0x3a4) + *(int *)(iVar8 + 0x5dc);
        local_40 = *(int *)(iVar8 + 0x3a8) + *(int *)(iVar8 + 0x5e0);
        iVar7 = (int)*(short *)(iVar5 + 0x47c) + (int)*(short *)(iVar8 + 0x47c);
        iVar6 = *param_3 * (int)*(short *)(iVar5 + 0x47c) +
                local_48 * (int)*(short *)(iVar8 + 0x47c);
        local_38 = iVar6 / iVar7;
        iVar7 = (int)*(short *)(iVar5 + 0x47c) + (int)*(short *)(iVar8 + 0x47c);
        iVar6 = param_3[1] * (int)*(short *)(iVar5 + 0x47c) +
                local_44 * (int)*(short *)(iVar8 + 0x47c);
        local_34 = iVar6 / iVar7;
        iVar7 = (int)*(short *)(iVar5 + 0x47c) + (int)*(short *)(iVar8 + 0x47c);
        iVar6 = param_3[2] * (int)*(short *)(iVar5 + 0x47c) +
                local_40 * (int)*(short *)(iVar8 + 0x47c);
        local_30 = iVar6 / iVar7;

        iVar6 = func_8005CF64((short *)iVar3, &local_38, &local_48, 1);
        if (iVar6 < 0) {
          DAT_8008d9f4 = 0;
        }
        iVar3 = func_8005CF64((short *)iVar3, &local_38, param_3, 0);
        if (0 < iVar3) {
          DAT_8008d9f4 = 0;
        }
        *param_3 = *param_3 + (*(short *)(param_2 + 0x18) * iVar2 >> 8);
        param_3[1] = param_3[1] + (*(short *)(param_2 + 0x1a) * iVar2 >> 8);
        param_3[2] = param_3[2] + (*(short *)(param_2 + 0x1c) * iVar2 >> 8);
        local_48 = local_48 - (*(short *)(param_2 + 0x18) * iVar2 >> 8);
        local_44 = local_44 - (*(short *)(param_2 + 0x1a) * iVar2 >> 8);
        local_40 = local_40 - (*(short *)(param_2 + 0x1c) * iVar2 >> 8);
        func_8005D0D0(iVar8, &local_48);
      }
      uVar10 = (uint)((uint)(*(int *)(PTR_DAT_8008d2ac + 0x1cf8) - DAT_8008d838) < 3) ^ 1;
      if (0x200 < DAT_8008d9f4) {
        if ((((*(short *)(param_1 + 0x44) == 0x18) ||
              (*(short *)(*(int *)(param_2 + 8) + 0x44) == 0x18)) &&
             (uVar4 = func_80058F9C(DAT_8008d9f4, 0, 0x1900, 0x3f, 0xff), uVar10 != 0)) &&
            ((*(char *)(iVar5 + 0x376) != (char)6 && (*(int *)(iVar5 + 0x24) == 0)) &&
             ((*(char *)(iVar8 + 0x376) != (char)6 && (*(int *)(iVar8 + 0x24) == 0))))) {
          func_8002E760((uint)*(ushort *)(iVar5 + 0x2ca) & 1, uVar4);
          DAT_8008d838 = *(int *)(PTR_DAT_8008d2ac + 0x1cf8);
          if (0xdc < uVar4) {
            func_8002CBE8(5, (int)(short)DAT_80086e84[*(byte *)(iVar5 + 0x4a)], 0x10);
          }
        }
        func_80026440(iVar8, 8, 0);
        func_800264C0(iVar8, 8, 0x7f);
        if (*(char *)(iVar8 + 0x4b) < (char)1) {
          uVar9 = 0x19;
        }
        else {
          uVar9 = 0x29;
        }
        func_800263A0(iVar8, uVar9, 0x60);
        func_80026440(iVar5, 8, 0);
        func_800264C0(iVar5, 8, 0x7f);
        if (*(char *)(iVar5 + 0x4b) < (char)1) {
          uVar9 = 0x19;
        }
        else {
          uVar9 = 0x29;
        }
        func_800263A0(iVar5, uVar9, 0x60);
        *(uint *)(iVar5 + 0x2c8) = *(uint *)(iVar5 + 0x2c8) | 0x10000000;
        *(uint *)(iVar8 + 0x2c8) = *(uint *)(iVar8 + 0x2c8) | 0x10000000;
      }
      uVar9 = func_8005D218(iVar5, iVar8, uVar10, 0);
      func_8005D218(iVar8, iVar5, uVar9, 1);
    }
    else {
      if ((*(uint *)(iVar8 + 0x2c8) & 0x100000) == 0) {
        func_8005E104(iVar8, &local_48);
        iVar6 = (int)*(short *)(iVar5 + 0x47c) + (int)*(short *)(iVar8 + 0x47c);
        iVar3 = *param_3 * (int)*(short *)(iVar5 + 0x47c) +
                local_48 * (int)*(short *)(iVar8 + 0x47c);
        local_38 = iVar3 / iVar6;
        iVar6 = (int)*(short *)(iVar5 + 0x47c) + (int)*(short *)(iVar8 + 0x47c);
        iVar3 = param_3[1] * (int)*(short *)(iVar5 + 0x47c) +
                local_44 * (int)*(short *)(iVar8 + 0x47c);
        local_34 = iVar3 / iVar6;
        iVar6 = (int)*(short *)(iVar5 + 0x47c) + (int)*(short *)(iVar8 + 0x47c);
        iVar3 = param_3[2] * (int)*(short *)(iVar5 + 0x47c) +
                local_40 * (int)*(short *)(iVar8 + 0x47c);
        local_30 = iVar3 / iVar6;

        iVar3 = func_8005CF64((short *)(param_2 + 0x18), &local_38, &local_48, 1);
        if (iVar3 < 0) {
          DAT_8008d9f4 = 0;
        }
        iVar3 = func_8005CF64((short *)(param_2 + 0x18), &local_38, param_3, 0);
        if (0 < iVar3) {
          DAT_8008d9f4 = 0;
        }
        *param_3 = *param_3 + (*(short *)(param_2 + 0x18) * iVar2 >> 8);
        param_3[1] = param_3[1] + (*(short *)(param_2 + 0x1a) * iVar2 >> 8);
        param_3[2] = param_3[2] + (*(short *)(param_2 + 0x1c) * iVar2 >> 8);
        local_48 = local_48 - (*(short *)(param_2 + 0x18) * iVar2 >> 8);
        local_44 = local_44 - (*(short *)(param_2 + 0x1a) * iVar2 >> 8);
        local_40 = local_40 - (*(short *)(param_2 + 0x1c) * iVar2 >> 8);
        func_8005D0D0(iVar5, param_3);
        func_8005CD1C(iVar8, &local_48, 0);
      }
      else {
        local_48 = *(int *)(iVar8 + 0x3a0) + *(int *)(iVar8 + 0x5d8);
        local_44 = *(int *)(iVar8 + 0x3a4) + *(int *)(iVar8 + 0x5dc);
        local_40 = *(int *)(iVar8 + 0x3a8) + *(int *)(iVar8 + 0x5e0);
        iVar6 = (int)*(short *)(iVar5 + 0x47c) + (int)*(short *)(iVar8 + 0x47c);
        iVar3 = *param_3 * (int)*(short *)(iVar5 + 0x47c) +
                local_48 * (int)*(short *)(iVar8 + 0x47c);
        local_38 = iVar3 / iVar6;
        iVar6 = (int)*(short *)(iVar5 + 0x47c) + (int)*(short *)(iVar8 + 0x47c);
        iVar3 = param_3[1] * (int)*(short *)(iVar5 + 0x47c) +
                local_44 * (int)*(short *)(iVar8 + 0x47c);
        local_34 = iVar3 / iVar6;
        iVar6 = (int)*(short *)(iVar5 + 0x47c) + (int)*(short *)(iVar8 + 0x47c);
        iVar3 = param_3[2] * (int)*(short *)(iVar5 + 0x47c) +
                local_40 * (int)*(short *)(iVar8 + 0x47c);
        local_30 = iVar3 / iVar6;

        iVar3 = func_8005CF64((short *)(param_2 + 0x18), &local_38, &local_48, 1);
        if (iVar3 < 0) {
          DAT_8008d9f4 = 0;
        }
        iVar3 = func_8005CF64((short *)(param_2 + 0x18), &local_38, param_3, 0);
        if (0 < iVar3) {
          DAT_8008d9f4 = 0;
        }
        *param_3 = *param_3 + (*(short *)(param_2 + 0x18) * iVar2 >> 8);
        param_3[1] = param_3[1] + (*(short *)(param_2 + 0x1a) * iVar2 >> 8);
        param_3[2] = param_3[2] + (*(short *)(param_2 + 0x1c) * iVar2 >> 8);
        local_48 = local_48 - (*(short *)(param_2 + 0x18) * iVar2 >> 8);
        local_44 = local_44 - (*(short *)(param_2 + 0x1a) * iVar2 >> 8);
        local_40 = local_40 - (*(short *)(param_2 + 0x1c) * iVar2 >> 8);
        func_8005D0D0(iVar8, &local_48);
        func_8005D0D0(iVar5, param_3);
        func_80016EC8(iVar5, iVar8);
      }
    }
  }
  return;
}
