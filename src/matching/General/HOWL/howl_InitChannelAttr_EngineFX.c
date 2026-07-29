#include "HOWL.h"

// howl_InitChannelAttr_EngineFX @ 0x8002C34C (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best score = 50, byte-BEHAVIOR-correct. The ENTIRE residual is a
//   pure gcc-2.8.1 saved-register swap (10 reg-name diffs, no instruction/scheduling differences):
//   retail assigns engineFX->$s1 and distort->$s0; gcc here assigns engineFX->$s0 and distort->$s1.
//   AUTO-GRANTED (2026-07-27) per the <300 regalloc-plateau policy. There is a genuine tension: WITHOUT
//   the `int spuAddr` local below, gcc matches retail's (unusual) engineFX=$s1 but schedules the ad/sr
//   stores BEFORE the spuAddr read (4 reorderings, score 360); WITH the local (needed so the spuAddr
//   VALUE is read before ad/sr and only the <<3 happens after, matching retail), gcc flips to engineFX=$s0
//   (score 50). No C construct gives both the correct ordering AND retail's register choice. Structural
//   levers exhausted (360->150->50 via volMul/spuAddr-early/int-spuAddr-not-u_short), permuter plateaus
//   at 50 over 175 iterations.
//
// Builds a ChannelAttr for an engine-FX voice: sets volume via Channel_SetVolume(vol_FX * engineFX->volume
// * vol >> 10, LR); pitch = engineFX->pitch, scaled by the engine distort table unless distort==0x80; fixed
// ADSR (ad=0x80ff, sr=0x1fc2); spuStartAddr = SampleAddrTable[engineFX->spuIndex] << 3.
//
// GP-rel globals DEFINED (GP_VALUE=0x8008CF6C): vol_FX=D_8008D7AC(0x840,u8), sampleAddrTable=D_8008D7DC
// (0x870,int). Absolute extern: distortConst_Engine=D_800825AC (int[]). Callee func_8002B540 Channel_SetVolume(#72).

extern void func_8002B540(struct ChannelAttr* attr, uint vol, uint pan);  // Channel_SetVolume

extern int D_800825AC[];   // g_anDistortConstEngine

u_char D_8008D7AC = 0;   // g_bVolFX               (gp+0x840)
int    D_8008D7DC = 0;   // g_pHowlSampleAddrTable (gp+0x870)

void howl_InitChannelAttr_EngineFX(struct EngineFX* engineFX, struct ChannelAttr* attr,
                                   int vol, int LR, int distort)
{
    short pitch;

    func_8002B540(attr, (uint)(D_8008D7AC * engineFX->volMul * vol) >> 10, LR);

    if (distort != 0x80)
        pitch = (short)((uint)engineFX->pitch * D_800825AC[distort] >> 0x10);
    else
        pitch = engineFX->pitch;
    attr->pitch = pitch;

    {
        int spuAddr = *(u_short*)(engineFX->spuIndex * 4 + D_8008D7DC);
        attr->ad = 0x80ff;
        attr->sr = 0x1fc2;
        attr->spuStartAddr = (void*)((uint)spuAddr << 3);
    }
}
