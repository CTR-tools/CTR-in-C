// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef unsigned char u8;
typedef struct Obj Obj;
typedef struct ObjRef ObjRef;
struct ObjRef
{
  char pad0[0x38];
  u8 unk38;
};
struct Obj
{
  char pad00[0x88];
  s32 unk88;
  s32 unk8C;
  s32 unk90;
  s32 unk94;
  s32 unk98;
  s32 unk9C;
  s32 unkA0;
  s32 unkA4;
  s16 unkA8;
  s16 unkAA;
  char padAC[0x2D8 - 0xAC];
  s32 unk2D8;
  char pad2DC[0x330 - 0x2DC];
  u32 unk330[5];
  char pad344[0x350 - 0x344];
  ObjRef *unk350;
  char pad354[0x360 - 0x354];
  s32 unk360;
  s16 unk364;
  char pad366[0x38C - 0x366];
  s16 unk38C;
  char pad38E[0x394 - 0x38E];
  s16 unk394;
  s16 unk396;
  char pad398[0x3CC - 0x398];
  s16 unk3CC;
  s16 unk3CE;
  s16 unk3D0;
};
void func_8005E104(Obj *, void *, s32);
void func_8005E214(Obj *, void *);
void func_8005EA60(void *arg0, Obj *arg1)
{
  s32 temp_a0;
  s32 temp_v1;
  ObjRef *temp_v0;
  register s32 temp_a2 asm("a2");
  if (arg1->unk38C >= 0x6401)
  {
    arg1->unk38C = 0x6400;
  }
  asm volatile("lw     $12, 0(%0);lw     $13, 4(%0);ctc2   $12, $16;ctc2   $13, $17;lw     $12, 8(%0);lw     $13, 12(%0);lw     $14, 16(%0);ctc2   $12, $18;ctc2   $13, $19;ctc2   $14, $20" : : "r"(arg1->unk330) : "$12", "$13", "$14");
  temp_a2 = 0x190000;
  asm volatile("mtc2 %0, $0" : : "r"(temp_a2));
  temp_a2 = 0;
  asm volatile("mtc2 %0, $1" : : "r"(temp_a2));
  asm volatile("nop\n\tnop\n\t.word 0x4A4C6012");
  asm volatile("swc2 $25, 0(%0)" : : "r"(&arg1->unk94) : "memory");
  asm volatile("swc2 $26, 0(%0)" : : "r"(&arg1->unk98) : "memory");
  asm volatile("swc2 $27, 0(%0)" : : "r"(&arg1->unk9C) : "memory");
  func_8005E104(arg1, &arg1->unk88, temp_a2);
  temp_v0 = arg1->unk350;
  if (temp_v0 != 0)
  {
    if (temp_v0->unk38 == 0xE)
    {
      temp_a0 = arg1->unk2D8;
      if (temp_a0 >= (-0xFFF))
      {
        temp_v1 = (-0x1000) - temp_a0;
        if (arg1->unk8C < temp_v1)
        {
          arg1->unk8C = temp_v1;
        }
      }
    }
  }
  func_8005E214(arg1, &arg1->unk88);
  arg1->unkA4 = 0x10000000;
  arg1->unkA0 = 0;
  arg1->unkA8 = 0;
  arg1->unk364 = 0;
  arg1->unk360 = 0x10000000;
  arg1->unkAA = 0;
  arg1->unk88 += arg1->unk3CC;
  arg1->unk8C += arg1->unk3CE;
  arg1->unk90 += arg1->unk3D0;
}
