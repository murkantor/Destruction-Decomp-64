typedef struct { f32 x, y, z; } Vec3f;

typedef struct {
    Vec3f pos;           /* 0x00 */
    Vec3f vel;           /* 0x0C */
    Vec3f angvel;        /* 0x18 */
    Vec3f rot[3];        /* 0x24 */
    f32 inertia[6];      /* 0x48 */
    f32 invInertia[6];   /* 0x60 */
} BodyState;             /* 0x78 */

typedef struct {
    u8 pad0[0x10];
    f32 x10;
    Vec3f x14;
    Vec3f x20;
    BodyState prev;      /* 0x2C */
    BodyState cur;       /* 0xA4 */
} Body;                  /* 0x11C */

typedef struct {
    Vec3f pos;           /* 0x00 */
    u8 pad0C[0x20 - 0xC];
    f32 spin;            /* 0x20 */
    Vec3f m[3];          /* 0x24 */
} WheelXf;               /* 0x48 */

typedef struct {
    u8 pad0[0x28];
    f32 x28;
    u8 pad2C[0xA4 - 0x2C];
    WheelXf xf;          /* 0xA4 */
    u8 padEC[0x17C - 0xEC];
    f32 x17C;
    f32 x180;
    f32 x184;
    u8 pad188[0x194 - 0x188];
    Vec3f x194;
    u8 pad1A0[0x1A8 - 0x1A0];
    s32 x1A8;
    u8 x1AC;
    u8 pad1AD[0x1B4 - 0x1AD];
    s32 x1B4;
    u8 pad1B8[0x1BC - 0x1B8];
} Wheel;                 /* 0x1BC */

typedef struct { u8 x0; u8 x1; u8 x2; u8 x3; s16 x4; } CarInput;
typedef struct { u8 pad[0xB4]; u8 xB4; } Unk3CC;
typedef struct { u8 pad[0xC]; Unk3CC *xC; } Unk3C;
typedef struct { u8 pad[0x38]; Vec3f pos; } Ent;

typedef struct {
    u8 pad0[0xC];
    u8 xC;
    u8 xD;
    u8 padE[0x2C - 0xE];
    CarInput *x2C;
    s32 x30;
    u8 pad34[0x3C - 0x34];
    Unk3C *x3C;
    u8 pad40[0x50 - 0x40];
    u64 x50;
    u8 pad58[0x198 - 0x58];
    s32 x198;
    u8 pad19C[0x1EC - 0x19C];
    Body body;           /* 0x1EC */
    u8 pad308[0x35C - 0x308];
    f32 x35C;
    f32 x360;
    Wheel wheels[4];     /* 0x364 */
    u8 padA54[0xA5C - 0xA54];
    f32 xA5C;
    f32 xA60;
    u8 padA64[0xB9C - 0xA64];
    f32 xB9C;
    u8 padBA0[0xBA8 - 0xBA0];
    f32 xBA8;
    u8 xBAC;
    u8 padBAD[0xBD4 - 0xBAD];
    f32 xBD4;
    f32 xBD8;
    s32 xBDC;
    u8 padBE0;
    u8 xBE1;
    u8 padBE2[3];
    u8 xBE5;
    u8 padBE6[2];
    f32 xBE8;
} Car;

typedef struct { u8 pad[0x10]; s32 x10; } Unk1100;
typedef struct {
    u8 pad0[0xB40];
    Ent *xB40[1];
    u8 padB44[0x10A8 - 0xB44];
    u8 x10A8[0x10FC - 0x10A8];
    s16 x10FC;
    u8 pad10FE[2];
    Unk1100 *x1100;
} Game;

extern Game *gGame;
extern s32 D_800960D0;
extern u8 D_800D2E58;
extern u32 D_800958B4;
extern s32 D_800958A8;
extern s32 D_80095894;
extern Vec3f D_800B9620;
extern Vec3f D_800B9698;

