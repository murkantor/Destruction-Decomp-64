#include "common.h"
#include "m2c.h"

void *func_800359DC();                              /* extern */
extern f64 D_800B93F8;
extern f64 D_800B9400;
extern f32 D_800D2E78;
extern void *gGame;

void func_800300D8(void *arg0) {
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f8;
    void *temp_v0;
    void *temp_v1;

    temp_v0 = func_800359DC();
    if (temp_v0 != NULL) {
        temp_f12 = M2C_FIELD(temp_v0, f32 *, 0xC);
        if (D_800B93F8 < fabs((f64) temp_f12)) {
            temp_f10 = M2C_FIELD(temp_v0, f32 *, 8);
            temp_v1 = M2C_FIELD(M2C_FIELD(M2C_FIELD(M2C_FIELD(gGame, void **, 0x1100), void **, 0x70), void **, 0xC), s32 *, 0x60) + (M2C_FIELD(temp_v0, u16 *, 0) * 0x10);
            temp_f8 = M2C_FIELD(temp_v0, f32 *, 0x10);
            if ((M2C_FIELD(arg0, f32 *, 0x294) - (((((temp_f10 * M2C_FIELD(temp_v1, f32 *, 0)) + (temp_f12 * M2C_FIELD(temp_v1, f32 *, 4)) + (temp_f8 * M2C_FIELD(temp_v1, f32 *, 8))) - (M2C_FIELD(arg0, f32 *, 0x290) * temp_f10)) - (M2C_FIELD(arg0, f32 *, 0x298) * temp_f8)) / temp_f12)) > 2.0f) {
                M2C_FIELD(arg0, f32 *, 0x2A0) = (f32) ((f64) M2C_FIELD(arg0, f32 *, 0x2A0) - ((f64) D_800D2E78 * D_800B9400));
            }
        }
    }
}
