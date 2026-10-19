#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xB4]; u8 xB4; } Model;
typedef struct { u8 pad[0xC]; Model *xC; } CarRes;
typedef struct Car {
    u8 pad[0x1C]; s32 x1C; u8 pad20[0x30 - 0x20]; s32 x30; u8 pad34[0x3C - 0x34]; CarRes *x3C;
    u8 pad40[0x50 - 0x40]; u64 x50; s32 x58; s32 x5C; u8 pad60[0x8C - 0x60]; s32 x8C;
} Car;
typedef struct { struct Car *x0; } Slot;
typedef struct { s32 x0; u8 pad4[4]; s32 x8; u8 padC[0x14 - 0xC]; Slot *x14; } Race;
typedef struct { u8 pad[0x10A8]; u64 x10A8; u8 pad10B0[0x1100 - 0x10B0]; Race *x1100; } Game;
extern Game *gGame;
extern u8 D_80124597[];
extern u8 D_80124598[];
extern u32 D_800958B0;
extern s32 D_800958B4;
extern const f64 D_800B8A30;
extern const f64 D_800B8A38;
extern const f64 D_800B8A40;
extern const f64 D_800B8A48;
extern const f64 D_800B8A50;
u64 Time_Get(void *);
void Msg_Queue(s32, s32, s32, s32);
void func_80023CE0(Model *);

void func_8001F000(Car *car, u8 flag, s32 target) {
    s32 adj;
    s32 d;
    u64 now;
    Race *race;

    if (car->x30 == 0) {
        adj = 0;
        D_80124597[0] = 0;
        D_80124598[0] = 0;
        if (car->x3C->xC->xB4 == 7) {
            return;
        }
    } else {
        adj = 0;
    }
    switch (D_800958B0) {
    case 0:
        adj = 0;
        break;
    case 1:
        adj = 2;
        break;
    case 2:
        adj = 4;
        break;
    case 3:
        adj = 6;
        break;
    }
    if (car->x30 == 1) {
        adj = -10;
        if (D_800958B4 == 9) {
            adj = -20;
        } else if (D_800958B4 == 8) {
            adj = -20;
        }
    }
    if (car->x5C < 6) {
        d = target - adj - car->x5C * 2;
    } else {
        d = target - adj - car->x5C * 3;
    }
    if (d < 0) {
        d = 1;
    }
    if (car->x58 == 1) {
        if (flag) {
            car->x8C++;
            if (car->x5C < car->x8C) {
                now = Time_Get(&gGame->x10A8);
                if (now >= car->x50) {
                    car->x50 = (f64)now + d * D_800B8A30;
                } else {
                    car->x50 = (f64)car->x50 + d * D_800B8A30;
                }
                car->x5C++;
                car->x1C += 0x14;
                if (car->x30 == 0) {
                    Msg_Queue(0xED, 0, 1, 0);
                    if ((f64)car->x50 > (f64)now + D_800B8A38) {
                        car->x50 = (f64)now + D_800B8A50;
                    }
                }
            }
        } else {
            car->x8C--;
        }
    } else {
        if (!flag) {
            car->x8C++;
            if (car->x5C < car->x8C) {
                now = Time_Get(&gGame->x10A8);
                if (now >= car->x50) {
                    car->x50 = (f64)now + d * D_800B8A40;
                } else {
                    car->x50 = (f64)car->x50 + d * D_800B8A40;
                }
                car->x5C++;
                car->x1C += 0x14;
                if (car->x30 == 0) {
                    Msg_Queue(0xED, 0, 1, 0);
                    if ((f64)car->x50 > (f64)now + D_800B8A48) {
                        car->x50 = (f64)now + D_800B8A50;
                    }
                }
            }
        } else {
            car->x8C--;
        }
    }
    race = gGame->x1100;
    if (race->x0 == 1 && race->x14->x0 == car && car->x30 == 0) {
        race->x8 = 0;
        car->x30 = 1;
        func_80023CE0(car->x3C->xC);
    }
}
