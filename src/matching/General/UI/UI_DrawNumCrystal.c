// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef unsigned int undefined4;

extern char *D_8008D2AC;
extern char D_8008D528;

extern void func_80022878(char *str, int x, int y, int p4, int p5);
extern int func_80078348(char *buf, const char *fmt, ...);

void func_8005111C(int param_1, int param_2, int param_3)
{
    char acStack32[16];

    func_80022878(&D_8008D528, (int)(short)param_1, (param_2 + 4) * 0x10000 >> 0x10, 2, 0);

    func_80078348(acStack32, "%2.02d/%ld",
                  (int)*(signed char *)(param_3 + 0x31),
                  *(undefined4 *)(D_8008D2AC + 0x1E28));

    func_80022878(acStack32, (param_1 + 0xD) * 0x10000 >> 0x10, (int)(short)param_2, 1, 0);
}
