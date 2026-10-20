#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad0[0x90]; s32 x90; } Obj;
extern s8 D_800D31CC;
extern Vec3f D_800B80F0; /* {0, 6, 0} */
extern Vec3f D_800B80FC; /* {0, 2.5, 0} */
extern f64 D_800B8108; /* 3.0 */
extern f64 D_800B8110; /* 1.0 */
extern u8 *D_801245A4;
s32 rand(void);
void func_800172D0(void *, Vec3f, Vec3f, f32, f32, s32);
void func_80016F48(void *, Vec3f, Vec3f, f32, f32, s32);

void Car_EmitWreckParticles(Obj *arg0, Vec3f pos, f32 *m) {
    s32 t = 0x13 - D_800D31CC * 3;
    Vec3f colA = D_800B80F0;
    Vec3f colB = D_800B80FC;
    Vec3f p;
    s32 n;
    s32 r1;
    f32 f1;
    f32 f2;

    n = arg0->x90;
    if (n < 0x8C) {
        arg0->x90 = n + 1;
    }
    r1 = rand() % 24 - 12;
    {
        s32 r2 = rand() % 10 - 5;
        p.x = pos.x - m[0] * r1 - m[8] * r2;
        p.y = pos.y - m[1] * r1 - m[9] * r2 + D_800B8108;
        p.z = pos.z - m[2] * r1 - m[10] * r2;
    }
    if (n < 20) {
        func_800172D0(D_801245A4 + 0x148, p, colA, 34.0f, 1.2f, t);
    } else if (n < 110) {
        func_800172D0(D_801245A4 + 0x148, p, colB, (110 - n) / 3 + 4, 0.5f, t);
    } else {
        r1 = rand() % 90;
        if (n - 110 < r1) {
        func_800172D0(D_801245A4 + 0x148, p, colB, 4.0f, 0.5f, t);
    } else {
        p.y += D_800B8110;
        func_80016F48(D_801245A4 + 0xB0, p, colB, 4.0f, 0.5f, t);
    }
    }
}
