// =============================================================
// Shared types for matching/General/HOWL/*.c
//
// The "Howl" namespace (Naughty Dog's audio engine) per
// src/decompile/symbols/syms926.txt (CountSounds .. Garage_Leave).
// Struct shapes are taken from src/decompile/include/* and verified
// against SCUS_944.26.exe in Ghidra. Each .c is built standalone by the
// permuter, so this header stays self-contained (no nested includes).
// =============================================================

#ifndef MATCHING_HOWL_H
#define MATCHING_HOWL_H

// USE_REAL_GTE must be visible to cpp (the permuter pre-processes base.c
// with bare cpp before compile.sh sets -D), so define it here when unset.
#ifndef USE_REAL_GTE
#define USE_REAL_GTE 1
#endif

// ---- primitive type aliases (per coding-standards.md) ----

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef signed int     s32;
typedef signed short   s16;
typedef signed char    s8;

// Aliases retained for Ghidra-style code in this subdir.
typedef unsigned int   u_int;
typedef unsigned int   uint;
typedef unsigned short u_short;
typedef unsigned short ushort;
typedef unsigned char  u_char;
typedef unsigned char  byte;
typedef int            bool;

// ---- basic math types (GTE) ----

typedef struct VECTOR  { s32 vx, vy, vz, pad; } VECTOR;
typedef struct SVECTOR { s16 vx, vy, vz, pad; } SVECTOR;
typedef struct MATRIX  { s16 m[3][3]; s16 pad; s32 t[3]; } MATRIX;

// ---- GTE (cop2) intrinsics — real PS1 asm, byte-exact vs SCUS_944.26.exe ----
// (USE_REAL_GTE is always 1 for the permuter/compile path; software emulation
//  is unnecessary since these files exist only to match the retail binary.)
#define gte_ldv0(r0)    asm volatile("lwc2 $0, 0( %0 );lwc2 $1, 4( %0 )" : : "r"(r0))
#define gte_rtv0()      asm volatile("nop;nop;cop2 0x0486012;")
#define gte_stlvnl0(r0) asm volatile("swc2 $25, 0( %0 )" : : "r"(r0))
#define gte_stlvnl1(r0) asm volatile("swc2 $26, 0( %0 )" : : "r"(r0))
#define gte_stlvnl2(r0) asm volatile("swc2 $27, 0( %0 )" : : "r"(r0))
#define gte_ldIR1(r0)   asm volatile("lwc2 $9, 0( %0 )"  : : "r"(r0))
#define gte_ldIR2(r0)   asm volatile("lwc2 $10, 0( %0 )" : : "r"(r0))
#define gte_ldIR3(r0)   asm volatile("lwc2 $11, 0( %0 )" : : "r"(r0))
#define gte_sqr0()      asm volatile("nop;nop;cop2 0x0A00428;")
#define gte_stMAC1(r0)  asm volatile("mfc2 %0, $25" : "=r"(r0))
#define gte_stMAC2(r0)  asm volatile("mfc2 %0, $26" : "=r"(r0))
#define gte_stMAC3(r0)  asm volatile("mfc2 %0, $27" : "=r"(r0))

// ---- audio-engine struct shapes (offsets verified vs SCUS_944.26.exe) ----

// Generic intrusive doubly-linked list head (LIST_* helpers). 8 bytes.
struct LinkedList
{
    void*    first;      // 0x0
    void*    last;       // 0x4
};

// Queued voiceline request node (Voiceline1 free list / Voiceline2 active list). 0x10 bytes.
struct Voiceline
{
    struct Voiceline* next;    // 0x0
    struct Voiceline* prev;    // 0x4
    short    id;               // 0x8
    u_char   channel;          // 0xA
    u_char   flag;             // 0xB
    int      time;             // 0xC
};

// A pickable set of voiceline sound-ids (g_pVoicelineSets[channel][index]). 8 bytes.
struct VoicelineSet
{
    u_short* soundIds;   // 0x0
    u_short  count;      // 0x4
    u_short  _pad6;      // 0x6
};

// Per-OtherFX metadata entry (g_pHowlMetaOtherFX[], 8 bytes each).
struct OtherFX
{
    u_char   flags;      // 0x0  (&2 => ChannelStats.flags|=4;  &4 => use vol_Voice scale)
    u_char   volMul;     // 0x1  per-sample volume multiplier
    u_short  pitch;      // 0x2  base pitch
    u_short  spuIndex;   // 0x4  index into the SPU-addr table
    u_short  duration;   // 0x6
};

