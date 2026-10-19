typedef struct { f32 x, y, z; } Vec3f;

typedef struct {
    Vec3f a;
    Vec3f b;
} Edge;

typedef struct {
    s32 x0;
    s32 x4;
} Seg;

typedef struct {
    u8 pad0[0x30];
    Edge *x30;
    Seg *x34;
    s32 x38;
} Race;

typedef struct {
    u8 pad0[0x1100];
    Race *race;
} Game;

typedef struct {
    u8 pad0[0x28];
    s32 x28;
    u8 pad2C[0x2C];
    s32 x58;
    u8 pad5C[0x234];
    Vec3f pos;
    Vec3f vel;
    u8 pad2A8[0x908];
    Vec3f target;
    s32 xBBC;
    f32 xBC0;
    f32 xBC4;
    f32 xBC8;
    f32 xBCC;
} Car;

extern Game *gGame;
extern f64 D_800B9690;

void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Normalize(Vec3f *, Vec3f *);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
s32 rand(void);

void func_800363C4(Car *car, f32 dt) {
    Race *race;
    Vec3f *pos;
    Vec3f *vel;
    Vec3f *target;
    Vec3f dir;
    Vec3f step;
    Edge *e;
    s32 idx;
    s32 n;
    f32 dx, dy, dz;
    f32 r;

    vel = &car->pos;
    pos = vel;
    target = &car->target;
    race = gGame->race;
    dx = car->target.x - car->pos.x;
    dy = car->target.y - car->pos.y;
    dz = car->target.z - car->pos.z;
    if (dx * dx + dy * dy + dz * dz < 64.0) {
        idx = car->x28 + car->x58 * 2;
        n = race->x38;
        car->xBBC = idx;
        if (idx >= n) {
            car->xBBC = idx - n;
        } else if (idx < 0) {
            car->xBBC = idx + n;
        }
        e = &gGame->race->x30[car->xBBC];
        car->target.x = e->a.x * car->xBC0 + e->b.x * (1.0 - car->xBC0);
        car->target.y = e->a.y * car->xBC0 + e->b.y * (1.0 - car->xBC0);
        car->target.z = e->a.z * car->xBC0 + e->b.z * (1.0 - car->xBC0);
        car->target.y += 2.0;
    }
    car->xBC4 += car->xBCC * dt;
    if (car->xBC8 < car->xBC4) {
        car->xBC4 = car->xBC8;
    }
    Vec3f_Sub(&dir, target, pos);
    Vec3f_Normalize(&dir, &dir);
    vel++;
    Vec3f_Scale(vel, &dir, race->x34[car->xBBC].x0 * car->xBC4);
    Vec3f_Scale(&step, vel, dt);
    Vec3f_Add(pos, pos, &step);
    r = rand() * (1.0f / 32767.0f);
    car->xBC0 += (f32)(dt * ((r - 0.5) * 3.0));
    if (car->xBC0 > 0.82) {
        car->xBC0 = 0.82f;
    } else if (car->xBC0 < 0.18) {
        car->xBC0 = 0.18f;
    }
    if (r * 20.0 < dt) {
        car->xBC4 = car->xBC4 * D_800B9690;
        car->xBCC = 0.1f;
    }
}
