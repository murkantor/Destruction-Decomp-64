#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xC]; s32 xC; } Obj;
extern u8 D_800D2E58;
extern u8 D_800958A1;
extern u8 D_801304D8[];
void func_800123C0(void);
void func_800627F0(Obj *, s32, u8);
f32 Track_GetScale(void);
void guPosition(void *m, f32 r, f32 p, f32 h, f32 s, f32 x, f32 y, f32 z);

void func_8005FA38(Obj *o) {
    D_800D2E58 = 1;
    func_800123C0();
    func_800627F0(o, 1, D_800958A1);
    o->xC = 0;
    guPosition(D_801304D8, 0.0f, 0.0f, 0.0f, 1.0 / Track_GetScale(), 0.0f, 0.0f, 0.0f);
}