// SPU sample-address table entry (g_pHowlSampleAddrTable[], 4 bytes each).
struct SpuAddr
{
    u_short  spuAddr;    // 0x0  0 => sample not loaded
    u_short  _pad;       // 0x2
};

// Loaded HOWL header (counts of each sound category). sizeof = 0x28; howl_ParseHeader
// walks these counts to derive the section base pointers cached in the g_pHowl* globals.
struct HowlHeader
{
    int      magic;        // 0x0   "HOWL" (compared as a 4-byte word vs g_szHowlMagic)
    int      version;      // 0x4   == 0x80
    u_char   _pad8[8];     // 0x8
    u_int    numSpuAddrs;  // 0x10  SPU sample-address table entries (4 bytes each)
    u_int    numOtherFX;   // 0x14
    u_int    numEngineFX;  // 0x18
    u_int    numBanks;     // 0x1C  bank offsets (2 bytes each)
    u_int    numSequences; // 0x20  song offsets (2 bytes each)
    u_int    headerSize;   // 0x24  (sizeof header = 0x28)
};

// Per-EngineFX metadata entry (g_pHowlMetaEngineFX[], 8 bytes each). Distinct
// layout from OtherFX: the SPU-addr-table index sits at 0x6.
struct EngineFX
{
    u_char   _pad0;      // 0x0
    u_char   volMul;     // 0x1  per-sample volume multiplier
    u_short  pitch;      // 0x2  base pitch
    u_short  _pad4;      // 0x4
    u_short  spuIndex;   // 0x6  index into the SPU-addr table
};

// Attribute block built on the stack by howl_InitChannelAttr_* / Channel_SetVolume; 0x10 bytes.
struct ChannelAttr
{
    void*    spuStartAddr; // 0x0  SPU sample start address (>>3-encoded)
    u_short  ad;           // 0x4  attack/decay
    short    sr;           // 0x6  sustain/release
    short    pitch;        // 0x8
    short    reverb;       // 0xA
    short    audioL;       // 0xC  left SPU volume  (set by Channel_SetVolume)
    short    audioR;       // 0xE  right SPU volume (set by Channel_SetVolume)
};

// Live per-channel playback state (returned by Channel_AllocSlot_AntiSpam).
struct ChannelStats
{
    struct ChannelStats* next;  // 0x0
    struct ChannelStats* prev;  // 0x4
    u_char   flags;             // 0x8   (& 1 = active bit)
    u_char   channelID;         // 0x9   index into g_abChannelUpdateFlags (4 bytes each)
    u_char   unk1;              // 0xA   from songSeq->unk on note-on
    u_char   type;              // 0xB
    u_char   unk2;              // 0xC
    u_char   drumIndex_pitchIndex; // 0xD
    u_char   echo;              // 0xE
    u_char   vol;               // 0xF
    u_char   distort;           // 0x10
    u_char   LR;                // 0x11
    short    ad;                // 0x12  attack/decay   (init 0x80ff)
    short    sr;                // 0x14  sustain/release (init 0x1fc2)
    short    timeLeft;          // 0x16
    int      soundID;           // 0x18  (CountSounds()<<16) | id
    int      startFrame;        // 0x1C
};

// CSEQ per-sequence playback state passed to the cseq_opcode* handlers.
struct SongSeq
{
    u_char   flags;             // 0x0   (& 2 = ?, & 4 = drum vs instrument, | 8 = 'stopping')
    u_char   soundID;           // 0x1
    u_char   unk;               // 0x2   copied to channel->unk1 on note-on
    u_char   instrumentID;      // 0x3   index into g_pCseqLongSamples (0xc each)
    u_char   reverb;            // 0x4
    u_char   vol_Curr;          // 0x5   current per-sequence volume
    u_char   vol_New;           // 0x6   target per-sequence volume
    u_char   vol_StepRate;      // 0x7   volume ramp step
    u_char   distort;           // 0x8   pitch distort (0x80 = none)
    u_char   LR;                // 0x9   pan
    u_char   unk0A;             // 0xA   note counter (bumped on note-on)
    u_char   songPoolIndex;     // 0xB   index into g_aSongPool (struct Song, 0x7C each)
    int      NoteLength;        // 0xC   current note's decoded VLQ length
    int      NoteTimeElapsed;   // 0x10  ticks elapsed on the current note
    char*    firstNote;         // 0x14  start of this sequence's note stream
    u_char*  currNote;          // 0x18  cursor into the note stream
};

