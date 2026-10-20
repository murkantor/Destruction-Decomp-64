#include "common.h"
#include "m2c.h"

typedef struct {
    u8 padB4[0xB4];
    u8 xB4;
} Unk3CC;

typedef struct {
    u8 pad0[0xC];
    Unk3CC *xC;
} Unk3C;

typedef struct {
    u8 pad0[0x18];
    s32 x18;
    u8 pad1C[0x14];
    s32 x30;
    u8 pad34[8];
    Unk3C *x3C;
    u8 pad40[0x28];
    u64 x68;
    u64 x70;
    u64 x78;
    u8 pad80[8];
    s32 x88;
} Car;

typedef struct {
    u8 pad0[0xC];
    s32 xC;
    u8 pad10[4];
    u8 x14[1];
} Race;

typedef struct {
    u8 pad0[0x1100];
    Race *race;
} Game;

typedef struct {
    u64 best;
    u8 pad8[0x30];
} TrackRecord;

typedef struct {
    u64 time;
    u8 pad8[0x10];
} TrackTime;

extern Game *gGame;
extern s32 D_800958B4;
extern s32 D_800958A4;
extern s32 D_80095FF8[];
extern TrackRecord D_80097A58[];
extern TrackTime D_80130528[];
extern u8 D_80097A18[];
extern u8 D_80097335[];
extern u8 D_80097337[];
extern s32 D_80097338;
extern s32 D_80097D48;
extern f64 D_800B91F0;

void Standings_Resort(void *, Car *);
void Msg_Queue(s32, s32, s32, s32);

void Race_OnLapCompleted(Car *car) {
    Race *race;
    s32 track;
    u64 t;

    race = gGame->race;
    car->x18 += 10;
    Standings_Resort(race->x14, car);
    if (car->x30 == 0) {
        race->xC++;
    }
    if (car->x88 == D_80095FF8[D_800958B4] && car->x30 == 0) {
        track = D_800958B4;
        if (D_800958A4 == 1) {
            t = car->x68 + car->x70 + car->x78;
            if (t < D_80097A58[track].best) {
                s32 map[12] = { -1, -1, -1, -1, 1, 5, 2, 9, 0, 7, 6, 11 };
                D_80130528[track].time = t;
                D_80097335[0] = 1;
                D_80097D48 = 0;
                if (D_80097A18[map[track]] == 0) {
                    D_80097A18[map[track]] = 1;
                    D_80097337[0] = 1;
                    D_80097338 = map[D_800958B4];
                }
            } else if ((f64)t - D_800B91F0 < (f64)D_80097A58[track].best) {
                Msg_Queue(0xE2, 0, 1, 0);
            }
        }
        if (car->x30 == 0) {
            car->x30 = 1;
            car->x3C->xC->xB4 = 7;
        }
    }
}
