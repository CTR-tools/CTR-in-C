// CTR_ScrambleGhostString @ 0x80022234  (vs SCUS_944.26.exe)
//
// SCORE 0 exact match.
//
// Decodes a scrambled ghost/name string: walks `src`, builds a 16-bit lookup
// key, finds it in the table at D_80081DFC and emits the mapped bytes to `dst`.
// Table = array of {u16 encoded; u16 key} entries, terminated by key == 0xFFFF.
// Per output symbol: a leading byte < 4 becomes the key's high byte (a two-byte
// escape), then the next src byte is OR'd in as the low byte. On a matching
// entry whose `encoded` has a non-zero high byte, both encoded bytes are written
// (high then low) and the scan stops; the string is NUL-terminated at the end.
//
// Two matching keys recovered here:
//   * `inputByte` is a wide unsigned int (not a u8): this yields `sltiu` for the
//     `< 4` test and the retail's `or key,key,inputByte` operand order.
//   * The table walk loads `entry->key` ONCE per iteration into a local (ekey)
//     used by both the terminator test and the key compare; reading entry->key
//     twice makes gcc strength-reduce it into a second induction pointer (520 vs
//     0). A {encoded,key} struct (single pointer, offset addressing) is required
//     over a bare u16[] index.

#include "CTR.h"

struct GhostEntry { u16 encoded; u16 key; };   // key==0xFFFF terminates the table

extern struct GhostEntry D_80081DFC[];   // ghost scramble table

void CTR_ScrambleGhostString(char *dst, char *src)
{
    u_int inputByte = *src;

    while (inputByte != 0)
    {
        u32 key = 0;

        if (inputByte < 4)
        {
            inputByte = *src++;
            key = (u32)inputByte << 8;
        }

        inputByte = *src++;
        key |= inputByte;

        {
            struct GhostEntry *entry = D_80081DFC;
            u16 ekey;
            while ((ekey = entry->key) != 0xffff)
            {
                if (ekey == (key & 0xffff))
                {
                    u16 encoded = entry->encoded;
                    if ((encoded & 0xff00) != 0)
                    {
                        *dst++ = encoded >> 8;
                        *dst++ = encoded;
                        break;
                    }
                }
                entry++;
            }
        }

        inputByte = *src;
    }

    *dst = 0;
}
