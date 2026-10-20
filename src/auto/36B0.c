#include "common.h"
#include "m2c.h"

typedef struct { f32 x0, x4, x8, xC; u8 x10; } Target;
typedef struct Car Car;
typedef struct {
    u8 pad[0x18];
    Car *x18;
    s32 x1C;
    u8 pad20[0x10];
    s32 x30;
    f32 x34;
    u8 pad38[8];
    s32 x40;
    s32 x44;
    u8 pad48[0xC];
    s32 x54;
    s32 x58;
} Ai;
struct Car { u8 pad[0xD]; u8 xD; u8 padE[2]; s32 x10; u8 pad14[0x24]; Ai *x38; };
typedef struct { u8 pad[0x74]; Car *x74[RACE_CARS]; s32 xA4; } Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;
extern Game *gGame;
extern void Ai_ComputeIntercept(Car *, Car *, Target *);
extern Car *Car_PickRandomOther(Car *);

void Ai_PickTarget2(Car *car, Target *out) {
    Ai *ai = car->x38;
    Target tmp;
    s32 i;
    Car *o;

    if (!ai->x18->xD) {
        if (ai->x1C >= 10 || ai->x54 != 0) {
            Ai_ComputeIntercept(car, ai->x18, out);
            if (ai->x54) {
                ai->x54--;
            }
            return;
        }
    } else {
        ai->x18 = Car_PickRandomOther(car);
    }
    if (ai->x54 == 0) {
        ai->x54 = ai->x58;
    }
    for (i = 0; i < gGame->x1100->xA4; i++) {
        o = gGame->x1100->x74[i];
        if (o->xD || o == car) {
            continue;
        }
        if (ai->x18 == o) {
            Ai_ComputeIntercept(car, o, out);
            if (ai->x18->xD) {
                out->x0 = 500.0f;
            }
            continue;
        }
        Ai_ComputeIntercept(car, o, &tmp);
        if (ai->x40 == 1 && tmp.x0 < ai->x34 && tmp.x0 < out->x0) {
            ai->x18 = o;
            ai->x30 = 0;
            ai->x1C++;
            out->x0 = tmp.x0;
            out->x4 = tmp.x4;
            out->x8 = tmp.x8;
            out->xC = tmp.xC;
            out->x10 = tmp.x10;
            continue;
        }
        if (ai->x44 == 1 && tmp.x0 < ai->x34 && tmp.x0 < out->x0 && o->x10 < 20) {
            ai->x18 = o;
            ai->x1C++;
            out->x0 = tmp.x0;
            out->x4 = tmp.x4;
            out->x8 = tmp.x8;
            out->xC = tmp.xC;
        }
    }
}
