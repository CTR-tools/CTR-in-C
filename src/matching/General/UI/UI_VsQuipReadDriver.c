typedef unsigned int uint;
typedef unsigned int undefined4;
typedef unsigned char byte;

uint func_80054A08(int param_1, int param_2, int param_3)
{
    switch (param_3) {
        case 1:
            return (uint)*(byte *)(param_1 + param_2);
        case 2:
            return (int)*(short *)(param_1 + param_2);
        case 4:
            return *(undefined4 *)(param_1 + param_2);
    }
    return 0;
}
