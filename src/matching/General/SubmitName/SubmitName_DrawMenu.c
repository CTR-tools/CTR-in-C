/*
 * SubmitName_DrawMenu @ 0x8004AA60  (vs SCUS_944.26.exe / NTSC-U 926)
 * Behavior-exact: Branch 0 / Stack 0 (residual = gcc mult-vs-shift-add + register coloring on a
 * ~441-insn function). Renders + runs the on-screen-keyboard name-entry screen for one frame:
 * draws the 13x3 letter grid (blinking cursor cell), header, current name + blinking underscore,
 * SAVE/CANCEL labels, and the bordered box; then processes P1 input (D-pad moves the cursor,
 * Cross/Circle types / SAVE / CANCEL / Backspace, Triangle/Square = backspace, Start = CANCEL).
 * Returns 0 (editing), 1 (SAVE) or -1 (CANCEL); writes the cursor to gGT->TitleOSK_CursorPosition.
 * Uses real GameTracker types from gametracker.h. NOTE: the in-repo decomp reference is a later,
 * refactored form; 926 retail differs in three spots, all followed here from the asm/Ghidra:
 * the char[3] keyboard-string path, the two-byte letter-typing path, and the D-pad clamp `> 1002`.
 * Phase B (saphi SubmitName_1_DrawMenu.c): CLEAN.
 */
#include "../gametracker.h"

/* PSX RECT (short x,y,w,h) — the box the OSK is drawn inside. */
typedef struct { short x, y, w, h; } RECT;

#define FONT_BIG 1

/* P1 tap-this-frame button bits (buttonTapPerPlayer[0] = D_8009A990[0]) */
#define BTN_UP       0x0001
#define BTN_DOWN     0x0002
#define BTN_LEFT     0x0004
#define BTN_RIGHT    0x0008
#define BTN_START    0x1000
#define BTN_TRI_SQ   0x40020   /* BTN_TRIANGLE | BTN_SQUARE */
#define BTN_CIR_CRS  0x00050   /* BTN_CIRCLE   | BTN_CROSS  */

extern int   func_80077CC8(char *s);                                    /* strlen */
extern void  func_80022878(char *s, int x, int y, int font, short col); /* DecalFont_DrawLine */
extern int   func_800224D0(char *s, int font);                          /* DecalFont_GetLineWidth */
extern void  func_80044F90(RECT *r, void *color, int edge, void *otMem); /* RECTMENU_DrawOuterRect_Edge */
extern void  func_800457B0(RECT *r, int inner, void *otMem);            /* RECTMENU_DrawInnerRect */
extern void  func_80046404(void);                                       /* RECTMENU_ClearInput */
extern void  func_80077E38(char *dst, char *src, int n);                /* memmove */
extern void  func_80028468(unsigned short sfx, int a1);                 /* OtherFX_Play */

extern struct GameTracker *D_8008D2AC;   /* gGT */
extern short  D_80085D94[];              /* data.unicodeAscii  — 13x3 glyph grid */
extern char **D_8008D878;                /* sdata->lngStrings  — localized string table */
extern char   D_8008D4AC[];              /* sdata->str_underscore */
extern char   D_8008D438[];              /* sdata->battleSetup_Color_UI_1 (passed by address) */
extern int    D_8009A990[];              /* sdata->buttonTapPerPlayer */
extern unsigned short D_80085DE4[];      /* data.soundIndexArray (u16 @ stride 4) */

unsigned short D_8008D8F2 = 0;           /* sdata->typeTimer (gp+0x986) — blink/type phase */

