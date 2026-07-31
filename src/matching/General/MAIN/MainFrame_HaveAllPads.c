// MainFrame_HaveAllPads @ 0x80035d70  (vs SCUS_944.26.exe)
// BEHAVIOR-EXACT (Branch/Stack 0; base 975, Reg 3; grind floor ~850 non-natural).
// Residual: whole-loop instruction scheduling on the running-AND pad scan.
// Returns 1 if the first numPads controllers are all present+ready, else 0 (0 when
// numPads==0). Always 1 unless a load just completed (Loading.stage==LOAD_IDLE=-1).
// A pad is ready iff ptrControllerPacket!=0 && isControllerConnected==0 (PLUGGED=0).
// Match levers: gp-relative Loading.stage (gp+0x18C, defined `=0`) + gGamepads
// (gp+0x344); running-AND accumulator held across the loop with a per-iter `thisPad`
// temp; `if (stage != -1) return 1` early-out first (retail fall-through layout);
// signed `short result`/`thisPad` (matches the sll zero-test).
#include "CTR.h"

struct ControllerPacket { unsigned char isControllerConnected; };
struct GamepadBuffer { char pad0[0x20]; struct ControllerPacket *ptrControllerPacket; char pad1[0x50 - 0x24]; };
struct GamepadSystem { struct GamepadBuffer gamepad[8]; };

int D_8008D0F8 = 0;                    /* sdata->Loading.stage (gp+0x18C); LOAD_IDLE = -1 */
struct GamepadSystem *D_8008D2B0 = 0;  /* sdata->gGamepads (gp+0x344) */

int MainFrame_HaveAllPads(short numPads)
{
    short result = 1;
    int n;

    if (D_8008D0F8 != -1)
        return 1;

    n = numPads;
    if (n == 0)
    {
        result = 0;
    }
    else
    {
        short i = 0;
        do
        {
            int thisPad = 0;
            if (result != 0)
            {
                struct ControllerPacket *pkt = D_8008D2B0->gamepad[i].ptrControllerPacket;
                if (pkt != 0)
                    thisPad = pkt->isControllerConnected == 0;
            }
            result = thisPad;
            i++;
        } while (i < n);
    }
    return (short)result;
}
