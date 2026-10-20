#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    s8 x10;
} Sub41CC4;

typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x14];
    Sub41CC4 *x28;
} Obj41CC4;

extern u8 D_80097344[];
extern s32 D_800958A4[];
extern s32 D_80097350;
extern s32 D_800958AC;

void Timer_Start(void *, s64);
void CarSelect_SetCar(s32, s32, s32);

s32 CarSelect_Next(Obj41CC4 *obj) {
    s32 p;
    s32 next;

    Timer_Start(obj, 300000000);
    p = obj->x28->x10;
    if (D_80097344[p]) {
        return 0;
    }
    if (p >= D_800958A4[0]) {
        return 0;
    }
    next = (obj->x10 + 1) % D_80097350;
    CarSelect_SetCar(next, obj->x10, p);
    if (D_800958A4[0] >= 2 && D_800958AC == 2 && D_800958A4[0] == 4) {
        switch (p) {
            case 0:
                CarSelect_SetCar(next, obj->x10, 1);
                break;
            case 1:
                CarSelect_SetCar(next, obj->x10, 0);
                break;
            case 2:
                CarSelect_SetCar(next, obj->x10, 3);
                break;
            case 3:
                CarSelect_SetCar(next, obj->x10, 2);
                break;
        }
    }
    obj->x10 = next;
    return 1;
}
