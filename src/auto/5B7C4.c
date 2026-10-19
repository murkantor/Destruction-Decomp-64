#include "common.h"
#include "m2c.h"

typedef f32 MtxF[4][4];
typedef struct {
    u8 pad[4];
    void *x4;
    u8 pad8[4];
    f32 xC;
    f32 x10;
    f32 x14;
    f32 x18;
    f32 x1C;
    f32 x20;
    f32 x24;
} Obj;
extern u8 gParticlePool[];
void func_80057334(void *);
void *Pool_Alloc(void *);
void guMtxIdentF(MtxF);
void func_80069AB0(MtxF, f32, f32, f32);
void Mtx4_Mul(MtxF, MtxF, MtxF);
void guRotateF(MtxF, f32, f32, f32, f32);
void guRotateRPYF(MtxF, f32, f32, f32);
void func_80069D90(MtxF, f32, f32, f32);
void guMtxF2L(MtxF, void *);
void osWritebackDCache(void *, s32);

void Particle_BuildMatrix(Obj *o) {
    MtxF m;
    MtxF t;

    func_80057334(o->x4);
    o->x4 = Pool_Alloc(gParticlePool);
    guMtxIdentF(m);
    func_80069AB0(t, o->x24, o->x24, o->x24);
    Mtx4_Mul(m, t, m);
    guRotateF(t, o->x1C * 57.29577957855229, 0.0f, 1.0f, 0.0f);
    Mtx4_Mul(m, t, m);
    guRotateRPYF(t, o->x18 * 57.29577957855229, 0.0f, o->x20 * 57.29577957855229);
    Mtx4_Mul(m, t, m);
    func_80069D90(t, o->xC, o->x10, o->x14);
    Mtx4_Mul(m, t, m);
    guMtxF2L(m, o->x4);
    osWritebackDCache(o->x4, 4);
}