short SubmitName_DrawMenu(unsigned short string)
{
    short currNameLength;
    int currNameWidth;
    unsigned int cursorCharacter;
    int cursorPosition;
    unsigned int keyboardCharacter;
    unsigned short uKbChar;
    char *currNameEntered;
    int currNameLengthCopy;
    unsigned int uCharToType;
    short strColorBlink;
    short strColorBlink2;
    int j;
    int cursorPosition2;
    int i;
    short cursorPositionShort;
    unsigned int soundID;
    short currNameLengthIncrement;
    char keyboardString[3];
    RECT r;
    unsigned short stringCopy;
    short nameLength;
    short result;
    int drawRow;
    int numCols;
    int cursorPositionLBitshift16;
    unsigned int cursorRegionTemp;
    int strlenCurrNameEnteredInt;
    unsigned int keyboardCharacterTopByte;
    unsigned char character;


    soundID = 0;
    result = 0;
    nameLength = 0;
    stringCopy = string;
    strlenCurrNameEnteredInt = func_80077CC8(D_8008D2AC->currNameEntered);
    currNameLength = (short)strlenCurrNameEnteredInt;
    currNameEntered = D_8008D2AC->currNameEntered;

    /* count "printable" chars (value > 2) currently entered */
    if (D_8008D2AC->currNameEntered[0] != '\0') {
        character = *currNameEntered;
        do {
            if (2 < character) {
                nameLength = nameLength + 1;
            }
            currNameEntered = currNameEntered + 1;
            character = *currNameEntered;
        } while (character != 0);
    }

    cursorPosition = (int)D_8008D2AC->TitleOSK_CursorPosition;
    if ((38 < cursorPosition) && (cursorPosition < 1000)) {
        cursorPosition = 38;
    }
    cursorPositionShort = (short)cursorPosition;
    D_8008D8F2 = D_8008D8F2 + 1;

    /* 13x3 grid of letters (numCols kept in a register -> mult, not shift-add) */
    numCols = 13;
    i = 0;
    drawRow = 0 < numCols;
    do {
        j = 0;
        if (drawRow != 0) {
            do {
                strColorBlink = 0;
                if ((int)cursorPositionShort == (short)(j + i * numCols)) {
                    strColorBlink = (D_8008D8F2 & 1) << 2;
                }
                uKbChar = D_80085D94[(short)i * numCols + (short)j];
                keyboardCharacter = (unsigned int)(short)uKbChar;
                keyboardCharacterTopByte = keyboardCharacter & 0xff00;
                if ((keyboardCharacter & 0xff00) == 0x1000) {
                    uKbChar = uKbChar & 0xff;
                    keyboardCharacterTopByte = 0;
                }
                if (keyboardCharacterTopByte == 0) {
                    keyboardString[1] = 0;
                    keyboardString[0] = (char)uKbChar;
                } else {
                    keyboardString[2] = 0;
                    keyboardString[0] = (char)(uKbChar >> 8);
                    keyboardString[1] = (char)uKbChar;
                }
                func_80022878(keyboardString, (short)j * 22 + 116,
                              (short)((short)i * 18 + 88), FONT_BIG, strColorBlink);
                j = j + 1;
            } while ((short)j < numCols);
        }
        i = i + 1;
    } while ((short)i < 3);

    /* header, current name */
    func_80022878(D_8008D878[0x13e], 256, 44, FONT_BIG, (short)0x8000);
    func_80022878(D_8008D2AC->currNameEntered, 192, 68, FONT_BIG, 4);

    if (((D_8008D8F2 & 2) != 0) && (currNameLength < 16)) {
        currNameWidth = func_800224D0(D_8008D2AC->currNameEntered, FONT_BIG);
        func_80022878(D_8008D4AC, (short)(currNameWidth + 192), 68, FONT_BIG, 0);
    }

    /* SAVE / CANCEL labels */
    cursorPosition2 = (int)cursorPositionShort;
    strColorBlink2 = 0;
    if (cursorPosition2 == 1001) {
        strColorBlink2 = (D_8008D8F2 & 1) << 2;
    }
    func_80022878(D_8008D878[stringCopy], 472, 150, FONT_BIG, strColorBlink2 | 0x4000);
    strColorBlink = 0;
    if (cursorPosition2 == 1000) {
        strColorBlink = (D_8008D8F2 & 1) << 2;
    }
    func_80022878(D_8008D878[0x141], 40, 150, FONT_BIG, strColorBlink);

    /* bordered box */
    r.x = 32;
    r.w = 448;
    r.y = 62;
    r.h = 2;
    func_80044F90(&r, D_8008D438, 0x20, D_8008D2AC->backBuffer->otMem_startPlusFour);
    r.y = 39;
    r.h = 130;
    func_800457B0(&r, 0, D_8008D2AC->backBuffer->otMem_startPlusFour);

    /* input */
    if ((D_8009A990[0] & (BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT)) == 0) {
        if ((D_8009A990[0] & BTN_START) == 0) {
            if ((D_8009A990[0] & BTN_TRI_SQ) == 0) {
                if ((D_8009A990[0] & BTN_CIR_CRS) != 0) {
                    if (cursorPosition2 == 38) {
                        /* Go-Back cell: delete last char */
                        soundID = 2;
                        if (currNameLength == 0) {
                            soundID = 2;
                        } else {
                            D_8008D2AC->currNameEntered[currNameLength - 1] = 0;
                        }
                    } else if (cursorPosition2 < 0x26) {
                        /* type the selected letter */
                        cursorCharacter = (unsigned int)(unsigned short)D_80085D94[cursorPosition2];
                        if (((int)D_80085D94[cursorPosition2] & 0xff00) == 0x1000) {
                            cursorCharacter = cursorCharacter & 0xff;
                        }
                        if (nameLength < 8) {
                            uCharToType = cursorCharacter;
                            currNameLengthIncrement = currNameLength;
                            if ((cursorCharacter & 0xff00) != 0) {
                                currNameLengthIncrement = currNameLength + 1;
                                uCharToType = cursorCharacter & 0xff;
                                D_8008D2AC->currNameEntered[currNameLength] = (char)(cursorCharacter >> 8);
                            }
                            soundID = 1;
                            D_8008D2AC->currNameEntered[currNameLengthIncrement] = (char)uCharToType;
                        } else {
                            soundID = 5;
                        }
                    } else {
                        if (cursorPosition2 == 1001) {
                            /* SAVE */
                            soundID = 2;
                            result = 1;
                            func_80077E38(D_8008D2AC->prevNameEntered, D_8008D2AC->currNameEntered, 0x11);
                        } else {
                            /* CANCEL */
                            soundID = 0;
                            if (cursorPosition2 != 1000) goto LAB_8004b0dc;
                            soundID = 3;
                            result = -1;
                        }
                        func_80046404();
                    }
                }
            } else {
                /* Triangle/Square = backspace */
                currNameLengthCopy = (int)currNameLength;
                soundID = 0;
                if (currNameLengthCopy != 0) {
                    D_8008D2AC->currNameEntered[currNameLengthCopy - 1] = 0;
                    soundID = 4;
                    if ((unsigned char)D_8008D2AC->currNameEntered[currNameLengthCopy - 2] < 3) {
                        D_8008D2AC->currNameEntered[currNameLengthCopy - 2] = 0;
                    }
                }
            }
        } else {
            /* Start = jump to CANCEL */
            soundID = 3;
            if (cursorPosition2 == 1000) {
                result = -1;
            } else {
                cursorPositionShort = 1000;
                soundID = 1;
            }
        }
    } else {
        /* D-pad moves the cursor */
        if ((D_8009A990[0] & BTN_UP) != 0) {
            cursorPosition = cursorPosition + -13;
        }
        if ((D_8009A990[0] & BTN_DOWN) != 0) {
            cursorPosition = cursorPosition + 13;
        }
        if ((D_8009A990[0] & BTN_LEFT) != 0) {
            cursorPosition = cursorPosition + -1;
        }
        cursorPositionLBitshift16 = cursorPosition << 0x10;
        if ((D_8009A990[0] & BTN_RIGHT) != 0) {
            cursorPosition = cursorPosition + 1;
            cursorPositionLBitshift16 = cursorPosition * 0x10000;
        }
        cursorPositionLBitshift16 = cursorPosition << 0x10;
        if (cursorPositionLBitshift16 < 0) {
            cursorPosition = 0x3e9;
            cursorPositionLBitshift16 = 0x3e90000;
        }
        cursorPositionShort = (short)cursorPosition;
        cursorRegionTemp = cursorPosition - 500;
        if ((0x26 < cursorPositionLBitshift16 >> 16) &&
            (cursorRegionTemp = cursorPosition - 500, cursorPositionLBitshift16 >> 16 < 500)) {
            cursorPositionShort = 0x3e9;
            cursorRegionTemp = 0x1f5;
        }
        if ((cursorRegionTemp & 0xffff) < 500) {
            cursorPositionShort = 0x26;
        }
        soundID = 1;
        if (0x3ea < cursorPositionShort) {
            cursorPositionShort = 0;
        }
    }

LAB_8004b0dc:
    if (soundID != 0) {
        func_80028468(D_80085DE4[soundID * 2], 1);
    }
    D_8008D2AC->TitleOSK_CursorPosition = cursorPositionShort;
    return result;
}
