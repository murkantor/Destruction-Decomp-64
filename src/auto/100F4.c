#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0x1058]; u8 x1058[8]; } Game;
extern Game *gGame;
extern void *gUi;
typedef struct { u64 time; s32 alpha; } Fade;
extern Fade D_8007EF20;
extern s32 D_8007EF28;
extern s8 D_8007EF2C;
extern u8 D_8007EF2D[];
u64 Time_Get(void *);
void Ui_EnableElementAlpha(void *, s32, s32, s32);
void Ui_SetElementAlphaValue(void *, s32, s32, s32);

s32 Ui_UpdateFadeBlink(s32 *arg0) {
    u64 t;

    t = Time_Get(gGame->x1058);
    if (!D_8007EF2D[0]) {
        Ui_EnableElementAlpha(gUi, 2, *arg0, 0xFF);
        D_8007EF28 = 0xFF;
        D_8007EF2C = 1;
        D_8007EF2D[0] = 1;
    }
    if (t < D_8007EF20.time) {
        goto end;
    }
    if (D_8007EF2C == 0) {
        D_8007EF28 += 0x30;
        if (D_8007EF28 >= 0x100) {
            D_8007EF28 = 0xFF;
            D_8007EF2C = 1;
            t = (f64)t + 156000000.0;
        }
    } else {
        D_8007EF28 -= 0x30;
        if (D_8007EF28 < 0) {
            D_8007EF28 = 0;
            D_8007EF2C = 0;
        }
    }
    Ui_SetElementAlphaValue(gUi, 2, *arg0, D_8007EF20.alpha);
    D_8007EF20.time = (f64)t + 52000000.0;
end:
    return 0;
}
