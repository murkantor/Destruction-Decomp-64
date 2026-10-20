#include "common.h"
#include "m2c.h"

extern s32 D_800CCABC;

s32 Audio_RandRange(s32 arg0) {
    s32 temp_a1;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_a2;

    var_a2 = 0;
    do {
        temp_a1 = D_800CCABC * 2;
        temp_v0 = D_800CCABC & 0x48000000;
        temp_v1 = (temp_v0 == 0x48000000) | (temp_v0 == 0x08000000);
        D_800CCABC = temp_a1;
        var_a2 += 1;
        if (temp_v1 != 0) {
            D_800CCABC = temp_a1 | 1;
        }
    } while (var_a2 < 8);
    return (s32) ((f32) arg0 * (((f32) D_800CCABC / 65536.0f) / 65536.0f));
}
