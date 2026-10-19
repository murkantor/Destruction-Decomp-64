#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x4]; void *x4; u8 pad8[0x54 - 0x8]; u16 x54; } UnkA;
typedef struct { u8 pad[0x6]; u8 x6; u8 x7; u8 pad8[2]; u16 xA; u8 padC[0x14 - 0xC]; s32 x14; s32 x18; u8 pad1C[0x22 - 0x1C]; u16 x22; } UnkH;
s32 Arc_SeekToEntryData(UnkA *, UnkH *);
s32 RomFile_Read(u8 *buf, u32 size, s32 count, void *f);
s32 func_800524E4(void *, s32, u8 *, s32);

static inline s32 load8(UnkA *a, UnkH *h, u8 *buf) {
    if (h->x6 >= 0x15 || h->x7 != 0 || h->x22 != a->x54) {
        return -2;
    }
    if (Arc_SeekToEntryData(a, h) != 0) {
        return -1;
    }
    if (func_800524E4(a->x4, h->x14, buf, h->x18) != 0) {
        return -3;
    }
    return 0;
}

s32 func_80053050(UnkA *a, UnkH *h, u8 *buf) {
    s32 r;
    if (h->xA == 0) {
        if (h->x14 != h->x18) {
            return -3;
        }
        r = Arc_SeekToEntryData(a, h);
        if (r != 0) {
            return r;
        }
        return -(RomFile_Read(buf, h->x14, 1, a->x4) != 1);
    }
    if (h->xA == 8) {
        return load8(a, h, buf);
    }
    return -2;
}
