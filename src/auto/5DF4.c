#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 x0, x4, x8, xC; u8 x10; } Rel;
typedef struct Car Car;
typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[4];
    Car *x18;
    u8 pad1C[0x5C - 0x1C];
    Vec3f x5C;
    f32 x68;
    u8 x6C;
} Ai;
struct Car { u8 pad[0x38]; Ai *x38; };
typedef struct {
    s32 x0;
    s32 x4;
    Car *x8;
    s32 xC;
    Vec3f x10;
    u8 pad1C[0x3C - 0x1C];
    Car *x3C;
    Car *x40;
    Car *x44;
} Slot;

extern Slot *D_80096078;
extern Slot *D_8009607C;
extern Slot *D_80096080;
extern Slot *D_80096084;
extern s32 D_800958A4;
u8 func_800267B4(void *, Car *);
Vec3f func_8001FBA8(void *);
Vec3f func_8001FB88(void *);
Vec3f func_8001FC08(Car *);
f32 Vec3f_DistXZ(Vec3f, Vec3f);
f32 func_800005C8(Vec3f, Vec3f);
void func_80000AC8(Car *, Car *, Rel *);
void Ai_ConstrainTarget(Car *);
void Ai_Steer(Car *);

static inline void Ai_ApplyRel(Car *car, Rel r) {
    Ai *ai = car->x38;
    Vec3f pos;
    Vec3f fwd;
    Vec3f d;

    if (r.xC >= 3.0) {
        r.xC = 2.0f;
    }
    pos = func_8001FBA8(ai->x18);
    fwd = func_8001FB88(ai->x18);
    ai->x5C.x = pos.x + fwd.x * r.xC;
    ai->x5C.y = pos.y + fwd.y * r.xC;
    ai->x5C.z = pos.z + fwd.z * r.xC;
    if (r.x10) {
        ai->x6C = 1;
    }
    fwd = func_8001FC08(car);
    pos = func_8001FBA8(car);
    d.x = ai->x5C.x - pos.x;
    d.y = ai->x5C.y - pos.y;
    d.z = ai->x5C.z - pos.z;
    ai->x68 = func_800005C8(fwd, d);
}

void Ai_SelectTeamTarget(Car *car) {
    Rel tmp;
    Ai *o;
    Slot *p;
    Slot *q;
    Slot *r;
    Slot *s;
    f32 d1;
    f32 d2;
    f32 d3;

    o = car->x38;
    d3 = 0.0f;
    if (func_800267B4(D_80096080, car)) {
        p = D_8009607C;
        q = D_80096078;
        r = D_80096080;
        s = D_80096084;
    } else {
        p = D_80096078;
        q = D_8009607C;
        r = D_80096084;
        s = D_80096080;
    }
    switch (o->x10) {
    case 14:
        if (p->x4 == 0) {
            o->x5C = p->x10;
        } else if (p->x8 == car) {
            o->x5C = *(Vec3f *)&r->x8;
        } else {
            d1 = Vec3f_DistXZ(func_8001FBA8(p->x8), func_8001FBA8(s->x3C));
            d2 = Vec3f_DistXZ(func_8001FBA8(p->x8), func_8001FBA8(s->x40));
            if (D_800958A4 == 2) {
                d3 = Vec3f_DistXZ(func_8001FBA8(p->x8), func_8001FBA8(s->x44));
            }
            if (d1 < d2) {
                d1 = d2;
                if (d1 < d3) {
                    o->x18 = s->x44;
                } else {
                    o->x18 = s->x40;
                }
            } else if (d1 < d3) {
                o->x18 = s->x44;
            } else {
                o->x18 = s->x3C;
            }
            func_80000AC8(car, o->x18, &tmp);
            Ai_ApplyRel(car, tmp);
        }
        break;
    case 13:
        if (q->x4 == 0) {
            d1 = Vec3f_DistXZ(q->x10, func_8001FBA8(s->x3C));
            d2 = Vec3f_DistXZ(q->x10, func_8001FBA8(s->x40));
            if (D_800958A4 == 2) {
                d3 = Vec3f_DistXZ(q->x10, func_8001FBA8(s->x44));
            }
            if (d1 < d2) {
                d1 = d2;
                if (d1 < d3) {
                    o->x18 = s->x44;
                } else {
                    o->x18 = s->x40;
                }
            } else if (d1 < d3) {
                o->x18 = s->x44;
            } else {
                o->x18 = s->x3C;
            }
        } else {
            o->x18 = q->x8;
        }
        func_80000AC8(car, o->x18, &tmp);
        Ai_ApplyRel(car, tmp);
        break;
    }
    Ai_ConstrainTarget(car);
    Ai_Steer(car);
}