// g_pCseqLongSamples entry (instrument, 0xc bytes each).
struct SampleInstrument
{
    u_char   _pad0;      // 0x0
    u_char   volume;     // 0x1
    u_char   _pad2[2];   // 0x2
    u_short  basePitch;  // 0x4
    u_short  spuIndex;   // 0x6  index into g_pHowlSampleAddrTable (*2 folded stride)
    short    ad;         // 0x8
    short    sr;         // 0xA
};

// g_pCseqShortSamples entry (drum, 8 bytes each).
struct SampleDrums
{
    u_char   _pad0;      // 0x0
    u_char   volume;     // 0x1
    u_short  pitch;      // 0x2
    u_short  spuIndex;   // 0x4  index into g_pHowlSampleAddrTable
    u_short  _pad6;      // 0x6
};

// SongSet: an Adventure-hub active-sequence descriptor passed to SongPool_Start / SongPool_AdvHub2.
// (In earlier consumers it was accessed as a bare uint*: [0]=numSeqs, [1]=ptrSongSetBits.)
struct SongSet
{
    int      numSeqs;         // 0x0  sequence count (must match the song's CseqSongHeader)
    u_char*  ptrSongSetBits;  // 0x4  per-sequence active-bit array
};

// Loaded CSEQ (music sequence) header; only numSongs (@0x6) is needed so far.
// Loaded CSEQ music-bank header. howl_ParseCseqHeader walks these counts to derive the
// section base pointers cached in the g_pCseq* globals. Data begins at +0x8.
struct CseqHeader
{
    char     _pad0[4];         // 0x0
    u_char   numLongSamples;   // 0x4  long-sample instruments (0xc bytes each)
    u_char   numShortSamples;  // 0x5  short-sample / drum entries (8 bytes each)
    u_short  numSongs;         // 0x6  song offset table (2 bytes each)
};

// Song-pool slot (g_aSongPool[], 0x7C bytes each). Only flags (@0x0, &1 = in use)
// is read directly here; the whole slot is passed to SongPool_Start.
struct Song
{
    u_char   flags;                    // 0x0  (& 1 => slot in use, & 2 => paused)
    char     songPoolIndex;            // 0x1  this slot's own index in g_aSongPool
    u_short  id;                       // 0x2  song id currently loaded in this slot
    int      songSetActiveBits;        // 0x4  active-sequence mask when started with a songSet
    short    tpqn;                     // 0x8  ticks per quarter note
    short    bpm;                      // 0xA  current beats per minute
    int      tempo;                    // 0xC  per-tick timing value (SongPool_CalculateTempo)
    int      unk10;                    // 0x10
    int      timeSpentPlaying;         // 0x14
    u_char   vol_Curr;                 // 0x18 current per-song volume
    u_char   vol_New;                  // 0x19 target per-song volume
    u_char   vol_StepRate;             // 0x1A volume ramp step
    u_char   numSequences;             // 0x1B live sequences in CseqSequences[]
    struct SongSeq* CseqSequences[24]; // 0x1C (24 * 4 = 0x60, sizeof = 0x7C)
};

// SPU reverb preset entry (g_aHowlReverbModeParams[], 0x14 bytes each = SpuReverbAttr).
// SpuSetReverbModeParam consumes the head; only the L/R depth (@0x8/0xA) is read here.
struct HowlReverbParam
{
    char   _pad0[8];    // 0x0
    short  depthLeft;   // 0x8
    short  depthRight;  // 0xA
    char   _padC[8];    // 0xC
};

// Partial driver/racer record — only the field HOWL needs so far.
struct Driver
{
    u_char   _pad0[0x4A];
    u_char   driverID;                     // 0x4A  index into g_awCharacterIDs
};

// Resident game state (only a couple of fields needed by HOWL so far).
struct GameTracker
{
    u_char   _pad0[0x14DC];
    struct Driver* cameraDC0_driverToFollow; // 0x14DC  cameraDC[0].driverToFollow
    u_char   _pad14e0[0x530];              // 0x14E0
    int      levelID;                      // 0x1A10  current level id (NAUGHTY_DOG_CRATE = 0x29)
    u_char   _pad1a14[0x294];              // 0x1A14
    u_char   numPlyrCurrGame;              // 0x1CA8  players in current game (1-4)
    u_char   _pad1[0x4F];                  // 0x1CA9
    int      frameTimer_MainFrame_ResetDB; // 0x1CF8
};

#endif // MATCHING_HOWL_H
