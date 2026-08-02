/*
 * MEMCARD_QueryCard @ 0x8003E238  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact match: score 0 (Branch, Stack, Register differences all 0).
 *
 * Starts a card-info query for the slot (only if idle): flushes events, stage=GETINFO, spins _card_info until accepted, retry 8; returns PENDING (TIMEOUT if busy).
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_RetailCard.c audited CLEAN vs this asm + Ghidra.
 */
extern void func_8003DB54(void);      /* SkipEvents */
extern long func_80077EA8(int slot);  /* _card_info */
int D_8008D404 = 0;  /* stage 0x498 */
int D_8008D8B8 = 0;  /* slot  0x94C */
int D_8008D410 = 0;  /* retry 0x4A4 */
int MEMCARD_QueryCard(int slotIdx)
{
    long accepted;
    if (D_8008D404 != 0)
        return 1;
    func_8003DB54();
    D_8008D8B8 = slotIdx;
    D_8008D404 = 1;
    do { accepted = func_80077EA8(slotIdx); } while (accepted != 1);
    D_8008D410 = 8;
    return 7;
}
