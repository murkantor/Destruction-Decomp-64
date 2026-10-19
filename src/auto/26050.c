#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x1100]; void *x1100; } Game;
extern void *Mem_Alloc(u32);
extern void func_80027CF4(void);
extern void Car_CompareRacePosition(void);
typedef struct {
    s32 x0, x4, x8, xC, x10, x14, x18;
    void *x1C;
    s32 x20, x24;
    void *x28;
    s32 x2C, x30, x34, x38, x3C, x40, x44, x48, x4C;
    s64 x50;
    s64 x58;
    u8 pad[0x44];
    s32 xA4, xA8, xAC;
} Sub;
void func_80025450(Game *g) {
    Sub *s;
    s32 c;
    s = Mem_Alloc(0xB0);
    c = 12;
    s->x10 = 0;
    s->x0 = c;
    s->x2C = 0;
    s->x14 = 0;
    s->x20 = 0;
    s->x1C = func_80027CF4;
    s->x28 = Car_CompareRacePosition;
    s->x18 = c;
    s->x30 = 0;
    s->x38 = 0;
    s->x3C = 0;
    s->x40 = 0;
    s->x44 = 0;
    s->x48 = 0;
    s->x24 = c;
    s->x0 = 0;
    s->x4 = 0;
    s->x8 = 0;
    s->xC = 0;
    s->x50 = 0x12A05F2000LL;
    s->x58 = 0x37E11D6000LL;
    s->xA4 = 0;
    s->xA8 = 0;
    g->x1100 = s;
}
