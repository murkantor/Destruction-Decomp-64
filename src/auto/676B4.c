#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x20];
    f32 x20;
    f32 x24;
    f32 x28;
    u8 pad2C[0x1C];
    f32 x48;
    f32 x4C;
    u8 pad50[0x52];
    u16 xA2;
    u8 padA4[0xC];
    u8 xB0;
    s8 xB1;
    u8 xB2;
    u8 xB3;
} Voice;
extern f64 D_800BBFA8;
extern f64 D_800BBFB0;
extern u8 *D_800CCA9C;
extern u8 D_8011F2B8[];
f32 Math_Exp2f(f32);
void alSynSetPitch(void *, void *, f32);

void AudioVoice_UpdatePitch(Voice *v, s32 idx, f32 f) {
    u8 n = v->xB0;
    f32 cur = v->x28;

    if (n != 0) {
        if (n >= v->xA2) {
            cur = v->x48 + (cur - v->x48) / n * v->xA2;
        }
        v->x4C = cur;
    }
    cur += f + v->x20 + v->xB1 * (1 - v->xB2);
    v->xB2 = 0;
    if (cur != v->x24) {
        v->x24 = cur;
        cur = Math_Exp2f(cur * D_800BBFA8);
        if (cur > D_800BBFB0) {
            cur = 2.0f;
            v->xB3 = 0;
        }
        alSynSetPitch(D_8011F2B8, D_800CCA9C + idx * 0x1C, cur);
    }
}
