#include "common.h"
#include "m2c.h"

extern u8 D_800B0BC4[];
typedef struct { u8 a, b; } P2;
#define T ((P2 *)D_800B0BC4)
void Text_MapChars(u8 *src, u8 *dst, s32 n) {
    s32 i; u32 ch; s32 j;
    for (i = 0; i < n; i++) { ch = *src; for (j = 0; j < 0x68; j += 2) { if (ch == D_800B0BC4[j]) { *dst = D_800B0BC4[j + 1]; break; } } src++; dst++; }
}
