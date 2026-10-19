#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;

s32 Gfx_IsCommandKind(Gfx g, s32 kind) {
    u32 w = g.w0;

    switch (kind) {
        case 0:
            return 1;
        case 1:
            return (w >> 24) == 0;
        case 2:
            if ((w >> 24) == 5) {
                return 1;
            }
            return ((w >> 24) == 6) * 2;
        case 3:
            if ((w >> 24) == 0xDB) {
                return 1;
            }
            return (w >> 24) == 0xDC;
        case 4:
            return (w >> 24) == 0xFC;
        case 5:
            return (w >> 24) == 0xE2;
        case 6:
            return (w >> 24) == 0xFA;
        case 7:
            if ((w >> 24) == 0xFD) {
                return 1;
            }
            if ((w >> 24) == 0xF5) {
                return 1;
            }
            if ((w >> 24) == 0xE6) {
                return 1;
            }
            if ((w >> 24) == 0xF4) {
                return 1;
            }
            if ((w >> 24) == 0xE7) {
                return 1;
            }
            return (w >> 24) == 0xF2;
        case 8:
            return (w >> 24) == 0xF4;
    }
    return -1;
}

const s32 D_800BBD94[7] = { 100000, 100000, 100000, -100000, -100000, -100000, 0 };
