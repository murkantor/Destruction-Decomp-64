#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x19C];
    f32 x19C;
    u8 pad1A0[0x1C];
} Wheel;
typedef struct {
    u8 pad[0x178];
    Wheel wheels[4];
} Body;
typedef struct {
    u8 pad[0x1EC];
    Body body;
} CarW;
extern f32 D_800D2E78;
void func_8002FAC8(CarW *, Wheel *);

void Car_UpdateWheels(CarW *car) {
    Body *b = &car->body;
    s32 i;

    for (i = 0; i < 4; i++) {
        if (b->wheels[i].x19C < 1.0f) {
            b->wheels[i].x19C = 1.0f;
        } else {
            b->wheels[i].x19C += D_800D2E78;
        }
        func_8002FAC8(car, &b->wheels[i]);
    }
}
