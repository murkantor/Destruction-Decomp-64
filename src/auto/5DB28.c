#include "common.h"
#include "m2c.h"

f32 Vec3f_DistSq(s32, void *);

s32 Snd_InRange(s32 arg0, u8 *obj) {
    f32 d = Vec3f_DistSq(arg0, obj + 0x18);
    f32 r = *(f32 *)(obj + 0x30);

    if (d <= r * r) {
        return 1;
    }
    return 0;
}
