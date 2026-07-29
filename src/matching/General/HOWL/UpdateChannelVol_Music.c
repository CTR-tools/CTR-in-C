#include "HOWL.h"

// UpdateChannelVol_Music @ 0x8002AD70 (SCUS_944.26.exe, 926/NTSC-U).
//
// MATCHING NOTE: NOT a score-0 match. Best legitimate score = 690 (a marginal 680 form differs only by
//   a preamble/HOWL.h-inlining artifact, ~20 pts of noise). 44/44 INSTRUCTION PARITY and the correct
//   6-multiply structure; behavior is byte-for-byte correct. The ENTIRE residual is one caller-saved
//   register-COALESCING tie-break producing a position-cascade diff: retail coalesces the per-branch
//   `(prod >> 10)` value into $a1 (`sra $a1,...; mult $a1,$v0(sampleVol)`) so it flows straight into the
//   Channel_SetVolume vol arg, while gcc allocates it to $v1 and puts sampleVol in $a1
//   (`sra $v1,...; lbu $a1,...; mult $a1,$v1`). Same #60/#43-class coalescing tie-break; unforceable
//   from C (objdump-verified across accumulator / statement-reorder / var-split variants — gcc always
//   picks $v1 for the scaled value) and the permuter finds only broken forms below it (uninitialized-var
//   aliases, volatile-truncation spills). USER time-boxed (6 fires) then auto-granted.
//
//   The hard structural work IS solved and is match-necessary (kept below): retail DUPLICATES the
//   volume subexpression per-branch (6 mults) which gcc keeps CSE-hoisting to 5. The fix is
//   (a) `poolSlot = &g_aSongPool[songSeq[0xb]]` — a shared pointer so the vol_Curr LOAD duplicates, and
//   (b) computing the `>> 10` INSIDE each branch on an INT value (`prodMusic >> 10` = arithmetic `sra`,
//   not `(uint)... >> 10` = logical `srl`) so GCSE can't hoist it. The `acc` accumulator then coalesces
//   the scaled volume toward the vol arg (fixing the attr->$t1 preservation). This drove 3495 -> 690.
//
// Sets one SPU channel's volume for a music sequence: combines g_bVolMusic, the song-pool slot's
// vol_Curr, the sequence volume (songSeq[5]), the per-sample volume (long-sample[instrumentID] or
// short-sample[index] when flags bit2 set), and vol, then Channel_SetVolume with the seq's L/R.
//
// Build note: g_bVolMusic (gp+0x84C) / g_pCseqShortSamples (gp+0x858) / g_pCseqLongSamples (gp+0x87C)
// are GP-relative, DEFINED so gcc emits %gp_rel (GP_VALUE=0x8008CF6C). g_aSongPool is absolute.
// Callee Channel_SetVolume (func_8002B540). Polarity inverted (percussive branch inline).

extern void func_8002B540(struct ChannelAttr* attr, uint vol, uint pan);  // Channel_SetVolume

extern struct Song D_80095D84[];   // g_aSongPool

u_char D_8008D7B8 = 0;   // g_bVolMusic         (gp+0x84C)
int    D_8008D7C4 = 0;   // g_pCseqShortSamples (gp+0x858)
int    D_8008D7E8 = 0;   // g_pCseqLongSamples  (gp+0x87C)

void UpdateChannelVol_Music(u_char* songSeq, struct ChannelAttr* attr, int index, int vol)
{
    struct Song* poolSlot;
    int prodMusic;
    int scaledVol;
    int longOff;
    u_char sampleVol;

    poolSlot = &D_80095D84[songSeq[0xb]];
    if ((*songSeq & 4) != 0)
    {
        // Percussive: base * pool vol_Curr * seq vol, computed inline so this branch keeps its own shift.
        scaledVol = ((uint)D_8008D7B8 * (uint)poolSlot->vol_Curr * (uint)songSeq[5]) >> 10;
        sampleVol = *(u_char*)(D_8008D7C4 + index * 8 + 1);
    }
    else
    {
        // Melodic: same product, but via an INT temp so `>> 10` is `sra` (matches retail); keeping the
        // product AND the shift per-branch stops GCSE from merging the two paths' five multiplies.
        prodMusic = (uint)D_8008D7B8 * (uint)poolSlot->vol_Curr * (uint)songSeq[5];
        longOff = (uint)songSeq[3] * 0xc;
        scaledVol = prodMusic >> 10;
        sampleVol = *(u_char*)(D_8008D7E8 + longOff + 1);
    }
    {
        // Accumulator coalesces scaledVol toward the Channel_SetVolume vol arg (fixes attr->$t1).
        uint acc;
        acc = (uint)scaledVol * (uint)sampleVol;
        acc = acc * vol;
        func_8002B540(attr, acc >> 0xf, (uint)songSeq[9]);
    }
}
