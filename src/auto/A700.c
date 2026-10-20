#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xA8C]; s32 xA8C; u8 padA90[0xAB4 - 0xA90]; s32 xAB4; u8 padAB8[0xAC8 - 0xAB8]; s32 xAC8; u8 padACC[0xAF0 - 0xACC]; s32 xAF0; } Obj;
void Car_DamagePart(Obj *, u8);

void Car_SetPartsForMode(Obj *o, u32 mode) {
    u8 i;
    switch (mode) {
    case 0:
        Car_DamagePart(o, 2);
        Car_DamagePart(o, 3);
        Car_DamagePart(o, 0);
        break;
    case 1:
        Car_DamagePart(o, 2);
        Car_DamagePart(o, 6);
        if (o->xAC8) Car_DamagePart(o, 0);
        break;
    case 2:
        Car_DamagePart(o, 8);
        break;
    case 3:
        Car_DamagePart(o, 10);
        Car_DamagePart(o, 4);
        if (o->xAF0) Car_DamagePart(o, 1);
        break;
    case 4:
        Car_DamagePart(o, 3);
        Car_DamagePart(o, 7);
        if (o->xA8C) Car_DamagePart(o, 0);
        break;
    case 5:
        Car_DamagePart(o, 9);
        break;
    case 6:
        Car_DamagePart(o, 11);
        Car_DamagePart(o, 5);
        if (o->xAB4) Car_DamagePart(o, 1);
        break;
    case 7:
        Car_DamagePart(o, 4);
        Car_DamagePart(o, 5);
        Car_DamagePart(o, 1);
        break;
    case 8:
    case 9:
        break;
    case 10:
        for (i = 0; i < 12; i++) {
            Car_DamagePart(o, i);
        }
        break;
    }
}
