#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s16 x10; } Obj;

void Sound_SetScrapeId(Obj *o, s32 n) {


    switch (n) {
        case 1:
            o->x10 = 0x13;
            break;
        case 2:
            o->x10 = 0x14;
            break;
        case 4:
            o->x10 = 0x16;
            break;
        case 5:
            o->x10 = 0x17;
            break;
        case 6:
            o->x10 = 0xB;
            break;
        case 7:
            o->x10 = 0x17;
            break;
        case 8:
            o->x10 = 8;
            break;
        case 9:
            o->x10 = 9;
            break;
        case 3:
        case 10:
        default:
            o->x10 = 0x15;
            break;
    }

}
