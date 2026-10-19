#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x5C]; s32 x5C; s32 x60; } Screen;
typedef struct { u8 pad[0x8C]; Screen *x8C; } Ctx;
extern u8 D_800BD1F0[];
Ctx *func_800563F0(void);
s32 func_80054DD0(void *, s32);
void func_80054EDC(void *, s32, s32, s32, s32);

void func_80057964(s32 a, s32 x, s32 y, s32 d) {
    Ctx *c = func_800563F0();
    x = x * (1.0f / 320.0f) * c->x8C->x5C;
    y = y * (1.0f / 240.0f) * c->x8C->x60;
    func_80054EDC(D_800BD1F0, x, y, d, func_80054DD0(D_800BD1F0, a));
}
