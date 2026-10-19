#include "common.h"
#include "m2c.h"

extern s32 D_801311B8;
extern s32 D_80125B70[24];
extern u8 D_80124478[];
void func_80069B10(void *, f32, f32, f32);

void func_8000A39C(void) {
    s32 i;

    D_801311B8 = 0;
    for (i = 23; i >= 0; i--) {
        D_80125B70[i] = 0;
    }
    func_80069B10(D_80124478, 1.0f, 0.5f, 0.5f);
}
