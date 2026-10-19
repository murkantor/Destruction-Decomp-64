#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0x10]; s32 x10; u8 pad14[0x59]; u8 x6D; u8 pad6E[2]; u64 x70; } State;
typedef struct { u8 pad0[0x38]; State *x38; u8 pad3C[0x4C]; s32 x88; } Car;
typedef struct { u8 pad0[0x10A8]; u8 x10A8[8]; } Game;
extern Game *gGame;
extern s32 D_800958B4;
extern s32 D_80095FF8[];
u8 func_800016DC(Car *);
u8 func_800019C0(Car *);
void func_800067E0(Car *);
u64 Time_Get(void *);

void func_80005C30(Car *arg0) {
    State *s = arg0->x38;
    u64 t;

    switch (s->x10) {
    case 7:
        if (func_800016DC(arg0)) {
            s->x10 = 8;
        }
        if (func_800019C0(arg0)) {
            s->x10 = 9;
        }
        if (arg0->x88 == D_80095FF8[D_800958B4]) {
            s->x10 = 0xC;
        }
        func_800067E0(arg0);
        break;
    case 8:
        break;
    case 9:
        if (!func_800019C0(arg0)) {
            s->x10 = 7;
        }
        break;
    case 10:
        break;
    case 11:
        t = Time_Get(gGame->x10A8);
        if (t > s->x70) {
            s->x6D = 0;
            s->x10 = 7;
            s->x70 = (f64)t + 3000000000.0;
        }
        break;
    case 12:
        break;
    }
}
