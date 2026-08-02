/*
 * MEMCARD_ProcessCardOperation @ 0x8003DDAC  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact match: base score 10650, Branch 0, Stack 0, Register 46
 * (register-coloring/scheduling residual only; natural C, no hacks).
 *
 * Core memcard state-machine dispatcher (switch on stage 1..0xE), polled each frame: advances GETINFO/NEWCARD/LOAD parts 0-5/SAVE parts 0-2/ERASE and returns an MC_RETURN code (PENDING while in progress). Full control flow + stack match; residual is register coloring across 291 instructions.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_RetailEvents.c audited CLEAN vs this asm + Ghidra.
 */
extern int func_8003D9EC(void);    /* GetNextSwEvent */
extern void func_8003DB54(void);   /* SkipEvents */
extern int func_8003DAE4(void);    /* WaitForHwEvent */
extern long func_80078338(int slot);/* _card_load */
extern long func_80077FF8(int slot);/* _card_clear */
extern void func_8003DD10(int slot);/* GetFreeBytes */
extern int func_8003DC30(int off, int size);            /* ReadFile */
extern int func_8003DC9C(int off, void *buf, int size); /* WriteFile */
extern int func_8003D618(int base, int len);            /* ChecksumLoad */
extern void func_8003DBF8(void);   /* CloseFile */
extern unsigned char D_800857A0[]; /* g_abMemcardIconActive */
int D_8008D404 = 0;   /* stage        (gp+0x498) */
int D_8008D8CC = 0;   /* statusFlags  (gp+0x960) */
int D_8008D8B8 = 0;   /* slot         (gp+0x94C) */
int D_8008D8AC = 0;   /* sizeRemaining(gp+0x940) */
unsigned char *D_8008D408 = 0; /* g_pMemcardStart (gp+0x49C) */
int D_8008D8A8 = 0;   /* fileSize     (gp+0x93C) */
int D_8008D8D8 = 0;   /* iconSize     (gp+0x96C) */
int D_8008D410 = 0;   /* blockCount/retry (gp+0x4A4) */
int D_8008D8B0 = 0;   /* crcProgress  (gp+0x944) */
int D_8008D8D0 = 0;   /* crc16        (gp+0x964) */

int MEMCARD_ProcessCardOperation(void)
{
    long cardResult;
    int offset;
    int size;
    int result;
    int iFileOffset;
    unsigned char *writeBuf;

    switch (D_8008D404) {
    case 1:
        result = func_8003D9EC();
        if (result == 0) {
            if ((D_8008D8CC & 1) != 0) {
                D_8008D404 = 2;
                func_8003DB54();
                do { cardResult = func_80078338(D_8008D8B8); } while (cardResult != 1);
                return 7;
            }
            result = 0;
            if ((D_8008D8CC & 2) == 0) {
                D_8008D8AC = 0;
                result = 5;
            }
        } else {
            if (result != 3) {
                if (result != 7) {
                    D_8008D404 = 0;
                    D_8008D8AC = 0;
                    return result;
                }
                return 7;
            }
            func_8003DB54();
            do { cardResult = func_80077FF8(D_8008D8B8); } while (cardResult != 1);
            result = func_8003DAE4();
            if (result == 0) {
                D_8008D404 = 2;
                func_8003DB54();
                do { cardResult = func_80078338(D_8008D8B8); } while (cardResult != 1);
                return 7;
            }
        }
        goto LAB_df38;
    case 2:
        result = func_8003D9EC();
        if (result == 0) {
            D_8008D404 = 0;
            D_8008D8CC = D_8008D8CC & 0xfffffffe | 2;
            func_8003DD10(D_8008D8B8);
            return 3;
        }
        if (result == 3) {
            D_8008D404 = 0;
            D_8008D8CC = D_8008D8CC & 0xfffffffc;
            return 5;
        }
        if (result == 7) {
            return 7;
        }
LAB_df38:
        D_8008D404 = 0;
        break;
    case 3:
        result = func_8003D9EC();
        if (result == 0) {
            D_8008D404 = 4;
            D_8008D8D8 = ((D_8008D408[2] & 0xf) + 1) * 0x80;
            result = func_8003DC30(D_8008D8D8, D_8008D8A8);
            if ((D_8008D8D8 + D_8008D8A8 + 0x1fff >> 0xd < (int)(unsigned int)D_8008D408[3]) &&
                (1 < D_8008D8D8 + D_8008D8A8 * 2 + 0x1fff >> 0xd)) {
                D_8008D8CC = D_8008D8CC & 0xfffffffb;
                return result;
            }
            D_8008D8CC = D_8008D8CC | 4;
            return result;
        }
        if (result == 7) {
            return 7;
        }
        if (0 < D_8008D410) {
            iFileOffset = 0;
            offset = 0x80;
            D_8008D410 = D_8008D410 + -1;
            goto LAB_e010;
        }
        goto LAB_e1fc;
    case 4:
    case 6:
        result = func_8003D9EC();
        if (result == 0) {
            D_8008D8B0 = 0;
            D_8008D8D0 = 0;
            D_8008D404 = D_8008D404 + 1;
            if ((D_8008D8CC & 8) == 0) {
                D_8008D8B0 = 0;
                D_8008D8D0 = 0;
                return 7;
            }
            goto LAB_case5;
        }
        if (result == 7) {
            return 7;
        }
        if (0 < D_8008D410) {
            iFileOffset = D_8008D8D8 + (D_8008D404 + -4) * D_8008D8A8;
            offset = D_8008D8A8;
            D_8008D410 = D_8008D410 + -1;
            goto LAB_e010;
        }
        goto LAB_e1fc;
    case 5:
    case 7:
LAB_case5:
        result = func_8003D618((int)D_8008D408, D_8008D8A8);
        if (result != 0) {
            if (result == 7) {
                return 7;
            }
            if (((D_8008D8CC & 4) == 0) && (offset = D_8008D404 + -3, D_8008D404 < 7)) {
                D_8008D404 = D_8008D404 + 1;
                iFileOffset = D_8008D8D8 + offset * D_8008D8A8;
                offset = D_8008D8A8;
LAB_e010:
                result = func_8003DC30(iFileOffset, offset);
                return result;
            }
        }
LAB_e1fc:
        func_8003DBF8();
        break;
    default:
        result = 1;
        break;
    case 9:
    case 10:
    case 0xb:
        result = func_8003D9EC();
        if (result == 0) {
            if ((D_8008D404 != 9) && ((10 < D_8008D404) || ((D_8008D8CC & 4) != 0))) {
                func_8003DBF8();
                func_8003DD10(D_8008D8B8);
                return 0;
            }
            offset = D_8008D404 + -9;
            D_8008D404 = D_8008D404 + 1;
LAB_e1e4:
            iFileOffset = D_8008D8D8 + offset * D_8008D8A8;
            writeBuf = D_8008D408;
            offset = D_8008D8A8;
        } else {
            if (result == 7) {
                return 7;
            }
            size = D_8008D410 + -1;
            if (D_8008D410 < 1) {
                goto LAB_e1fc;
            }
            offset = D_8008D404 + -10;
            D_8008D410 = size;
            if (D_8008D404 != 9) {
                goto LAB_e1e4;
            }
            iFileOffset = 0;
            writeBuf = D_800857A0;
            offset = D_8008D8D8;
        }
        result = func_8003DC9C(iFileOffset, writeBuf, offset);
        break;
    case 0xd:
        D_8008D404 = 0;
        result = 1;
        break;
    case 0xe:
        D_8008D404 = 0;
        result = 0;
    }
    return result;
}
