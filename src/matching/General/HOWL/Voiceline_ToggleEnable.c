#include "HOWL.h"

// Voiceline_ToggleEnable @ 0x8002CBB4 (SCUS_944.26.exe, 926/NTSC-U). SCORE-0 MATCH (perfect).
//
// Enables/disables voicelines. When disabling (toggle==0): zero the voiceline cooldown and drain the active
// list (Voiceline_StopAll, #93). Always store the new enable flag.
//
// GP-rel DEFINED (GP_VALUE=0x8008CF6C): boolCanPlayVoicelines=D_8008D7EC(0x880,u8),
// voicelineCooldown=D_8008D7EE(0x882,short). Callee: func_8002CB44 Voiceline_StopAll(#93).

extern void func_8002CB44(void);   // Voiceline_StopAll (#93)

u_char D_8008D7EC = 0;   // boolCanPlayVoicelines (gp+0x880)
short  D_8008D7EE = 0;   // voicelineCooldown     (gp+0x882)

void Voiceline_ToggleEnable(int toggle)
{
    if (toggle == 0)
    {
        D_8008D7EE = 0;
        func_8002CB44();
    }
    D_8008D7EC = toggle;
}
