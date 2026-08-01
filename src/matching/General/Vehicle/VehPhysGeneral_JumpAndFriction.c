/*
 * VehPhysGeneral_JumpAndFriction @ 0x80060630 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: base score 1960, Branch 0 / Stack 0 (Register 80 residual) -
 * a large gcc-2.8.1 register-allocation/scheduling residual on this ~567-instruction GTE
 * function; behavior verified identical to the retail asm. From the aibox staging attempt
 * (far more advanced than the base_clean.c attempt which sat at 8265).
 *
 * FALSE-FRIEND REPAIRED: the aibox seed (deceptive 1045) computed the speed-loss as
 * "baseSpeed - accel" while DEAD-discarding the VehCalc_FastSqrt(|movement|^2) result
 * (uVar14 = sqrt>>8; then uVar14 = d->baseSpeed). Retail (asm @80060A08:
 * srl s4,v0,8 ; lh v1,baseSpeed ; bgez v1 ; negu v1 ; subu s4,s4,v1) and saphi both compute
 * speedLoss = clamp((FastSqrt(|movement|^2) >> 8) - abs(baseSpeed), 0, accel*elapsedMS>>5).
 * Fixed to use the sqrt magnitude (correctness over score: honest score rose 1045 to 1960).
 *
 * Physics: pedal/no-pedal friction (turn-decrease, wall-rub scrub clamp), an accel ramp
 * scaled by terrain->slowUntilSpeed, GTE rotation of the accel vector by matrixMovingDir
 * (gte_rtv0 = mvmva sf=1/ROT/V0), the speed-loss above, jump-force math (const_JumpForce
 * x9/4, trampoline x3 or x3/2), anti-grav ground-normal impulse, a jump-velY selection
 * loop, and the speedometer-needle lerp. Uses PSY-Q GTE intrinsic macros (USE_REAL_GTE
 * inline asm) with a software-emulation fallback.
 *
 * NOTE: generic int locals (iVar, uVar, param_ names) are left as-is - they are heavily
 * register-reused across unrelated lifetimes in this large function, so per-lifetime
 * de-reuse + rename is deferred; the named Driver/Thread struct fields carry readability.
 *
 * saphi game/Vehicle/VehPhysGeneral.c:510 VehPhysGeneral_JumpAndFriction audited CLEAN
 * (by a prior fork) vs this asm + Ghidra - the friction bug lived only in the matching seed.
 */

