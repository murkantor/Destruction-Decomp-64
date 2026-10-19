#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

typedef struct Car Car;

typedef struct {
    u8 pad0[0x18];
    Car *x18;
} Tracker;

struct Car {
    u8 pad0[0x34];
    s32 x34;
    Tracker *x38;
};

typedef struct Node {
    Car *car;
    struct Node *next;
} Node;

typedef struct {
    u8 pad0[0x20];
    Node *x20;
    u8 pad24[0xA4 - 0x24];
    s32 xA4;
} Race;

typedef struct {
    u8 pad0[0x1100];
    Race *x1100;
} Game;

extern Game *gGame;
Vec3f func_8001FBA8(Car *);
Vec3f func_8001FC08(Car *);
f64 acos(f64);
f32 sqrtf(f32);
f64 fabs(f64);

static inline f32 DistXZ(Vec3f a, Vec3f b) {
    f32 dx = a.x - b.x;
    f32 dz = a.z - b.z;

    if (dx == 0.0 && dz == 0.0) {
        return 1.0f;
    }
    return sqrtf(dx * dx + dz * dz);
}

static inline f32 AngleBetween(Vec3f a, Vec3f b) {
    f32 dot = a.x * b.x + a.y * b.y + a.z * b.z;

    if (fabs(dot) < 9.99999999999999955e-07) {
        return 0.0f;
    }
    return acos(dot / sqrtf((a.x * a.x + a.y * a.y + a.z * a.z) * (b.x * b.x + b.y * b.y + b.z * b.z))) * 57.2957795785522919;
}

s32 AI_FindTargetAhead(Car *self) {
    Tracker *t = self->x38;
    Node *node = gGame->x1100->x20;
    f32 best = 3000.0f;
    f32 bestAng = 180.0f;
    Car *target = NULL;
    Car *c;
    s32 i;
    Vec3f dir;
    Vec3f a;
    Vec3f b;
    f32 dist;
    f32 ang;

    for (i = 0; i < gGame->x1100->xA4; node = node->next, i++) {
        if (node->car->x34 == self->x34) {
            break;
        }
        c = node->car;
        a = func_8001FBA8(self);
        b = func_8001FBA8(c);
        dir.x = b.x - a.x;
        dir.y = 0.0f;
        dir.z = b.z - a.z;
        dist = DistXZ(a, b);
        if (dist < best) {
            ang = AngleBetween(dir, func_8001FC08(self));
            if (ang < bestAng) {
                best = dist;
                bestAng = ang;
                target = c;
            }
        }
    }
    if (bestAng < 10.0) {
        t->x18 = target;
        return 1;
    }
    return 0;
}
