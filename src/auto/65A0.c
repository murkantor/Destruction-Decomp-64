#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    u8 pad0[0x10];
    s32 x10;
    f32 x14;
    void *x18;
    u8 pad1C[0x38 - 0x1C];
    f32 x38;
    s32 x3C;
    u8 pad40[0x4C - 0x40];
    s32 x4C;
    u8 pad50[0x6C - 0x50];
    u8 x6C;
    u8 x6D;
    u8 pad6E[2];
    u64 x70;
} AiCtrl;
typedef struct { u8 pad0[0x10]; s32 x10; u8 pad14[0x38 - 0x14]; AiCtrl *x38; } AiCar;
typedef struct { u8 pad[0x10A8]; u64 time; u8 pad10B0[0x1100 - 0x10B0]; s32 *level; } Game;
extern Game *gGame;
Vec3f func_8001FBA8(void *);
f32 Vec3f_DistXZ(Vec3f a, Vec3f b);
void func_80000F9C(AiCar *);
void func_800067E0(AiCar *);
u64 Time_Get(u64 *);
s32 __cmpdi2(s64, s64);

void Ai_UpdateState(AiCar *car) {
    AiCtrl *ctrl = car->x38;
    s32 *lvl = gGame->level;
    s32 flag = 0;
    Vec3f a;

    switch (ctrl->x10) {
    case 0: {
        Vec3f b;
        if (car->x10 < ctrl->x3C && *lvl >= 7) {
            ctrl->x10 = 1;
        }
        a = func_8001FBA8(car);
        b = func_8001FBA8(ctrl->x18);
        if (Vec3f_DistXZ(a, b) < ctrl->x38) {
            ctrl->x10 = 2;
        }
        if (flag) {
            ctrl->x10 = 3;
        }
        if (ctrl->x6C) {
            ctrl->x10 = 4;
        }
        if (*lvl < 5 && ctrl->x4C != 6) {
            ctrl->x4C = 6;
            func_80000F9C(car);
        }
        func_800067E0(car);
        break;
    }
    case 1:
        if (*lvl < 6) {
            ctrl->x10 = 0;
            func_80000F9C(car);
        }
        if (flag) {
            ctrl->x10 = 3;
        }
        break;
    case 2: {
        Vec3f c;
        a = func_8001FBA8(car);
        c = func_8001FBA8(ctrl->x18);
        if (Vec3f_DistXZ(a, c) < ctrl->x38) {
            ctrl->x10 = 0;
        } else if (ctrl->x14 < 10.0f) {
            ctrl->x10 = 0;
        }
        break;
    }
    case 3:
        if (*lvl >= 7) {
            ctrl->x10 = 0;
            func_80000F9C(car);
        }
        break;
    case 5:
        if (ctrl->x70 < Time_Get(&gGame->time)) {
            ctrl->x6D = 0;
            ctrl->x10 = 0;
        } else {
            __cmpdi2(ctrl->x70 - Time_Get(&gGame->time), 0);
        }
        break;
    case 6:
        break;
    }
}
