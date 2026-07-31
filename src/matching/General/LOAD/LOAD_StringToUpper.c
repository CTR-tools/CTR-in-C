// LOAD_StringToUpper @ 0x80031c1c  (vs SCUS_944.26.exe)
// SCORE 0. In-place ASCII lowercase->uppercase over a NUL-terminated string:
// for each c, if (unsigned)(c-0x61) < 0x1a then c += 0xE0 (== c-0x20 as a byte).
// (Retail uses the +0xE0 byte-wrap constant, not -0x20.)
#include "CTR.h"

void LOAD_StringToUpper(char *path)
{
    char *letter = path;
    char c = *letter;

    while (c != 0)
    {
        if ((unsigned int)(c - 0x61) < 0x1a)
            *letter = c + 0xe0;
        letter++;
        c = *letter;
    }
}