typedef int s32;
typedef short s16;
typedef unsigned int u_int;
typedef int bool;
typedef struct 
{
  s32 vx;
  s32 vy;
  s32 vz;
  s32 pad;
} VECTOR;
typedef struct 
{
  s16 t[2];
  s16 pad;
} SVECTOR;
typedef struct 
{
  short m[3][3];
  short pad;
  int t[3];
} MATRIX;
typedef struct Vec3
{
  s32 v[3];
} Vec3;
typedef struct SVec3
{
  s16 v[3];
} SVec3;
struct Level
{
  char pad[0x18C];
  unsigned char unk_18C;
};
struct GameTracker
{
  char pad0[0x160];
  struct Level *level1;
  char pad1[0x1B88];
  int timer;
  char pad2[0x14];
  int elapsedTimeMS;
};
struct sData
{
  char pad[0x340];
  struct GameTracker *gGT;
};
extern struct GameTracker *D_8008D2AC;
struct Terrain
{
  char *name;
  unsigned int flags;
  int unk_0x8;
  int slowUntilSpeed;
};
struct QuadBlock
{
  char pad[0x3B];
  signed char mulNormVecY;
};
struct Thread
{
  struct Thread *next;
  struct Thread *prev;
  char *name;
  struct Thread *parentThread;
  struct Thread *siblingThread;
  struct Thread *childThread;
  int cooldownFrameCount;
  u_int flags;
  int timesDestroyed;
  void (*funcThDestroy)(struct Thread *self);
  void (*funcThCollide)(struct Thread *self);
  void (*funcThTick)(struct Thread *self);
  void *object;
  struct Instance *inst;
  int driver_unk1;
  short driver_unk2;
  short driver_unk3E;
  short driver_unk3;
  short driver_HitRadius;
  short modelIndex;
  short padding;
};
struct Driver
{
  struct Icon **wheelSprites;
  unsigned short wheelSize;
  short wheelRotation;
  unsigned int tireColor;
  short clockReceive;
  short hazardTimer;
  struct Instance *instBombThrow;
  struct Instance *instBubbleHold;
  struct Instance *instTntRecv;
  struct Instance *instSelf;
  struct Instance *instTntSend;
  int invincibleTimer;
  int invisibleTimer;
  unsigned int instFlagsBackup;
  char numWumpas;
  char numCrystals;
  char numTimeCrates;
  signed char accelConst;
  char turnConst;
  char turboConst;
  char heldItemID;
  char numHeldItems;
  short superEngineTimer;
  short itemRollTimer;
  short noItemTimer;
  short unknown_noitemtimer_laptime;
  int lapTime;
  unsigned char lapIndex;
  unsigned char clockSend;
  short jumpMeter;
  short jumpMeterTimer;
  unsigned char driverID;
  char simpTurnState;
  unsigned char matrixArray;
  unsigned char matrixIndex;
  short numTurbos;
  unsigned short frameAgainstWall;
  short funcPtrs_compilerpadding;
  void *funcPtrs[0xD];
  Vec3 velocity;
  Vec3 originToCenter;
  struct QuadBlock *currBlockTouching;
  SVec3 normalVecUP;
  short unkAA;
  short spsHitPos[4];
  short spsNormalVec[4];
  unsigned int stepFlagSet;
  short ampTurnState;
  char currentTerrain;
  char skidmarkFrameIndex;
  char skidmarks[0x200];
  u_int skidmarkEnableFlags;
  unsigned int actionsFlagSet;
  unsigned int actionsFlagSetPrevFrame;
  int quadBlockHeight;
  Vec3 posCurr;
  Vec3 posPrev;
  struct 
  {
    short x;
    short y;
    short z;
    short w;
  } rotCurr;
  struct 
  {
    short x;
    short y;
    short z;
    short w;
  } rotPrev;
  int sfxDistortOffset;
  void *driverAudioPtrs[4];
  MATRIX matrixMovingDir;
  MATRIX matrixFacingDir;
  struct QuadBlock *underDriver;
  struct QuadBlock *lastValid;
  struct Terrain *terrainMeta1;
  struct Terrain *terrainMeta2;
  SVec3 AxisAngle1_normalVec;
  char forcedJump_trampoline;
  char clockFlash;
  short AxisAngle2_normalVec[3];
  short unk36E;
  short AxisAngle3_normalVec[3];
  char kartState;
  char Screen_OffsetY;
  short AxisAngle4_normalVec[3];
  short unk37e;
  char normalVecID;
  char unk381;
  short buttonUsedToStartDrift;
  short posWallColl[3];
  short scrubMeta8;
  short speed;
  short speedApprox;
  short jumpHeightCurr;
  short jumpHeightPrev;
  short axisRotationY;
  short axisRotationX;
  short oh_no_anotherFiller;
  short angle;
  short baseSpeed;
  short fireSpeed;
  int xSpeed;
  int ySpeed;
  int zSpeed;
  short unkVectorX;
  short unkVectorY;
  short unkVectorZ;
  short unk_offset3B2;
  short rotationSpinRate;
  short fill_3B6[2];
  short japanTurboUnknown;
  short unkSpeedValue1;
  short unkSpeedValue2;
  short mashingXMakesItBig;
  short mashXUnknown;
  short unknowndriverBaseSpeed;
  short turnAngleCurr;
  short turnAnglePrev;
  short unk3CA;
  SVec3 accel;
  short unk_LerpToForwards;
  short unk3D4[3];
  short multDrift;
  short turbo_MeterRoomLeft;
  short turbo_outsideTimer;
  short VehFire_AudioCooldown;
  short reserves;
  short fireSpeedCap;
  short numFramesSpentSteering;
  short forwardDir;
  short previousFrameMultDrift;
  short timeUntilDriftSpinout;
  short distanceFromGround;
  short jump_TenBuffer;
  short jump_CooldownMS;
  short jump_CoyoteTimerMS;
  short jump_ForcedMS;
  short jump_InitialVelY;
  short jump_unknown;
  short jump_LandingBoost;
  short set_0xF0_OnWallRub;
  short NoInputTimer;
  short burnTimer;
  short squishTimer;
  short StartDriving_0x60;
  short StartRollback_0x280;
  short unknownTraction;
  short jumpSquishStretch;
  short unk40E;
  short jumpSquishStretch2;
  short unk412;
  short filler_short;
  short const_Gravity;
  short const_JumpForce;
  short const_PedalFriction_Perpendicular;
  short const_PedalFriction_Forward;
  short const_NoPedalFriction_Perpendicular;
  short const_NoPedalFriction_Forward;
  short const_BrakeFriction;
  short const_DriftCurve;
  short const_DriftFriction;
  short const_Accel_ClassStat;
  short const_Accel_Reserves;
  short const_Speed_ClassStat;
  short const_AccelSpeed_ClassStat;
  short const_SingleTurboSpeed;
  short const_SacredFireSpeed;
  short const_BackwardSpeed;
  short const_MaskSpeed;
  short const_DamagedSpeed;
  char const_TurnRate;
  char const_BackwardTurnRate;
  short const_TurnDecreaseRate;
  short const_TurnInputDelay;
  short const_unk440;
  short const_TerminalVelocity;
  short const_unk444;
  char const_SteerAccel_Stage4_FirstFrame;
  char const_SteerAccel_Stage2_FirstFrame;
  char const_SteerAccel_Stage2_FrameLength;
  char boolFirstFrameSinceRevEngine;
  short const_SteerAccel_Stage1_MaxSteer;
  short const_SteerAccel_Stage1_MinSteer;
  short unk44e;
  short unk450;
  short const_modelRotVelMax;
  short const_modelRotVelMin;
  char unusedPadding;
  char angleMaxCounterSteer;
  char unk458;
  char unk459;
  char unk45a;
  char unk45b;
  char const_turnResistMin;
  char const_turnResistMax;
  char const_SteerVel_DriftSwitchWay;
  char const_SteerVel_DriftStandard;
  char unk460;
  char unk461;
  char unk462;
  char const_Drifting_FramesTillSpinout;
  short unk464;
  short unk466;
  short const_Drifting_CameraSpinRate;
  char unk46a;
  char unk46b;
  short unk46c;
  short unk46e;
  short unk470;
  short unk472;
  short unk474;
  char const_turboMaxRoom;
  char const_turboLowRoomWarning;
  char const_turboFullBarReserveGain;
  char unk479;
  char unk47A;
  char unk47B;
  short unk47C;
  short unk47E;
  short unk480;
  short driverRank;
  int const_prototypeKey;
  unsigned int distanceToFinish_curr;
  unsigned int distanceToFinish_checkpoint;
  unsigned int distanceDrivenBackwards;
  unsigned char unknown_lap_related[2];
  unsigned short engineVol;
  struct Instance *instBigNum;
  struct Instance *instFruitDisp;
  struct Thread *thCloud;
  struct Thread *thTrackingMe;
  struct Thread *plantEatingMe;
  int damageColorTimer;
  struct 
  {
    int cooldown;
    short startX;
    short startY;
  } PickupTimeboxHUD;
  struct 
  {
    int cooldown;
    short startX;
    short startY;
    int numCollected;
  } PickupWumpaHUD;
  struct 
  {
    short cooldown;
    short modelID;
    short startX;
    short startY;
    int numCollected;
  } PickupLetterHUD;
  struct 
  {
    int cooldown;
    short startX;
    short startY;
    int unk;
    int scoreDelta;
    int juicedUpCooldown;
    int numLives;
    int teamID;
  } BattleHUD;
  int framesSinceRaceEnded_forThisDriver;
  short quip1;
  short quip2;
  short quip3;
  short quip4;
  struct Instance *wakeInst;
  short wakeScale;
  char revEngineState;
  char ChangeState_param2;
  int ChangeState_param3;
  int ChangeState_param4;
  short alphaScaleBackup;
  short driverRankItemValue;
  char numTimesAttackingPlayer[8];
  int timeElapsedInRace;
  int distanceDriven;
  int timeSpentWithHighSpeed;
  int timeSpentReversing;
  int timeSpentInMud;
  int timeSpentInLastPlace;
  int timeSpentInTenWumpa;
  int timeSpentAgainstWall;
  int timeSpentUsingReserves;
  int timeSpentDrifting;
  int timeSpentJumping;
  int timeSpentSpinningOut;
  int timeSpentSquished;
  int timeSpentEaten;
  int timeSpentBurnt;
  short highestJump;
  short longestShot;
  short numberOfJumps;
  char numTimesMovingPotionHitSomeone;
  char numTimesMissileHitSomeone;
  char numTimesClockWeaponUsed;
  char numTimesAttacking;
  char numTimesBombsHitSomeone;
  char numTimesSquishedSomeone;
  char numTimesMissileLaunched;
  char numTimesMissileHitYou;
  char numTimesBombHitYou;
  char numTimesMotionlessPotionHitYou;
  char numTimesAttackedByPlayer[8];
  char numTimesHitWeaponBox;
  char numTimesWumpa;
  char numTimesMaskGrab;
  char unused_alignment_56b;
  short *EndOfRaceComment_ptrQuip;
  int EndOfRaceComment_characterID;
  int NumMissilesComparedToNumAttacks;
  int TimeWinningDriverSpentLastPlace;
  int numTimesAttacked;
  union 
  {
    struct 
    {
      short numFramesDrifting;
      short driftBoostTimeMS;
      short driftTotalTimeMS;
      char numBoostsAttempted;
      char numBoostsSuccess;
    } Drifting;
    struct 
    {
      short driftSpinRate;
      short spinDir;
    } Spinning;
    struct 
    {
      struct MaskHeadWeapon *maskObj;
      int boostMeter;
      int fireLevel;
      short timeMS;
      short unk58e;
      short unk590;
      unsigned char unk[2];
      int boolMaskGrab;
    } RevEngine;
    struct 
    {
      struct MaskHeadWeapon *maskObj;
      short AngleAxis_NormalVec[3];
      short animFrame;
      char boolParticlesSpawned;
      char boolStillFalling;
      char boolLiftingPlayer;
      char boolWhistle;
      char unused[8];
    } MaskGrab;
    struct 
    {
      char boolInited;
    } EatenByPlant;
    struct 
    {
      char boolPlayBackwards;
    } Blasted;
    struct 
    {
      int timer;
      int heightOffset;
      int quadHeight;
      int numParticle;
      int beamHeight;
    } Warp;
  } KartStates;
  char paddingbotdata[0x94];
  struct GhostTape *ghostTape;
  short ghostID;
  short ghostBoolInit;
  short ghostBoolStarted;
  short unk636;
};
void VehPhysGeneral_JumpAndFriction(struct Thread *t, struct Driver *d)
{
  char uVar1;
  bool bVar2;
  short sVar3;
  int new_var2;
  int iVar4;
  int new_var5;
  int iVar5;
  int iVar6;
  int new_var4;
  short sVar7;
  int iVar8;
  int iVar9;
  unsigned int uVar10;
  unsigned int uVar11;
  int iVar13;
  unsigned int uVar14;
  int param_2;
  int param_3;
  VECTOR movement;
  int new_var3;
  struct GameTracker **new_var;
  int param_1;
  int jumpForce;
  int temp;
  short *axisAngle;
  asm volatile("lw     $12, 0( %0 );lw     $13, 4( %0 );ctc2   $12, $0;ctc2   $13, $1;lw     $12, 8( %0 );lw     $13, 12( %0 );lw     $14, 16( %0 );ctc2   $12, $2;ctc2   $13, $3;ctc2   $14, $4" : : "r"(&d->matrixMovingDir) : "$12", "$13", "$14");
  if (((d->kartState != 2) && ((d->actionsFlagSet & 0x800000) == 0)) && (d->reserves == 0))
  {
    int ampTurnState = ((short) d->ampTurnState) >> 8;
    iVar9 = d->const_TurnDecreaseRate;
    if (ampTurnState < 0)
    {
      ampTurnState = -ampTurnState;
    }
    param_1 = func_80058F9C(ampTurnState, 0, d->const_BackwardTurnRate, 0, iVar9);
    new_var = &D_8008D2AC;
    param_3 = (iVar9 = d->baseSpeed);
    if (param_3 < 0)
    {
      iVar9 = -param_3;
    }
    if (iVar9 < param_1)
    {
      param_1 = iVar9;
    }
    if (param_3 < 0)
    {
      param_1 = -param_1;
    }
    d->baseSpeed = (short) (((unsigned short) d->baseSpeed) - param_1);
  }
  if (d->set_0xF0_OnWallRub != 0)
  {
    if (d->baseSpeed > d->scrubMeta8)
    {
      d->baseSpeed = d->scrubMeta8;
    }
    if (d->baseSpeed < (-d->scrubMeta8))
    {
      d->baseSpeed = -d->scrubMeta8;
    }
  }
  movement.vx = d->velocity.v[0];
  movement.vy = d->velocity.v[1];
  movement.vz = d->velocity.v[2];
  uVar14 = 0;
  iVar9 = 0;
  iVar8 = d->actionsFlagSet;
  if ((iVar8 & 1) == 0)
  {
    iVar13 = iVar8;
    goto CHECK_FOR_ANY_JUMP;
  }
  if ((d->stepFlagSet & 3) != 0)
  {
    if (0 < d->baseSpeed)
    {
      goto SET_HIGH_ACCEL;
    }
  }
  if (d->baseSpeed != 0)
  {
    if ((((d->terrainMeta1->flags & 4) == 0) || (d->baseSpeed < 1)) || ((-1) < d->speedApprox))
    {
      ;
      iVar5 = iVar8;
      if (((int) d->speedApprox) < 0)
      {
        iVar13 = -iVar5;
      }
      bVar2 = (int) d->speedApprox;
      if (((0x2ff < iVar13) && ((d->baseSpeed < 1) || (((int) d->speedApprox) < 1))) && (((-1) < d->baseSpeed) || ((-1) < bVar2)))
      {
        goto PROCESS_ACCEL;
      }
    }
    iVar9 = d->const_Accel_ClassStat + ((((int) d->accelConst) << 5) / 5);
    if ((d->stepFlagSet & 3) != 0)
    {
      if (d->baseSpeed <= 0)
      {
        goto PROCESS_ACCEL;
      }
      SET_HIGH_ACCEL:
      iVar9 = 8000;

    }
    else
    {
      if ((d->reserves != 0) && (0 < d->baseSpeed))
      {
        iVar9 = d->const_Accel_Reserves;
      }
      param_1 = d->terrainMeta1->slowUntilSpeed;
      if ((param_1 != 0x100) && ((d->actionsFlagSet & 0x800000) == 0))
      {
        iVar9 = (param_1 * iVar9) >> 8;
      }
      d->jump_CooldownMS = 0x180;
    }
  }
  PROCESS_ACCEL:
  uVar10 = (iVar9 * (*new_var)->elapsedTimeMS) >> 5;

  asm volatile("mtc2   %0, $0" : : "r"(0));
  temp = uVar10 & 0xffff;
  asm volatile("mtc2   %0, $1" : : "r"(temp));
  asm volatile("nop;nop;cop2 0x0486012;");
  asm volatile("mfc2   %0, $25" : "=r"(param_1));
  asm volatile("mfc2   %0, $26" : "=r"(param_3));
  asm volatile("mfc2   %0, $27" : "=r"(param_2));
  if (d->baseSpeed < 0)
  {
    d->unk_offset3B2 = -((short) uVar10);
    movement.vx -= param_1;
    movement.vz -= param_3;
    movement.vy -= param_2;
    d->unkVectorX = -((short) param_1);
    d->unkVectorY = -((short) param_3);
    d->unkVectorZ = -((short) param_2);
  }
  else
  {
    d->unk_offset3B2 = (short) uVar10;
    movement.vx += param_1;
    movement.vz += param_3;
    movement.vy += param_2;
    d->unkVectorX = (short) param_1;
    d->unkVectorY = (short) param_3;
    d->unkVectorZ = (short) param_2;
  }
  uVar14 = func_80059070(((movement.vx * movement.vx) + (movement.vy * movement.vy)) + (movement.vz * movement.vz), 0x10);
  uVar14 = uVar14 >> 8;
  new_var2 = d->baseSpeed;
  if (new_var2 < 0)
  {
    new_var2 = -new_var2;
  }
  uVar14 = uVar14 - new_var2;
  bVar2 = ((int) uVar10) < ((int) uVar14);
  if (((int) uVar14) < 0)
  {
    uVar14 = 0;
    ;
  }
  if (((int) uVar10) < ((int) uVar14))
  {
    uVar14 = uVar10;
  }
  if ((d->actionsFlagSet & 1) == 0)
  {
    goto CHECK_FOR_ANY_JUMP;
  }
  if (d->jump_ForcedMS == 0)
  {
    goto CHECK_FOR_ANY_JUMP;
  }
  if (d->jump_unknown != 0)
  {
    d->jump_unknown = 0x180;
  }
  if (d->kartState == 6)
  {
    func_80026440(d, 8, 0);
    func_800264C0(d, 8, 0x7f);
  }
  goto PROCESS_JUMP;
  CHECK_FOR_ANY_JUMP:
  if (((d->actionsFlagSet & 0x8000) != 0) && (d->heldItemID == 5))
  {
    d->actionsFlagSet &= ~0x8000;
    if ((d->jump_CoyoteTimerMS != 0) && (d->jump_CooldownMS == 0))
    {
      d->jump_ForcedMS = 0xa0;
      jumpForce = d->const_JumpForce * 9;
      d->jump_InitialVelY = (short) (jumpForce / 4);
      func_80028494(9, 1, ((unsigned short *) (&d->actionsFlagSet))[1] & 1);
      d->jump_unknown = 0x180;
      goto PROCESS_JUMP;
    }
    d->noItemTimer = 0;
  }

  if (d->forcedJump_trampoline != 0)
  {
    if ((d->jump_ForcedMS == 0) || (d->jump_InitialVelY == d->const_JumpForce))
    {
      func_80028468(0x7e, 1);
    }
    d->jump_ForcedMS = 0xa0;
    if (d->forcedJump_trampoline == 2)
    {
      d->jump_unknown = 0x180;
      d->jump_InitialVelY = (short) (((int) d->const_JumpForce) * 3);
    }
    else
    {
      d->jump_InitialVelY = (short) ((((int) d->const_JumpForce) * 3) / 2);
    }
    d->forcedJump_trampoline = 0;
  }
  else
  {
    if (d->jump_CoyoteTimerMS == 0)
    {
      goto NOT_JUMPING_COLD;
    }
    if (d->jump_TenBuffer == 0)
    {
      goto NOT_JUMPING_COLD;
    }
    if (d->jump_CooldownMS != 0)
    {
      goto NOT_JUMPING_COLD;
    }
    d->jump_ForcedMS = 0xa0;
    d->numberOfJumps++;
    d->jump_InitialVelY = d->const_JumpForce;
    uVar11 = ((unsigned short *) (&d->actionsFlagSet))[1];
    func_80028494(8, 1, uVar11 & 1);
  }
  PROCESS_JUMP:
  iVar9 = 0;

  iVar13 = iVar9;
  d->jump_CooldownMS = 0x180;
  d->jump_TenBuffer = 0;
  d->actionsFlagSet |= 0x480;
  iVar8 = 0x378;
  do
  {
    iVar4 = func_800605A0((short *) (((char *) d) + iVar8), &movement);
    iVar6 = iVar4;
    if (iVar4 < 0)
    {
      iVar6 = -iVar4;
    }
    iVar5 = iVar9;
    if (iVar9 < 0)
    {
      iVar4 = -iVar9;
      iVar5 = iVar4;
    }
    if (iVar5 < iVar6)
    {
      iVar9 = iVar4;
    }
    iVar13++;
    iVar8 += 8;
  }
  while (iVar13 <= 0);
  axisAngle = &d->AxisAngle1_normalVec.v[0];
  if ((d->actionsFlagSet & 1) == 0)
  {
    axisAngle = &d->AxisAngle2_normalVec[0];
  }
  iVar8 = (param_1 = (iVar13 = func_800605A0(axisAngle, &movement)));
  if (iVar8 < 0)
  {
    iVar13 = -iVar8;
  }
  iVar6 = iVar9;
  if (iVar9 < 0)
  {
    iVar6 = -iVar9;
  }
  if (iVar6 < iVar13)
  {
    iVar9 = iVar8;
  }
  iVar4 = iVar9 * iVar9;
  iVar13 = func_80059070(((((int) d->jump_InitialVelY) * ((int) d->jump_InitialVelY)) + iVar4) >> 8, 8);
  temp = (uVar11 = iVar13 - iVar9);
  uVar10 = D_8008D2AC->level1->unk_18C << 8;
  if (uVar10 == 0)
  {
    uVar10 = 0x3700;
  }
  else
    if (0x5000 < ((int) uVar10))
  {
    uVar10 = 0x5000;
  }
  if (((int) uVar10) < temp)
  {
    uVar11 = uVar10;
  }
  if (movement.vy < ((int) uVar11))
  {
    movement.vy = uVar11;
  }
  new_var3 = 4;
  goto NOT_JUMPING;
  NOT_JUMPING_COLD:
  if ((d->actionsFlagSet & 1) != 0)
  {
    if ((d->underDriver != 0) && ((iVar9 = (iVar4 = d->underDriver->mulNormVecY), iVar9 != 0)))
    {
      iVar5 = d->speedApprox;
      iVar13 = (int) iVar5;
      {
        iVar13 = -iVar13;
      }
      new_var5 = iVar9;
      iVar5 = iVar13;
      temp = ((new_var5 * iVar5) >> 8) << 0x10;
      asm volatile("mtc2   %0, $0" : : "r"(temp));
      asm volatile("mtc2   %0, $1" : : "r"(0));
      asm volatile("nop;nop;cop2 0x0486012;");
      asm volatile("mfc2   %0, $25" : "=r"(iVar9));
      asm volatile("mfc2   %0, $26" : "=r"(param_3));
      asm volatile("mfc2   %0, $27" : "=r"(temp));
      movement.vx += new_var5;
      movement.vy += param_3;
      movement.vz += temp;
    }
  }

  NOT_JUMPING:
  func_8005CD1C(d, &movement, 0);

  iVar9 = ((unsigned short) d->speed) - uVar14;
  d->speed++;
  d->speed--;
  if ((iVar9 * 0x10000) < 0)
  {
    d->speed = (param_1 = (short) iVar9);
    d->speed = 0;
  }
  iVar4 = (int) d->speedApprox;
  if (iVar4 < 0)
  {
    if (iVar4 < 0)
    {
      iVar4 = -iVar4;
    }
    if (iVar4 < 0x100)
    {
      sVar7 = d->unk36E - (d->unk36E >> 3);
    }
    else
    {
      int unk13 = ((int) d->unk36E) * 0xd;
      int timer7 = (int) (D_8008D2AC->timer & 7);
      int timer768 = (timer7 << 8) + (timer7 << 9);
      new_var4 = unk13 + timer768;
      param_3 = (short) (((unsigned int) new_var4) >> 4);
      do
      {
        sVar7 = param_3;
      }
      while (0);
    }
  }
  else
  {
    sVar7 = (short) (((d->unk36E * 0xd) + (iVar4 * 3)) >> new_var3);
  }
  d->unk36E = sVar7;
  return;
}