f32 sqrtf(f32);
f32 fabsf(f32);
double fabs(double);
double atan2(double, double);
u64 Time_Get(void *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
void Vec3f_Normalize(Vec3f *, Vec3f *);
void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Set(Vec3f *, f32, f32, f32);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_TransformTransposed(Vec3f *, Vec3f *, Vec3f *, Vec3f *);
void Tensor_MulVec3f(Vec3f *, f32 *, Vec3f *);
void Inertia_RotateDiag(Vec3f *, Vec3f *, f32 *);
void Mtx2x3_RotateY(Vec3f *, f32);
void func_8003A568(Vec3f *, Vec3f *, f32);
void func_8003A4D8(Vec3f *, Vec3f *);
void Car_ApplySuspensionForces(Car *, Vec3f *, Vec3f *, f32);
void func_800329C0(s32, Vec3f *);
void Car_ApplyWheelForces(Car *, Vec3f *, Vec3f *, f32 *);
void Car_SelectGear(Car *, f32);
void Car_UpdateEngineRpm(Car *);
void Car_UpdateRpm(Car *, f32);
void CarAI_InitTarget(Car *);
void CarAI_SteerToTarget(Car *, f32);

static inline void Wheel_ClampY(Wheel *w) {
    f32 top = w->x184;
    f32 lo = w->x180;
    f32 hi = w->x17C;

    if (w->xf.pos.y < top - lo) {
        w->xf.pos.y = top - lo;
    }
    if (top - hi < w->xf.pos.y) {
        w->xf.pos.y = top - hi;
    }
}

static inline Vec3f Body_AngStep(f32 *inertia, f32 *invInertia, Vec3f *angvel, Vec3f *torque, f32 h) {
    Vec3f dw;
    Vec3f iw;
    Vec3f gyro;

    Tensor_MulVec3f(&iw, inertia, angvel);
    Vec3f_Cross(&gyro, &iw, angvel);
    Vec3f_Add(&gyro, &gyro, torque);
    Tensor_MulVec3f(&dw, invInertia, &gyro);
    dw.x *= h;
    dw.y *= h;
    dw.z *= h;
    return dw;
}

void Car_UpdatePhysics(Car *car, f32 dt) {
    Game *game = gGame;
    Body *body = &car->body;
    Vec3f *aipos = &car->body.cur.pos;
    f32 *inertia = body->cur.inertia;
    f32 *invInertia = body->cur.invInertia;
    Vec3f *pos = &body->cur.pos;
    Vec3f *vel = &body->cur.vel;
    Vec3f *rot = body->cur.rot;
    Vec3f *angvel = &body->cur.angvel;
    u8 hit = 0;
    u8 allGround = 1;
    u8 canBoost;
    u8 blocked;
    s32 numAir;
    s32 i;
    s32 j;
    s32 k;
    Wheel *sw;
    WheelXf *sxf;
    f32 sa;
    f32 best;
    f32 ramp;
    f32 steer;
    f32 h;
    Vec3f v10;
    Vec3f v20;
    Vec3f v30;
    f32 f40;
    f32 f44;
    f32 f108;
    f32 f150;
    f32 f154;
    f32 f158;
    f32 f15C;
    f32 f160;
    f32 f164;
    f32 f168;

    if (game->x10FC == 0x1A && car->xC) {
        return;
    }
    if (game->x10FC == 0x1B) {
        goto rpm;
    }
    body->prev = body->cur;
    if (car->xBDC) {
        if (car->xBD8 == 0.0) {
            f32 r = car->xBA8 / car->x198 - 0.65;
            f32 a = fabsf(r);
            if (!car->x2C->x0 && !car->x2C->x3) {
                car->xBDC = 0;
            } else if (a < 0.06) {
                car->xBD4 = 3.0f;
            } else if (a < 0.2) {
                car->xBDC = 0;
            } else if (r < 0.0) {
                car->xBDC = 0;
            } else {
                car->xBD4 = (0.35 - r) * (0.4 / (0.35 - 0.2)) + 0.1;
            }
            car->xBD8 = 0.001f;
        } else {
            car->xBD8 += dt;
            if (car->xBD8 >= 4.0 || (!car->x2C->x0 && !car->x2C->x3)) {
                car->xBDC = 0;
            }
        }
    }
    if (D_800960D0 && gGame->x1100->x10 == 0 && car->x30 == 1) {
        if (car->xBAC) {
            goto steer;
        }
        best = 10000000.0f;
        for (k = 0; k < D_800D2E58; k++) {
            Ent *e = gGame->xB40[k];
            f32 dx = e->pos.x - aipos->x;
            f32 dy = e->pos.y - aipos->y;
            f32 dz = e->pos.z - aipos->z;
            f32 d = dx * dx + dy * dy + dz * dz;
            if (d < best) {
                best = d;
            }
        }
        if (best > 40000.0) {
            CarAI_InitTarget(car);
        }
    }
    if (car->xBAC) {
    steer:
        CarAI_SteerToTarget(car, dt);
        best = 10000000.0f;
        for (k = 0; k < D_800D2E58; k++) {
            Ent *e = gGame->xB40[k];
            f32 dx = e->pos.x - aipos->x;
            f32 dy = e->pos.y - aipos->y;
            f32 dz = e->pos.z - aipos->z;
            f32 d = dx * dx + dy * dy + dz * dz;
            if (d < best) {
                best = d;
            }
        }
        if (!(best < 39204.0) && D_800960D0) {
            return;
        }
        {
            Body *b = &car->body;
            Vec3f *r0 = &b->cur.rot[0];
            Vec3f *r1 = &b->cur.rot[1];
            Vec3f *r2 = &b->cur.rot[2];

            v30 = D_800B9620;
            if (b->cur.vel.x == 0.0 && b->cur.vel.y == 0.0 && b->cur.vel.z == 0.0) {
                Vec3f_Set(r0, 1.0f, 0.0f, 0.0f);
            } else {
                Vec3f_Normalize(r0, &b->cur.vel);
            }
            Vec3f_Cross(r2, r0, &v30);
            Vec3f_Normalize(r2, r2);
            Vec3f_Cross(r1, r2, r0);
            Vec3f_Normalize(r1, r1);
        }
        car->xBAC = 0;
        return;
    }
    canBoost = 0;
    blocked = 0;
    if (Time_Get(gGame->x10A8) >= car->x50 && D_800958B4 >= 4 && D_800958A8 != 2) {
        canBoost = car->xBE1 == 0;
    }
    if (car->x30 == 0 && car->x3C->xC->xB4 == 7) {
        blocked = 1;
    }
    if (D_80095894 == 0 && car->x2C->x1 && !car->xD && !canBoost && !blocked) {
        Vec3f_Dot(&f40, vel, rot);
        if (f40 < 1.0) {
            car->x2C->x1 = 0;
            car->x2C->x3 = 1;
            car->x2C->x0 = 1;
        }
    }
    numAir = 0;
    car->xA60 = 1000.0f;
    for (i = 0; i < 4; i++) {
        Wheel *w = &car->wheels[i];
        u8 g = 0;
        if (allGround && w->x194.z >= 1.0) {
            g = 1;
        }
        allGround = g;
        if (w->x194.z >= 1.0) {
            f32 d = w->x194.z - 1.0f;
            hit = 1;
            car->xA60 = (car->xA60 <= d) ? car->xA60 : d;
        } else {
            numAir++;
        }
    }
    if (!allGround) {
        car->xA60 = 0;
    }
    ramp = dt * 10.0 * car->xB9C;
    if (ramp > 1.0) {
        ramp = 1.0f;
    }
    {
        f32 s = (f32)(car->x2C->x4 * 0.0164);
        steer = -(car->x360 / (sqrtf(car->body.cur.vel.x * car->body.cur.vel.x + car->body.cur.vel.y * car->body.cur.vel.y + car->body.cur.vel.z * car->body.cur.vel.z) + car->x35C)) * s * s * s;
    }
    Vec3f_Dot(&v30.x, &car->body.cur.vel, car->body.cur.rot);
    if (v30.x < -5.0) {
        if (v30.x < -50.0) {
            steer *= 0.25f;
        } else {
            steer *= (f32)((v30.x + 50.0) * 0.016666666666666666 + 0.25);
        }
    }
    car->xA5C = steer;
    for (i = 0; i < 2; i++) {
        Wheel *w = &car->wheels[i];
        Vec3f *m = w->xf.m;
        f32 a = atan2(w->xf.m[2].x, w->xf.m[2].z);
        v30 = D_800B9620;
        func_8003A568(m, &v30, ramp * (car->xA5C - a));
    }
    Car_SelectGear(car, -car->wheels[0].xf.spin * 9.5493);
    Car_UpdateEngineRpm(car);
    if ((car->wheels[0].x194.z < 1.0) == (car->wheels[1].x194.z < 1.0)) {
        car->wheels[1].xf.spin = car->wheels[0].xf.spin = (car->wheels[0].xf.spin + car->wheels[1].xf.spin) * 0.5;
    }
    if ((car->wheels[2].x194.z < 1.0) == (car->wheels[3].x194.z < 1.0)) {
        car->wheels[3].xf.spin = car->wheels[2].xf.spin = (car->wheels[2].xf.spin + car->wheels[3].xf.spin) * 0.5;
    }
    Vec3f_Dot(&f44, angvel, angvel);
    {
        f32 maxw = 15.0f;
        if (gGame->x1100->x10 == 0) {
            maxw = 6.0f;
        }
        if (maxw * maxw < f44) {
            Vec3f_Normalize(angvel, angvel);
            Vec3f_Scale(angvel, angvel, maxw);
        }
    }
    {
    Vec3f v48;
    Vec3f savedPos[4];
    Vec3f localPos[4];
    Vec3f vB8;
    Vec3f vC8;
    Vec3f vD8;
    Vec3f vE8;

    for (i = 0; i < 4; i++) {
        Wheel *w = &car->wheels[i];
        savedPos[i] = w->xf.pos;
        Vec3f_TransformTransposed(&vB8, rot, pos, &savedPos[i]);
        localPos[i] = vB8;
    }
    h = dt * 0.3333333333333333;
    for (i = 0; i < 3; i++) {
        vB8 = D_800B9698;
        vC8 = D_800B9698;
        vD8 = D_800B9698;
        vE8 = D_800B9698;
        {
        f32 torque[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        pos->x += vel->x * h;
        pos->y += vel->y * h;
        pos->z += vel->z * h;
        if (car->xBE5) {
            Vec3f_Dot(&f108, angvel, rot);
            if (fabs(f108) < 6.0) {
                Vec3f v110;
                f32 s;
                {
                    f32 d = f108;
                    s = (d > 0.0) ? 6.0 - d : -6.0 - d;
                }
                do { Vec3f_Scale(&v110, rot, s); } while (0);
                Vec3f_Add(angvel, angvel, &v110);
            }
        }
        {
        v10.x = angvel->x * h;
        v10.y = angvel->y * h;
        v10.z = angvel->z * h;
        func_8003A4D8(rot, &v10);
        Car_ApplyWheelForces(car, &vD8, &vE8, torque);
        Car_ApplySuspensionForces(car, &vB8, &vC8, h);
        Vec3f_Add(&v30, &vD8, &vB8);
        Vec3f_Add(&v48, &vE8, &vC8);
        vel->x += body->x10 * v30.x * h;
        vel->y += body->x10 * v30.y * h;
        vel->z += body->x10 * v30.z * h;
        v20 = Body_AngStep(inertia, invInertia, angvel, &v48, h);
        angvel->x += v20.x;
        angvel->y += v20.y;
        angvel->z += v20.z;
        j = 0;
        do {
            sw = &car->wheels[j];
            sxf = &sw->xf;
            sa = sxf->spin * h;
            sxf->spin += sw->x28 * torque[j] * h;
            if (sxf->spin > 200.0f) {
                sxf->spin = 200.0f;
            } else if (sxf->spin < -200.0f) {
                sxf->spin = -200.0f;
            }
            Mtx2x3_RotateY(sxf->m, sa);
            j++;
        } while (j < 4);
        {
        Vec3f v120;
        Vec3f v130;

        Vec3f_Dot(&f150, vel, vel);
        if (f150 < 4.0) {
            func_800329C0(car->wheels[0].x1A8, &v120);
            if (!car->x2C->x0) {
                if (!car->x2C->x3 && !hit) {
                    f32 a;
                    f32 b;
                    f32 lim;
                    if (car->x2C->x1 || car->x2C->x2) {
                        lim = 0.36 * v120.z * v120.z;
                        a = 4.0f;
                        b = 0.8f;
                    } else {
                        a = 0.4f;
                        lim = 0.05f;
                        b = a;
                    }
                    if (fabs(rot[0].y) < lim) {
                        if (f150 < b) {
                            vel->x = 0;
                            vel->z = 0;
                            Vec3f_Dot(&f154, angvel, angvel);
                            if (f154 < 0.1) {
                                Vec3f_Set(angvel, 0.0f, 0.0f, 0.0f);
                            }
                        } else if (f150 < a) {
                            Vec3f_Dot(&f158, vel, rot);
                            Vec3f_Scale(&v130, rot, f158 * -0.04);
                            Vec3f_Add(vel, vel, &v130);
                        }
                    }
                }
                if (!car->x2C->x0 && !car->x2C->x3 && !hit) {
                    f32 lim2 = 0.48 * v120.z * v120.z;
                    if (fabs(rot[2].y) < lim2) {
                        Vec3f *r2 = &rot[2];
                        Vec3f_Dot(&f15C, vel, r2);
                        if (fabs(f15C) < 0.2) {
                            Vec3f_Scale(&v130, r2, -f15C);
                        } else {
                            Vec3f_Scale(&v130, r2, f15C * -0.04);
                        }
                        Vec3f_Add(vel, vel, &v130);
                    }
                }
            }
        }
        if (i < 2) {
            for (j = 0; j < 4; j++) {
                Wheel *w = &car->wheels[j];
                if (w->x194.z < 1.0f && w->x194.y > 0.0f) {
                    Vec3f_TransformTransposed(&v120, rot, pos, &savedPos[j]);
                    Vec3f_Sub(&v130, &localPos[j], &v120);
                    Vec3f_Dot(&f160, &v130, &w->x194);
                    Vec3f_Dot(&f164, &w->x194, &rot[1]);
                    if (f164 > 0.1) {
                        w->xf.pos.y = savedPos[j].y + f160 * (f32)(1.0 / f164);
                        Wheel_ClampY(w);
                    }
                }
            }
        }
        }
        }
        }
    }
    for (i = 0; i < 4; i++) {
        Wheel *w = &car->wheels[i];
        w->xf.pos = savedPos[i];
        if (w->x1AC && w->x1B4 == 2) {
            Vec3f_Scale(vel, vel, 0.9987f);
        }
    }
    car->xBE8 += dt;
    if (car->xBE8 > 100000.0) {
        car->xBE8 = 100000.0f;
    }
    if (car->x2C->x2) {
        car->xBE8 = 10.0f;
    }
    if (numAir >= 3) {
        Vec3f_Dot(&f168, vel, rot);
        if (f168 < -44.0) {
            Vec3f_Scale(vel, vel, 0.97f);
        }
    }
    {
        f32 drop = dt * 10.0f;
        for (j = 0; j < 4; j++) {
            Wheel *w = &car->wheels[j];
            w->xf.pos.y -= drop;
            Wheel_ClampY(w);
        }
    }
    if (rot[1].y > 0.707) {
        car->xBE5 = 0;
    }
    Inertia_RotateDiag(body->cur.rot, &body->x14, body->cur.inertia);
    Inertia_RotateDiag(body->cur.rot, &body->x20, body->cur.invInertia);
    }
    return;
rpm:
    Car_UpdateRpm(car, dt);
}
