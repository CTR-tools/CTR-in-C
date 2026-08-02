/*
 * MEMCARD_FormatCard @ 0x8003E51C  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact match: base score 310, Branch 0, Stack 0, Register 2
 * (register-coloring/scheduling residual only; natural C, no hacks).
 *
 * Formats the card (only if idle): format(path); on success flush events, stage=NEWCARD, spin _card_load until accepted, retry 8, PENDING; TIMEOUT if busy or format fails.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_RetailCard.c audited CLEAN vs this asm + Ghidra.
 */
extern char *func_8003D6E8(int slot, char *dst);
extern long func_80078038(char *path);
extern void func_8003DB54(void);
extern long func_80078338(int slot);
int D_8008D404 = 0, D_8008D8B8 = 0, D_8008D410 = 0;
int MEMCARD_FormatCard(int slotIdx)
{
    char *pPath;
    long r;
    int result;
    if (D_8008D404 == 0) {
        pPath = func_8003D6E8(slotIdx, 0);
        r = func_80078038(pPath);
        result = 1;
        if (r == 1) {
            D_8008D8B8 = slotIdx;
            D_8008D404 = 2;
            func_8003DB54();
            do { r = func_80078338(D_8008D8B8); } while (r != 1);
            D_8008D410 = 8;
            result = 7;
        }
    } else {
        result = 1;
    }
    return result;
}
