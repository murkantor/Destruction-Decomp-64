typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 m[9]; } Mtx3;
typedef struct {
    u8 pad0[0x30];
    f32 x30;
    u8 pad34[0xA8 - 0x34];
    f32 xA8;
    u8 padAC[0x170 - 0xAC];
    f32 x170;
    f32 x174;
    f32 x178;
    u8 pad17C[0x184 - 0x17C];
    f32 x184;
    u8 pad188[0x190 - 0x188];
    f32 x190;
    u8 pad194[0x198 - 0x194];
    f32 x198;
    f32 x19C;
    u8 pad1A0[0x1BC - 0x1A0];
} Wheel;
typedef struct {
    u8 pad0[0xC];
    f32 xC;
    f32 x10;
    u8 pad14[0xB0 - 0x14];
    Vec3f xB0;
    Vec3f xBC;
    Mtx3 xC8;
} RigidBody;
typedef struct {
    u8 pad0[0x1EC];
    RigidBody rb;
    u8 pad2D8[0x364 - 0x2D8];
    Wheel wheels[4];
} Car;
typedef struct { u8 pad[0x10]; s32 x10; } Opts;
typedef struct { u8 pad[0x1100]; Opts *x1100; } Game;

extern f32 D_800960D4[];
extern Game *gGame;

Vec3f Mtx3_MulVec3fTransposed(Mtx3 *, Vec3f *);
void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
f32 func_80039DA4(f32, f32, f32, f32);
f64 fabs(f64);

void Car_ApplySuspensionForces(Car *car, Vec3f *force, Vec3f *torque, f32 mass) {
    f64 lim = 70.0;
    s32 i;
    RigidBody *rb = &car->rb;
    Vec3f *lv = &rb->xB0;
    Vec3f *av = &rb->xBC;
    Vec3f *up = (Vec3f *)&rb->xC8.m[3];

    for (i = 0; i < 4; i++) {
        Wheel *w = &car->wheels[i];
        if (w->x19C < 1.0f && w->x198 > 0.0f) {
            Vec3f f;
            Vec3f t;
            Vec3f v;
            Vec3f c;
            f32 susp;
            f32 vel;
            f32 slip;
            f32 res;
            Wheel *w2;

            v.x = D_800960D4[i];
            v.y = -0.095f;
            v.z = w->x190;
            susp = (w->x184 - w->x178) - w->xA8;
            v = Mtx3_MulVec3fTransposed(&rb->xC8, &v);
            Vec3f_Cross(&c, av, &v);
            vel = c.x * up->x + c.y * up->y + c.z * up->z;
            w2 = w;
            if (vel > 4.0) {
                vel = 4.0f;
            } else if (vel < -4.0) {
                vel = -4.0f;
            }
            vel += lv->x * up->x + lv->y * up->y + lv->z * up->z;
            slip = (w2->x30 - w2->xA8) / mass;
            if (fabs(vel) < 2.0 || fabs(vel) < fabs(slip)) {
                slip = vel;
            }
            if (slip > 20.0) {
                slip = 20.0f;
            } else if (slip < -20.0) {
                slip = -20.0f;
            }
            res = func_80039DA4(w->x170, susp, w->x174, slip);
            if (gGame->x1100->x10 == 0) {
                res *= 1.25;
            }
            if (res * rb->x10 > lim) {
                res = rb->xC * lim;
            } else if (res * rb->x10 < -1.0) {
                res = -rb->xC;
            }
            f.x = res * up->x;
            f.y = res * up->y;
            f.z = res * up->z;
            Vec3f_Cross(&t, &v, &f);
            force->x += f.x;
            force->y += f.y;
            force->z += f.z;
            if (res < 0.0) {
                Vec3f_Scale(&t, &t, 0.5f);
            }
            torque->x += t.x;
            torque->y += t.y;
            torque->z += t.z;
        }
        w->x30 = w->xA8;
    }
}
