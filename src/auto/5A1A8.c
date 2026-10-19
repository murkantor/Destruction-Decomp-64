#include "common.h"
#include "m2c.h"

typedef struct { u8 c; u8 k; } CharMap;
extern CharMap D_800B0BC4[];

void Text_MapChars2(u8 *src, u8 *dst, s32 n) {
    s32 i;
    s32 j;
    u8 ch;

    for (i = 0; i < n; i++) {
        ch = *src;
        for (j = 0; j < 0x34; j++) {
            if (ch == D_800B0BC4[j].k) {
                if (n == 4) {
                    if (ch >= 0x34) {
                        *dst = ' ';
                        break;
                    }
                }
                *dst = D_800B0BC4[j].c;
                break;
            }
        }
        if (j == 0x34) {
            if (n == 16) {
                *dst = '-';
            } else {
                *dst = ' ';
            }
        }
        src++;
        dst++;
    }
}
