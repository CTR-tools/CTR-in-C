// LOAD_InitCDvol @ 0x8007c118  (vs SCUS_944.26.exe)  SCORE 0 — exact match.
// CD/SPU volume init (retail CdInit hook): if SPU master vol is zero, set L/R to
// 0x3fff; set SPU CD vol L/R = 0x3fff and SPU_CTRL = 0xc001; then program the CD
// volume mixer via the CD registers (CdlSetmixon-style) with the 0x80/0 mixer
// levels staged in a local param buffer. SPU regs via D_8008C6F4 base + offsets;
// CD_REG(0..3) via pointer globals D_8008C6E0/E4/E8/EC.
#include "CTR.h"

extern char *D_8008C6F4;          /* SPU register block base */
extern unsigned char *D_8008C6E0; /* CD_REG(0) */
extern unsigned char *D_8008C6E4; /* CD_REG(1) */
extern unsigned char *D_8008C6E8; /* CD_REG(2) */
extern unsigned char *D_8008C6EC; /* CD_REG(3) */

int LOAD_InitCDvol(void)
{
    unsigned char param[4];

    if (*(unsigned short *)(D_8008C6F4 + 0x1B8) == 0 && *(unsigned short *)(D_8008C6F4 + 0x1BA) == 0)
    {
        *(unsigned short *)(D_8008C6F4 + 0x180) = 0x3fff;
        *(unsigned short *)(D_8008C6F4 + 0x182) = 0x3fff;
    }

    *(unsigned short *)(D_8008C6F4 + 0x1B0) = 0x3fff;
    *(unsigned short *)(D_8008C6F4 + 0x1B2) = 0x3fff;
    *(unsigned short *)(D_8008C6F4 + 0x1AA) = 0xc001;

    param[0] = 0x80;
    param[1] = 0;
    param[2] = 0x80;
    param[3] = 0;

    *D_8008C6E0 = 2;
    *D_8008C6E8 = param[0];
    *D_8008C6EC = param[1];
    *D_8008C6E0 = 3;
    *D_8008C6E4 = param[2];
    *D_8008C6E8 = param[3];
    *D_8008C6EC = 0x20;

    return 0;
}
