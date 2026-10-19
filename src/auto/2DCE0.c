#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; u8 pad4[0xC]; } Entry;
extern s32 D_80095CB8[];
extern Entry *D_80095CA8[];
extern u8 D_80095CC8[12];
extern s32 D_80095D00[];
extern u8 D_80095CD3[];
extern s32 D_801252F4;

void func_8002D0E0(s32 k) {
    s32 n = D_80095CB8[k];
    s32 i;

    {
        u8 *p;

        i = 11;
        p = D_80095CD3;
        for (; i >= 0; i--) {
            *p-- = 0;
        }
    }
    for (i = 0; i < n; i++) {
        D_80095CC8[D_80095CA8[k][i].x0] = 1;
    }
    for (i = 11; i >= 0; i--) {
        D_80095D00[i - 11] = 0;
    }
    D_801252F4 = 0;
}
