#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10A8];
    u64 x10A8;
} Game;

typedef struct {
    u64 x68;
    u64 x70;
    u8 pad10[0x1D - 0x10];
    s8 x85;
    u8 pad1E[0x20 - 0x1E];
} Slot;

typedef struct {
    u8 pad0[0x10];
    s8 x10;
} Info;

typedef struct {
    u8 pad0[0x30];
    s32 x30;
    u8 pad34[0x3C - 0x34];
    Info *x3C;
} Car;

extern Game *gGame;
extern s8 D_800D31CC;
extern s32 D_8007EF18;
extern Slot D_8007EF68[][10];
u64 Time_Get(u64 *);

void func_8000F1C4(s32 a0, Car *car, s32 a2, s32 a3) {
    s32 idx;
    u64 t;
    s32 flag;

    if (car->x30 != 1 && D_800D31CC < 3) {
        idx = car->x3C->x10;
        t = Time_Get(&gGame->x10A8);
        flag = D_8007EF18 < a2;
        if ((f64)D_8007EF68[idx][a3].x70 != (f64)t + 1000000000.0) {
            D_8007EF68[idx][a3].x85 = flag;
        } else if (D_8007EF68[idx][a3].x85 < flag) {
            D_8007EF68[idx][a3].x85 = flag;
        }
        D_8007EF68[idx][a3].x68 = (f64)t + 2500000.0;
        D_8007EF68[idx][a3].x70 = (f64)t + 1000000000.0;
    }
}
