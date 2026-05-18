// ===================================================
// Should be score 0, needs types analysis/replacement
// ===================================================

typedef unsigned int undefined4;

extern char *D_8008D2AC;
extern char D_8008D528;

extern void func_80022878(char *str, int x, int y, int p4, int p5);
extern int func_80078348(char *buf, const char *fmt, ...);

void func_80050E6C(int param_1, int param_2, int param_3)
{
    char acStack32[16];

    func_80022878(&D_8008D528, (param_1 + 0x14) * 0x10000 >> 0x10, (param_2 + -10) * 0x10000 >> 0x10, 2, 0);

    func_80078348(acStack32, "%2.02d/%ld",
                  (int)*(signed char *)(param_3 + 0x32),
                  *(undefined4 *)(D_8008D2AC + 0x1E2C));

    func_80022878(acStack32, (param_1 + 0x21) * 0x10000 >> 0x10, (param_2 + -0xE) * 0x10000 >> 0x10, 1, 0);
}
