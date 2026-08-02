/*
 * MEMCARD_SaveFile @ 0x8003E344  (matching vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact match: base score 2555, Branch 0, Stack 0, Register 43
 * (register-coloring/scheduling residual only; natural C, no hacks).
 *
 * Begins saving (only if idle and NewTask succeeds): builds the 0x100-byte save header in g_abMemcardIconActive (block-count byte[3], status bit4, 0x81/0x40 title fill then iconName copy), checksums the data, create-then-reopen the file FASYNC|FWRITE, stage=SAVE_PART0_START, starts the header write. FULL(4) on open fail. Full control flow + stack match; residual is register coloring.
 *
 * Phase B: saphi game/MEMCARD/MEMCARD_RetailTransfer.c audited CLEAN vs this asm + Ghidra.
 */
extern int  func_8003DB98(int slot, char *name, void *ptr, int size);  /* NewTask */
extern void func_8003D584(void *buf, int len);                         /* ChecksumSave */
extern int  func_80078078(char *path, unsigned int mode);              /* open */
extern void func_80077ED8(int fd);                                     /* close */
extern int  func_8003DC9C(int off, void *buf, int size);               /* WriteFile */
extern void func_8003DBF8(void);                                       /* CloseFile */
extern unsigned char D_800857A0[];   /* g_abMemcardIconActive (0x100 header) */
extern char D_800990A4[];            /* g_szMemcardFileCurr */
int D_8008D404 = 0;  /* stage       0x498 */
int D_8008D8D8 = 0;  /* iconSize     0x96C */
int D_8008D8CC = 0;  /* statusFlags  0x960 */
int D_8008D40C = 0;  /* fd           0x4A0 */
int MEMCARD_SaveFile(int slotIdx, char *name, unsigned char *iconName, void *ptrMemcard, int memcardFileSize, unsigned int flags)
{
    int iScratch;
    int iFillNext;
    unsigned char bNameChar;
    unsigned char *pIcon;
    if (D_8008D404 == 0) {
        D_8008D8D8 = 0x100;
        iScratch = func_8003DB98(slotIdx, name, ptrMemcard, memcardFileSize);
        if (iScratch == 0) {
            pIcon = D_800857A0;
            if (((flags & 1) == 0) && (1 < (D_8008D8D8 + memcardFileSize * 2 + 0x1fff >> 0xd))) {
                pIcon[3] = (unsigned char)(D_8008D8D8 + memcardFileSize + 0x1fff >> 0xd);
                D_8008D8CC = D_8008D8CC | 4;
            } else {
                D_8008D8CC = D_8008D8CC & 0xfffffffb;
                pIcon[3] = (unsigned char)(D_8008D8D8 + memcardFileSize * 2 + 0x1fff >> 0xd);
            }
            iScratch = 0;
            do {
                iFillNext = iScratch + 2;
                pIcon[iScratch + 4] = 0x81;
                pIcon[iScratch + 5] = 0x40;
                iScratch = iFillNext;
            } while (iFillNext < 0x40);
            bNameChar = *iconName;
            for (iScratch = 0; (bNameChar != 0 && iScratch < 0x40); iScratch = iScratch + 1) {
                bNameChar = *iconName;
                iconName = iconName + 1;
                pIcon[iScratch + 4] = bNameChar;
                bNameChar = *iconName;
            }
            func_8003D584(ptrMemcard, memcardFileSize);
            D_8008D40C = func_80078078(D_800990A4, (unsigned int)pIcon[3] << 0x10 | 0x200);
            if (D_8008D40C != -1) {
                func_80077ED8(D_8008D40C);
                D_8008D40C = -1;
            }
            D_8008D40C = func_80078078(D_800990A4, 0x8002);
            if (D_8008D40C != -1) {
                D_8008D404 = 9;
                iScratch = func_8003DC9C(0, pIcon, D_8008D8D8);
                return iScratch;
            }
            func_8003DBF8();
            return 4;
        }
    }
    return 1;
}
