#include "common.h"
#include "m2c.h"

typedef struct { u32 x0; s32 x4; u64 x8; s32 x10; s32 x14; s32 x18; s32 x1C; } Credits;
extern u8 *gGame;
extern Credits D_80124560;
extern const f64 D_800BB4A0;
void func_8003ECF8(void);
void Ui_ResetScreen(s32, s32);
u64 Time_Get(void *);
s32 Res_Get(s32);
u64 __fixunsdfdi(f64);

void Credits_Init(void) {
    Credits *c;
    u64 *p;
    u64 t;
    func_8003ECF8();
    Ui_ResetScreen(0x112C2, 0x112C3);
    c = &D_80124560;
    c->x0 = 0;
    t = Time_Get(gGame + 0x1058);
    p = &c->x8;
    *p = __fixunsdfdi((f64)t + D_800BB4A0);
    D_80124560.x10 = 0;
    D_80124560.x1C = Res_Get(0x11426);
}
