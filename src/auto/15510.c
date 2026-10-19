#include "common.h"
#include "m2c.h"

void Angle_StepWrap(u8 *obj) {
    u8 *p = *(u8 **)(obj + 0x38);
    *(f32 *)(p + 0x10) += 0.01;
    if (*(f32 *)(p + 0x10) > 6.2831853) {
        *(f32 *)(p + 0x10) = 0.0f;
    }
}
