#include "HOWL.h"

// Garage_Enter @ 0x80030264 (SCUS_944.26.exe, 926/NTSC-U).
// BEST MATCH -- score 10, NOT score-0 (auto-granted head-load-lui, 2026-07-29). Behavior byte-EXACT.
// Set up the garage SFX slots when entering menu slot `idx`: current slot pans centre (LR 0x80), the
// previous slot (wrap 0->7) pans left (0x3C), the next slot (wrap 7->0) pans right (0xC3), all others
// GSP_GONE(3); each slot with a configured soundID (D_8008D080[i]) (re)starts its FX via OtherFX_RecycleNew
// (func_8002E690). Verified vs asm. KEY (1290->10): index D_800962E4[i] DIRECTLY (no cached g/pAudio)
// so gcc strength-reduces the pool + &.audioPtr induction vars like retail. Flags = (volume<<16)|0x80{LR}.
//
// MATCH NOTE -- residual 10 (2 reg): the D_8008D080 table base loads as `lui v0; addiu s1,v0` (mine) vs
// `lui s1; addiu s1,s1` (retail) -- a head-load-lui materialization tie-break (#34/#39-class), unforceable
// from C (cached-table and the permuter both plateau at 10). Both forms compute &D_8008D080 identically.

struct GarageFX { char gsp_curr; char gsp_prev; short volume; short LR; int audioPtr; };
extern void func_8002E690(int* slot, u_int soundID, u_int flags);   // OtherFX_RecycleNew
extern struct GarageFX D_800962E4[];   // garageSoundPool[8]
extern u_char          D_8008D080[];   // per-slot soundID table

void Garage_Enter(int idx)
{
    int i, prev, next;

    if (idx >= 8) return;
    prev = idx - 1;
    if (prev < 0) prev = 7;
    next = idx + 1;
    if (next >= 8) next = 0;

    for (i = 0; i < 8; i++)
    {
        if (i == idx)
        {
            D_800962E4[i].gsp_curr = 0; D_800962E4[i].gsp_prev = 3; D_800962E4[i].volume = 0; D_800962E4[i].LR = 0x80;
            if (D_8008D080[i] != 0) { func_8002E690(&D_800962E4[i].audioPtr, D_8008D080[i], (D_800962E4[i].volume << 16) | 0x8080); continue; }
        }
        else if (i == prev)
        {
            D_800962E4[i].gsp_curr = 1; D_800962E4[i].gsp_prev = 3; D_800962E4[i].volume = 0; D_800962E4[i].LR = 0x3C;
            if (D_8008D080[i] != 0) { func_8002E690(&D_800962E4[i].audioPtr, D_8008D080[i], (D_800962E4[i].volume << 16) | 0x803C); continue; }
        }
        else if (i == next)
        {
            D_800962E4[i].gsp_curr = 2; D_800962E4[i].gsp_prev = 3; D_800962E4[i].volume = 0; D_800962E4[i].LR = 0xC3;
            if (D_8008D080[i] != 0) { func_8002E690(&D_800962E4[i].audioPtr, D_8008D080[i], (D_800962E4[i].volume << 16) | 0x80C3); continue; }
        }
        else
        {
            D_800962E4[i].gsp_curr = 3; D_800962E4[i].gsp_prev = 3; D_800962E4[i].volume = 0; D_800962E4[i].LR = 0x80;
        }
        D_800962E4[i].audioPtr = 0;
    }
}
