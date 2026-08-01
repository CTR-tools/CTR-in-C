/*
 * VehFrameProc_Driving @ 0x8005B178 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 1060, Branch 0 / Stack 0, Register 24 (gcc-2.8.1
 * reg-alloc wall). Finished from the "aibox" WIP set. Delivered NATURAL: the prior
 * seed forced 520 via four `register ... asm("a1"/"v1"/"v0")` pins + a
 * `(unsigned long long)8` type-pun (all barred by the no-hacky-match mandate); this is
 * the pin-free form, verified behaviorally identical to the retail asm + saphi
 * game/Vehicle/VehFrame.c. Rejected a permuter 970 candidate that split the frame
 * expression so `>>1` operated on the divisor result instead of the old frame count
 * (asm `sra $v0,$s0,1; addu $s0,$a1,$v0` = `+ (oldFrames>>1)`) — a false-friend.
 *
 * Selects the driving animation (idle/turn/burn) and advances the anim frame:
 * gear-change transition when desiredAnim != inst->animIndex, else the burn-timer /
 * steer-mapped target frame. `((burnTimer>>5) %% 5)*4 - 8` via the 0x66666667 div-by-5.
 *
 * saphi VehFrameProc_Driving audited CLEAN vs this asm + Ghidra.
 */

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
extern int func_8005B0F4(int, uint);
extern int func_8005B0C4(uint, int);
extern int func_80058F54(int, uint, int);
extern uint func_80058F9C(int, int, int, int, int);
extern int func_80040308(int, uint, void *);
extern byte *D_8008D2AC;
extern short D_80086E84;
extern int D_80089974;
void func_8005B178(int thread, int driver)
{
  int currAnim;
  byte *gGT;
  char tmpChar;
  ushort newFrame;
  int jumpVal;
  int iVar5;
  int sVar6;
  int iVar7;
  uint animSpeed;
  int iVar9;
  int turnStateA;
  int inst;
  uint desiredAnim;
  int *new_var;
  uint turnRange;
  inst = *((int *) (thread + 0x34));
  desiredAnim = 0;
  new_var = &(*((int *) (driver + 0x2d8)));
  if (((*((int *) (driver + 0x18))) == 0) && ((*((char *) (driver + 0x376))) != '\n'))
  {
    if ((*((short *) (driver + 0x39e))) < 0)
    {
      desiredAnim = (uint) ((*((short *) (driver + 0x38e))) < 1);
    }
    if (((0x600 < (*((short *) (driver + 0x390)))) || ((*((char *) (inst + 0x52))) == '\x03')) && (0x8000 < ((*new_var) - (*((int *) (driver + 0x2d0))))))
    {
      desiredAnim = 3;
    }
  }
  iVar5 = func_8005B0F4(inst, (uint) (*((byte *) (inst + 0x52))));
  if (0 < iVar5)
  {
    currAnim = *((byte *) (inst + 0x52));
    if (desiredAnim != ((uint) currAnim))
    {
      goto gear_change;
    }
    LAB_common:
    if (desiredAnim != 0)
    {
      if (desiredAnim == 3)
      {
        goto uVar11_is_3;
      }
      iVar5 = iVar5 + (-1);
    }
    else
    {
      if ((*((int *) (driver + 0x18))) != 0)
      {
        iVar5 >>= 1;
      }
      else
      {
        sVar6 = (short) (*((ushort *) (driver + 0x402)));
        iVar9 = sVar6;
        if ((iVar9 != 0) && (sVar6 <= 0x1df))
        {
          gGT = D_8008D2AC;
          iVar5 = (((16 * (4096 * (((int) ((short) (sVar6 >> 5))) % 5))) >> 0xe) + (-8)) + (iVar5 >> 1);
          *((short *) (inst + 0x54)) = (short) iVar5;
          iVar5 = driver + 0x4a;
          iVar7 = func_80040308(0, *((uint *) (gGT + 0x2118)), &D_80089974);
          if (iVar7 != 0)
          {
            *((char *) (iVar7 + 0x18)) = *((char *) ((*((int *) (driver + 0x1c))) + 0x50));
            *((uint *) (iVar7 + 0x20)) = *((uint *) (driver + 0x1c));
            *((char *) (iVar7 + 0x19)) = *((char *) iVar5);
          }
        }
        else
          if (((*((uint *) (driver + 0x2c8))) & 8) != 0)
        {
          turnRange = 0x40;
          iVar7 = (int) (*((short *) (driver + 0xc0)));
          jumpVal = iVar7;
          iVar5 = (int) func_80058F9C(-jumpVal, -0x40, turnRange, 0, iVar5 + (-1));
        }
        else
        {
          iVar7 = (int) (*((signed char *) (driver + 0x4b)));
          turnRange = (uint) (*((byte *) (driver + 0x43a)));
          iVar9 = -((int) turnRange);
          turnStateA = iVar7;
          iVar5 = (int) func_80058F9C(-turnStateA, iVar9, turnRange, 0, iVar5 + (-1));
        }
      }
      uVar11_is_3:
      {
        newFrame = (ushort) func_80058F54((int) (*((short *) (inst + 0x54))), 1, iVar5 + (-1));
        *((ushort *) (inst + 0x54)) = newFrame;
        if ((*((char *) (driver + 0x376))) == '\x05')
        {
          return;
        }
        sVar6 = (&D_80086E84)[*((byte *) (driver + 0x4a))];
        if (sVar6 == 0xd)
        {
          sVar6 = 3;
        }
        if (sVar6 == 0xe)
        {
          sVar6 = 0;
        }
        tmpChar = sVar6 + 7;
        if (sVar6 == 0xf)
        {
          sVar6 = 0;
          ;
        }
        *((char *) (driver + 0x4c)) = sVar6 + 7;
        *((char *) (driver + 0x4d)) = *((char *) (inst + 0x54));
        return;
      }

    }

    newFrame = (ushort) func_80058F54((int) (*((short *) (inst + 0x54))), 1, iVar5);
    *((ushort *) (inst + 0x54)) = newFrame;
    return;
    gear_change:
    {
      if (currAnim == 2)
      {
        iVar5 = func_8005B0F4(inst, (uint) currAnim);
        iVar5 = iVar5 + (-1);
      }
      else
      {
        iVar5 = func_8005B0C4((uint) currAnim, iVar5);
      }
      if (((int) (*((short *) (inst + 0x54)))) != iVar5)
      {
        goto mismatch_path;
      }
      iVar5 = func_8005B0F4(inst, desiredAnim);
      if (iVar5 < 1)
      {
        return;
      }
      *((char *) (inst + 0x52)) = (char) desiredAnim;
      newFrame = (ushort) func_8005B0C4(desiredAnim, iVar5);
      *((ushort *) (inst + 0x54)) = newFrame;
      *((char *) (driver + 0x4c)) = 0;
      *((char *) (driver + 0x4d)) = 0;
      goto LAB_common;
      mismatch_path:
      {
        do
        {
          if ((*((char *) (inst + 0x52))) == '\0')
          {
            goto mismatch_case0;
          }
          if ((*((char *) (inst + 0x52))) == '\x02')
          {
            iVar9 = -((int) turnRange);
            goto mismatch_case2;
          }
          animSpeed = 2;
          goto mismatch_call;
          mismatch_case0:
          {
            animSpeed = 6;
            goto mismatch_call;
          }

          mismatch_case2:
          {
            animSpeed = 1;
            *((char *) (driver + 0x4d)) = *((char *) (inst + 0x54));
          }

          mismatch_call:
          {
            newFrame = (ushort) func_80058F54((int) (*((short *) (inst + 0x54))), animSpeed, iVar5);
            *((ushort *) (inst + 0x54)) = newFrame;
            if (1 < (((uint) (*((byte *) (inst + 0x52)))) - 2))
            {
              return;
            }
            tmpChar = (*((char *) (driver + 0x4d)) = *((char *) (inst + 0x54)));
            if (tmpChar != '\0')
            {
              return;
            }
            *((char *) (driver + 0x4c)) = 0;
            return;
          }

        }
        while (0);
      }

    }

  }
  *((char *) (driver + 0x4d)) = 0;
  return;
}
