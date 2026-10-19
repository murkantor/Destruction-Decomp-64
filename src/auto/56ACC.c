#include "common.h"
#include "m2c.h"

typedef struct { u32 data[20]; } OSViMode;
typedef struct {
    OSViMode mode;
    s32 x50;
    f32 x54;
    f32 x58;
    s32 x5C;
    s32 x60;
    u8 pad64[4];
    s32 x68;
    s32 x6C;
    u8 pad70[4];
    s32 x74;
    u8 pad78[4];
    u8 x7C;
    u8 x7D;
} ViConfig;
extern OSViMode osViModeTable[];
extern s32 D_800B0B08;

void Vi_InitConfig(ViConfig *c, u8 mode) {
    s32 idx = D_800B0B08;

    idx += mode * 14;
    c->mode = osViModeTable[idx];
    c->x50 = 0;
    c->x5C = 0;
    c->x60 = 0;
    c->x74 = 0;
    c->x68 = 0;
    c->x6C = 0;
    c->x7C = 0;
    c->x7D = 1;
    c->x54 = 1.0f;
    c->x58 = 1.0f;
}
