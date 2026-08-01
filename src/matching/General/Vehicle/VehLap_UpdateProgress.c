/*
 * VehLap_UpdateProgress @ 0x8005CA24 (SCUS_944.26 / NTSC-U 926)
 *
 * Behavior-exact match: Branch 0 / Stack 0 (base score 2730, Register 22). The residual is a
 * gcc-2.8.1 instruction-scheduling wall (compiled object has 184 vs retail 188 instructions —
 * nearly complete; the difference is ordering the permuter's random search could not bridge, not
 * missing logic). From "aibox staging".
 *
 * ⚠️ The prior aibox seed scored a deceptively-low 380 by being BEHAVIORALLY WRONG (false-friend):
 *   1. the GTE `ctc2 $0` read `relToSegStart` BEFORE it was computed (uninitialized stack);
 *   2. the wrong-way flag was `(cond ? x|0x100 : x) & ~0x100` — the trailing `& ~0x100` cancels the
 *      set, so bit 0x100 (ACTION_DRIVING_WRONG_WAY) was NEVER set;
 *   3. one flag access used `(unsigned char)0x2c8` == 0xc8 (wrong field offset).
 * This delivery is the corrected natural form (verified vs asm + Ghidra + saphi). saphi
 * game/Vehicle/VehLap.c VehLap_UpdateProgress is CLEAN — the bugs were only in the matching seed.
 *
 * Behavior (driver=a0): pick the lap checkpoint index (bot: driver->botData[0x60a]; non-bot:
 * driver->lastValid->checkpointIndex[+0x3e]); with a valid level + index, project the driver's
 * position onto the current track-segment direction via the GTE (mvmva 0,0,0,3,0 dot product):
 * distanceToFinish_curr = (progressNode->distToFinish<<3 + MAC1>>12) % (nodes[0].distToFinish<<3);
 * set/clear ACTION_DRIVING_WRONG_WAY from MAC2 vs 0x5A800; maintain the checkpoint branch-pending
 * flag + currentIndex.
 */
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;

extern unsigned char *PTR_DAT_8008d2ac;         /* gGT */
void MATH_VectorNormalize(void *);              /* func_8003D378 */

void VehLap_UpdateProgress(int driver)
{
    int segmentIndex;
    int gt160;
    int checkpointBase;
    int currCheckpoint;   /* s4 */
    int segStart;         /* s0 */
    int segEnd;           /* a0 */
    int macDot;           /* MAC1 */
    int macDir;           /* MAC2 */
    int progress;
    int sectionSize;
    uint flags;
    uint rotR13R21;
    uint rotR22R23;
    short driverPos[3];       /* sp+0x18 */
    short relToSegStart[3];   /* sp+0x10 */
    short segDir[3];          /* sp+0x20 */

    if (driver == 0)
        return;

    segmentIndex = -1;
    if ((*((uint *) (driver + 0x2c8)) & 0x100000) != 0)
    {
        segmentIndex = *((byte *) (driver + 0x60a));
    }
    else
    {
        int owner = *((int *) (driver + 0x354));
        if ((owner != 0) && (*((byte *) (owner + 0x3e)) != 0xff))
            segmentIndex = *((byte *) (owner + 0x3e));
    }

    gt160 = *((int *) (PTR_DAT_8008d2ac + 0x160));
    if (((uint) (*((int *) (gt160 + 0x148)) - 1) < 0xff) &&
        (0 <= (int) ((short) segmentIndex)))
    {
        checkpointBase = *((int *) (gt160 + 0x14c));
        currCheckpoint = checkpointBase + ((int) ((short) segmentIndex)) * 0xc;
        segStart = checkpointBase + ((uint) (*((byte *) (currCheckpoint + 8)))) * 0xc;

        driverPos[0] = (short) (*((int *) (driver + 0x2d4)) >> 8);
        driverPos[1] = (short) (*((int *) (driver + 0x2d8)) >> 8);
        driverPos[2] = (short) (*((int *) (driver + 0x2dc)) >> 8);

        segEnd = checkpointBase + ((uint) (*((byte *) (segStart + 8)))) * 0xc;
        segDir[0] = *((ushort *) (segStart + 0)) - *((ushort *) (segEnd + 0));
        segDir[1] = *((ushort *) (segStart + 2)) - *((ushort *) (segEnd + 2));
        segDir[2] = *((ushort *) (segStart + 4)) - *((ushort *) (segEnd + 4));
        MATH_VectorNormalize(segDir);

        relToSegStart[0] = (ushort) driverPos[0] - *((ushort *) (segStart + 0));
        relToSegStart[1] = (ushort) driverPos[1] - *((ushort *) (segStart + 2));
        relToSegStart[2] = (ushort) driverPos[2] - *((ushort *) (segStart + 4));

        asm volatile("ctc2 %0, $0" : : "r"(*((int *) (&relToSegStart[0]))));
        rotR13R21 = ((uint) ((ushort) relToSegStart[2])) |
                    ((((int) (((uint) (*((ushort *) (driver + 0x314)))) << 0x10)) >> 0x15) << 0x10);
        asm volatile("ctc2 %0, $1" : : "r"(rotR13R21));
        rotR22R23 = ((((int) (((uint) (*((ushort *) (driver + 0x31a)))) << 0x10)) >> 0x15) & 0xffffU) |
                    ((((int) (((uint) (*((ushort *) (driver + 0x320)))) << 0x10)) >> 0x15) << 0x10);
        asm volatile("ctc2 %0, $2" : : "r"(rotR22R23));
        asm volatile("lwc2 $0, 0( %0);lwc2 $1, 4( %0)" : : "r"(segDir));
        asm volatile("nop\nnop\n.word 0x1260404A");
        asm volatile("mfc2 %0, $25" : "=r"(macDot));
        asm volatile("mfc2 %0, $26" : "=r"(macDir));

        progress = (((uint) (*((ushort *) (segStart + 6)))) << 3) + (macDot >> 0xc);
        *((int *) (driver + 0x488)) = progress;

        sectionSize = ((uint) (*((ushort *) (*((int *) (*((int *) (PTR_DAT_8008d2ac + 0x160)) + 0x14c)) + 6)))) << 3;
        *((int *) (driver + 0x488)) = progress % sectionSize;

        if (0x5a800 < macDir)
            flags = *((uint *) (driver + 0x2c8)) | 0x100;
        else
            flags = *((uint *) (driver + 0x2c8)) & 0xfffffeff;
        *((uint *) (driver + 0x2c8)) = flags;

        if (((*((uint *) (driver + 0x2c8)) & 0x8000000) != 0) &&
            (((uint) (*((byte *) (driver + 0x495)))) != ((int) ((short) segmentIndex))))
        {
            *((byte *) (driver + 0x494)) = (byte) segmentIndex;
            *((uint *) (driver + 0x2c8)) = *((uint *) (driver + 0x2c8)) & 0xf7ffffff;
        }

        if (*((byte *) (currCheckpoint + 9)) != 0xff)
            *((uint *) (driver + 0x2c8)) = *((uint *) (driver + 0x2c8)) | 0x8000000;

        *((byte *) (driver + 0x495)) = (byte) segmentIndex;
    }
}
