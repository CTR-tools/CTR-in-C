#ifndef CTR_MATCHING_H
#define CTR_MATCHING_H

// Shared types for the CTR-namespace matching functions (byte-exact vs SCUS_944.26.exe).
typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;
typedef unsigned long  u_long;
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed char    s8;
typedef signed short   s16;
typedef signed int     s32;

// PS1 GPU primitive-scratch arena (PrimMem): a bump allocator.
//   curr      @ 0x0C = next free primitive slot
//   endMin100 @ 0x10 = last slot that can still be handed out (end - 0x100)
struct PrimMem
{
    char  pad0[0xC];
    void *curr;
    void *endMin100;
};

// Animated-texture descriptor (variable length). numFrames texture pointers follow inline at 0xC.
//   ptrActiveTex@0 = current frame's texture (written each tick; also the list terminator == head)
//   numFrames@4, frameOffset@6, frameSkip@8, frameCurr@0xA, ptrArray@0xC..
struct AnimTex
{
    void *ptrActiveTex;
    s16   numFrames;
    s16   frameOffset;
    s16   frameSkip;
    s16   frameCurr;
    void *ptrArray[1];
};

#endif
