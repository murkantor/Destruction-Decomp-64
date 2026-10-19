#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[0x10]; f32 x10; } ScaleInfo;
typedef struct { u8 pad[0xC]; ScaleInfo *xC; } TrackInfo;
typedef struct { s32 x0; u8 pad4[0x70 - 4]; TrackInfo *x70; } Race;
typedef struct {
    u8 pad0[0xC];
    s16 xC;
    u8 padE[0x1074 - 0xE];
    u8 x1074;
    u8 pad1075[0x1100 - 0x1075];
    Race *x1100;
} Game;
typedef struct { u8 pad[0x10]; s8 x10; } Player;
typedef struct {
    u8 pad0[0xC];
    u8 xC;
    u8 xD;
    u8 padE[0x30 - 0xE];
    s32 x30;
    u8 pad34[0x3C - 0x34];
    Player *x3C;
    u8 pad40[0x98 - 0x40];
    u8 x98[0x2B4 - 0x98];
    Vec3f x2B4;
    Vec3f x2C0;
    u8 pad2CC[0xB6C - 0x2CC];
    void *xB6C;
    void *xB70[4];
    void *xB80;
} Car;
typedef struct { u8 pad[0xB4]; u8 xB4; } Cam;
typedef struct { void *x0; } LocalInfo;
typedef struct { u8 pad[0xC]; void *xC; } Res;

extern s32 gDrawCars;
extern Game *gGame;
extern LocalInfo *D_80096074;
extern u8 D_8012455C;
extern u8 *D_801245A4;
extern void *D_8012556C[];
extern Res *D_80125570;
void func_8000B6EC(Car *);
Gfx *Model_Draw(void *, Gfx *);
Gfx *Car_DrawShadowDL(Car *, Gfx *);
void Frame_ToMatrix(f32 m[4][4], Car *, s32);
void Mtx_FromRotPos(f32 out[4][4], f32 m[4][4], Car *, s32, s32);
void func_80018270(void *, f32 m[4][4], f32 out[4][4]);
void func_8000AD40(Car *, f32 a[4][4], f32 b[4][4]);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
void Particles_EmitTrail(void *, Vec3f, Car *);
void Light_UpdateVertexColors(void *, void *, void *, f32 m[4][4], s32);
void func_8000A9D8(Car *, Vec3f, f32 m[4][4]);
Gfx *func_8000A894(Car *, Gfx *);
Gfx *Debris_UpdateDraw(Car *, Gfx *);
Gfx *Gfx_SetViewport2(Gfx *, s32);
Gfx *func_80015624(void *, Gfx *);
void Shadow_Build(void *, f32 m[4][4]);
void Particle_DrawAt(void *);
Gfx *func_80017E8C(void *, Gfx *);
void func_800187D4(void *);
void Obj_EmitPositionedDL(void *);

static inline void UpdateWheels(Car *car, f32 m[4][4])
{
    f32 tmp[4][4];
    if (car->xB70[0]) {
        Mtx_FromRotPos(tmp, m, car, 0, 1);
        func_80018270(car->xB70[0], m, tmp);
        Mtx_FromRotPos(tmp, m, car, 1, 1);
        func_80018270(car->xB70[1], m, tmp);
        Mtx_FromRotPos(tmp, m, car, 2, 1);
        func_80018270(car->xB70[2], m, tmp);
        Mtx_FromRotPos(tmp, m, car, 3, 1);
        func_80018270(car->xB70[3], m, tmp);
    }
}

static inline void EmitTrail(Car *car)
{
    Vec3f k, a, b, c;
    k.x = -1.42f;
    k.y = 1.64f;
    k.z = 0.0f;
    Vec3f_Scale(&a, &car->x2C0, k.y);
    Vec3f_Scale(&b, &car->x2B4, k.x);
    Vec3f_Add(&c, &a, &b);
    Particles_EmitTrail(D_801245A4 + 0xC, c, car);
}

static inline Gfx *DrawParticle(Car *car, Gfx *gfx, f32 m[4][4])
{
    Gfx *g = gfx;
    if (car->xB6C) {
        g = func_80015624(car->xB6C, gfx);
        if (!D_8012455C) {
            Shadow_Build(car->xB6C, m);
        }
        Particle_DrawAt(car->xB6C);
    }
    return g;
}

Gfx *func_8000B1D0(Car *car, Gfx *gfx, s32 unused, Cam *cam)
{
    f32 m18[4][4];
    f32 m58[4][4];
    Vec3f pos;
    u8 isLocal;
    Gfx *g;

    if (car->xC && gDrawCars) {
        if (gGame->xC != 2) {
            func_8000B6EC(car);
            gfx = Model_Draw(car->x98, gfx);
            gfx = Car_DrawShadowDL(car, gfx);
            return gfx;
        }
        isLocal = 0;
        if (D_80096074 != NULL && D_80096074->x0 == car && gGame->x1100->x0 >= 2) {
            isLocal = !car->xD;
        }
        if (!D_8012455C) {
            func_8000B6EC(car);
        }
        Frame_ToMatrix(m58, car, 0);
        Frame_ToMatrix(m18, car, 1);
        pos.x = m18[3][0] * gGame->x1100->x70->xC->x10;
        pos.y = m18[3][1] * gGame->x1100->x70->xC->x10;
        pos.z = m18[3][2] * gGame->x1100->x70->xC->x10;
        if (!gGame->x1074 && !D_8012455C) {
            if (!car->xD) {
                UpdateWheels(car, m18);
                func_8000AD40(car, m58, m18);
                if (isLocal) {
                    EmitTrail(car);
                }
                if (car->xB80) {
                    Light_UpdateVertexColors(car->xB80, D_8012556C[0], D_80125570->xC, m18, !car->xD);
                }
            } else {
                func_8000A9D8(car, pos, m18);
            }
        }
    if (car->x30 != 0 || cam->xB4 || D_8012455C != car->x3C->x10) {
            gfx = Model_Draw(car->x98, gfx);
            if (!car->xD) {
                gfx = func_8000A894(car, Car_DrawShadowDL(car, gfx));
            }
            gfx = Gfx_SetViewport2(Debris_UpdateDraw(car, gfx), -2);
            gfx = DrawParticle(car, gfx, m18);
            if (!car->xD && car->xB70[0]) {
                g = func_80017E8C(car->xB70[0], gfx);
                g = func_80017E8C(car->xB70[1], g);
                g = func_80017E8C(car->xB70[2], g);
                g = func_80017E8C(car->xB70[3], g);
                if (!D_8012455C) {
                    func_800187D4(car->xB70[0]);
                    func_800187D4(car->xB70[1]);
                    func_800187D4(car->xB70[2]);
                    func_800187D4(car->xB70[3]);
                }
                gfx = g;
                Obj_EmitPositionedDL(car->xB70[0]);
                Obj_EmitPositionedDL(car->xB70[1]);
                Obj_EmitPositionedDL(car->xB70[2]);
                Obj_EmitPositionedDL(car->xB70[3]);
            }
            gfx = Gfx_SetViewport2(gfx, 0);
        }
    }
    return gfx;
}
