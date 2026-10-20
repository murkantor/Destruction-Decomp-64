#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10];
    s8 x10;
} CarInfo;

typedef struct {
    u8 pad0[0x18];
    s32 x18;
    u8 pad1C[0x30 - 0x1C];
    s32 x30;
    s32 x34;
    CarInfo *x3C_pad[1];
} CarHead;

typedef struct Car {
    u8 pad0[0x18];
    s32 x18;
    u8 pad1C[0x30 - 0x1C];
    s32 x30;
    s32 x34;
    u8 pad38[0x3C - 0x38];
    CarInfo *x3C;
} Car;

typedef struct {
    Car *x0;
    Car *x4;
    u8 pad8[0x10 - 0x8];
    u64 x10;
    s8 x18;
    u8 x19;
    u8 x1A;
    s32 x1C;
} TagState;

typedef struct {
    s32 x0;
    u8 pad4[0x14 - 0x4];
    u8 x14[1];
} Race;

typedef struct {
    u8 pad0[0x10A8];
    u64 x10A8;
    u8 pad10B0[0x10FC - 0x10B0];
    s16 x10FC;
    u8 pad10FE[0x1100 - 0x10FE];
    Race *x1100;
} Game;

extern Game *gGame[];
extern TagState *D_80096074;
extern f64 D_800B8D58;
extern f64 D_800B8D60;
extern f64 D_800B8D68;
extern f64 D_800B8D70;

Car *Car_PickRandomOther(Car *);
void Standings_Resort(void *, Car *);
void Msg_Queue(s32, s32, s32, s32);
void func_800638C8(void *);
void func_80063890(void *);
void func_800638C0(void *);

void Tag_PassTo(Car *car) {
    s32 n = 10;
    u64 times[6] = { 22000000000ULL, 20000000000ULL, 18000000000ULL, 16000000000ULL, 14000000000ULL, 12000000000ULL };
    TagState *tag = D_80096074;
    Race *race = gGame[0]->x1100;

    if (car == tag->x0) {
        if (race->x0 < 3) {
            tag->x0 = NULL;
            tag->x10 = times[0];
        } else {
            tag->x0 = Car_PickRandomOther(car);
            if (gGame[0]->x10FC == 0x1C) {
                tag->x10 = times[race->x0 - 3];
            }
        }
        tag->x4 = car;
    } else {
        u64 t = gGame[0]->x10A8;
        tag->x0->x18 += (s32)(t * D_800B8D58) * 3;
        Standings_Resort(race->x14, tag->x0);
        tag->x4 = D_80096074->x0;
        tag->x0 = car;
        t = tag->x10 - gGame[0]->x10A8;
        if ((s32)(t * D_800B8D60) < 6) {
            Msg_Queue(0x6E, 0, 1, 0);
        }
        n -= tag->x1C;
        if (n <= 0) {
            n = 1;
        }
        t += (u64)(n * D_800B8D68);
        tag->x10 = t;
        if (t > D_800B8D70) {
            tag->x10 = 45000000000ULL;
        }
        tag->x1C++;
    }
    tag->x19 = 0;
    tag->x1A = 0;
    func_800638C8(&gGame[0]->x10A8);
    func_80063890(&gGame[0]->x10A8);
    func_800638C0(&gGame[0]->x10A8);
    if (gGame[0]->x10FC == 0x1C && tag->x0 != NULL && (s8)tag->x0->x34 != tag->x18) {
        tag->x18 = tag->x0->x34;
        if (tag->x0->x30 == 0) {
            s32 msgs[4] = { 0x6A, 0x6B, 0x6C, 0x6D };

            Msg_Queue(msgs[tag->x0->x3C->x10], 0, 1, 0);
        }
    }
}
