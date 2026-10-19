#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

typedef struct Car Car;

typedef struct {
    u8 pad0[0x18];
    Car *x18;
    u8 pad1C[0x5C - 0x1C];
    Vec3f x5C;
    f32 x68;
} Tracker;

struct Car {
    u8 pad0[0xD];
    u8 xD;
    u8 padE[0x30 - 0xE];
    s32 x30;
    u8 pad34[0x38 - 0x34];
    Tracker *x38;
};

typedef struct {
    u8 pad0[0x74];
    Car *x74[(0xA4 - 0x74) / 4];
    s32 xA4;
} Race;

typedef struct {
    u8 pad0[0x1100];
    Race *x1100;
} Game;

extern Game *gGame;
Vec3f func_8001FBA8(Car *);
Vec3f func_8001FC08(Car *);
f32 Vec3f_DistXZ(Vec3f, Vec3f);
f32 func_800005C8(Vec3f, Vec3f);

void AI_PickAttackTarget(Car *self) {
    Tracker *t = self->x38;
    Vec3f dir;
    Vec3f n;
    Vec3f pos;
    Vec3f tp;
    Vec3f q;
    Vec3f r;
    Vec3f p2;
    f32 best;
    f32 bestT;
    f32 d;
    f32 dist;
    Car *target;
    Car *c;
    Tracker *ct;
    s32 i;
    s32 found;

    pos = func_8001FBA8(self);
    best = 100000.0f;
    target = NULL;
    found = 0;
    bestT = best;
    for (i = 0; i < gGame->x1100->xA4; i++) {
        c = gGame->x1100->x74[i];
        ct = c->x38;
        if (c->xD != 0 || c == self) {
            continue;
        }
        p2 = func_8001FBA8(c);
        d = Vec3f_DistXZ(pos, p2);
        if (c->x30 == 1 && ct->x18 == self) {
            found = 1;
            if (d < bestT) {
                target = c;
                bestT = d;
            }
        } else if (d < best) {
            target = c;
            best = d;
        }
    }
    if (target != NULL) {
        tp = func_8001FBA8(target);
        dir.x = tp.x - pos.x;
        dir.y = tp.y - pos.y;
        dir.z = tp.z - pos.z;
        dist = best;
        if (found) {
            dist = bestT;
        }
        n.x = dir.x / dist;
        n.y = dir.y / dist;
        n.z = dir.z / dist;
        n.x = -n.x;
        n.y = -n.y;
        n.z = -n.z;
        t->x5C.x = pos.x + n.x * 100.0f;
        t->x5C.y = pos.y + n.y * 100.0f;
        t->x5C.z = pos.z + n.z * 100.0f;
        q = func_8001FC08(self);
        r.x = t->x5C.x - pos.x;
        r.y = t->x5C.y - pos.y;
        r.z = t->x5C.z - pos.z;
        t->x68 = func_800005C8(q, r);
    }
}
