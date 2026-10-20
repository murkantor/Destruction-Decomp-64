typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f r[3]; } Mtx3f;
typedef struct {
    u8 pad[0x2C];
    u8 *x2C;
    u8 pad30[0xC4 - 0x30];
    f32 xC4;
} WheelSub;
typedef struct {
    WheelSub sub;
    u8 padC8[0xE0 - 0xC8];
    f32 xE0;
    f32 xE4;
    f32 xE8;
    u8 padEC[0x120 - 0xEC];
    f32 x120;
    u8 pad124[0x194 - 0x124];
    Vec3f x194;
    f32 x1A0;
    f32 x1A4;
    u8 pad1A8[4];
    u8 x1AC;
    u8 pad1AD[3];
    f32 x1B0;
    s32 x1B4;
    f32 x1B8;
    u8 pad1BC[0x1BC - 0x1BC];
} Wheel;
typedef struct {
    u8 pad[0x2C];
    u8 *x2C;
    u8 pad30[0x1F8 - 0x30];
    f32 x1F8;
    u8 pad1FC[0x29C - 0x1FC];
    Vec3f x29C;
    Vec3f x2A8;
    Mtx3f x2B4;
    u8 pad2D8[0x364 - 0x2D8];
    Wheel wheels[4];
    u8 padA54[0xB84 - 0x364 - 4 * 0x1BC];
    f32 xB84;
    f32 xB88;
    f32 xB8C;
    u8 padB90[0xBD4 - 0xB90];
    f32 xBD4;
    f32 xBD8;
    s32 xBDC;
} Car;

extern void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
extern void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
extern void Vec3f_Scale(Vec3f *, Vec3f *, f32);
extern void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
extern f32 func_800397FC(Car *);
extern void Math_SinCos(f32, f32 *, f32 *);
extern f64 pow(f64, f64);
extern f32 fabsf(f32);
extern f64 fabs(f64);

static inline s32 gtd(f64 a, f64 b) {
    return a > b;
}

static inline f32 ramp(f32 x, f32 lo, f32 hi) {
    if (x <= lo) {
        return 0.0f;
    }
    if (x >= hi) {
        return 1.0f;
    }
    return (x - lo) / (hi - lo);
}

static inline f32 ramp0(f32 x, f32 hi) {
    if (x <= 0.0f) {
        return 0.0f;
    }
    if (x >= hi) {
        return 1.0f;
    }
    return x / hi;
}

static inline f32 tireF(f32 v, f32 a, f32 b, f32 load) {
    f32 r;
    if (v > 5.0f) {
        r = -a * load;
    } else if (v < -5.0f) {
        r = a * load;
    } else {
        r = v * -0.2f * b * load;
    }
    return r;
}

Vec3f Wheel_ComputeTireForce(Car *car, Wheel *w, f32 load, Vec3f *n, f32 *out, s32 idx) {
    u8 *p = car->x2C;
    Vec3f N = w->x194;
    Vec3f fwd;
    Vec3f side;
    Vec3f vel;
    Vec3f v;
    Vec3f vt;
    Vec3f res = { 0.0f, 0.0f, 0.0f };
    f32 dot;
    f32 radius;
    f32 invR;
    u8 p3;
    u8 front;
    u8 rear;
    WheelSub *ws;
    Mtx3f *m;
    Vec3f *cv;
    u8 p0;
    u8 p1;
    u8 p2;
    f32 c, s;
    f32 sp2;
    f32 lat;
    f32 lon;
    f32 fl;
    f32 ft;
    f32 fs;
    f32 drive;
    f32 a;
    f32 b;
    f32 k;
    f32 d;
    f32 al;
    f64 dl;

    radius = w->x120;
    invR = 1.0f / radius;
    ws = &w->sub;
    cv = &car->x29C;
    m = &car->x2B4;
    c = w->xE0;
    s = w->xE8;
    if (idx == 0 || idx == 2) {
        front = 1;
        rear = 0;
    } else {
        front = 0;
        rear = 1;
    }
    p2 = 0;
    if (idx >= 2) {
        p2 = p[2];
    }
    p0 = p[0];
    p1 = p[1];
    p3 = p[3];
    fwd.x = c * m->r[0].x + s * m->r[2].x;
    fwd.y = c * m->r[0].y + s * m->r[2].y;
    fwd.z = c * m->r[0].z + s * m->r[2].z;
    side.x = s * m->r[0].x - c * m->r[2].x;
    side.y = s * m->r[0].y - c * m->r[2].y;
    side.z = s * m->r[0].z - c * m->r[2].z;
    Vec3f_Cross(&v, &car->x2A8, n);
    v.x += cv->x;
    v.y += cv->y;
    v.z += cv->z;
    d = v.x * N.x + v.y * N.y + v.z * N.z;
    vt.x = v.x - d * N.x;
    vt.y = v.y - d * N.y;
    vt.z = v.z - d * N.z;
    k = radius * ws->xC4;
    vel.x = k * side.x + vt.x;
    vel.y = k * side.y + vt.y;
    vel.z = k * side.z + vt.z;
    sp2 = vel.x * vel.x + vel.y * vel.y + vel.z * vel.z;
    if (sp2 > 80.0f) {
        w->x1AC = 1;
        w->x1B4 = 2;
        w->x1B0 = ramp(sp2, 80.0f, 160.0f);
    } else {
        w->x1AC = 0;
    }
    if (p2) {
        sp2 = vt.x * vt.x + vt.y * vt.y + vt.z * vt.z;
        if (sp2 > 1.0) {
            w->x1AC = 1;
            w->x1B4 = 1;
            w->x1B0 = ramp(sp2, 1.0f, 120.0f);
        }
    }
    lat = vel.x * fwd.x + vel.y * fwd.y + vel.z * fwd.z;
    lon = vel.x * side.x + vel.y * side.y + vel.z * side.z;
    load = load * N.y;
    fl = tireF(lon, w->x1A4, w->x1A0, load);
    if (p2) {
        a = w->x1A4 * 0.55;
        b = w->x1A0 * 0.55;
        ft = tireF(lat, a, b, load);
    } else if (idx >= 2) {
        f32 g = w->x1B8 * 0.45 + 0.55;
        ft = tireF(lat, g * w->x1A4, g * w->x1A0, load);
    } else {
        ft = tireF(lat, w->x1A4, w->x1A0, load);
    }
    fs = cv->x * m->r[0].x + cv->y * m->r[0].y + cv->z * m->r[0].z;
    if ((p3 && fs > 3.0) || (p0 && !p3 && fs < -3.0)) {
        w->x1AC = 1;
        w->x1B4 = 0;
        w->x1B0 = ramp0(fabsf(fs), 30.0f);
        fl *= 1.3;
        ft *= 1.3;
        p0 = 0;
        p1 = 1;
    }
    fl *= car->xB84;
    ft *= car->xB84;
    if (p0 && !p2) {
        f32 q;
        f32 fx;
        f32 aq;
        f32 lim;
        drive = func_800397FC(car);
        if (p1) {
            drive *= 0.1;
        }
        q = drive * invR;
        lim = w->x1A0 * load;
        aq = fabsf(q);
        Vec3f_Dot(&dot, &vt, &vt);
        if ((lim < aq || fabs(lon) > 8.0 || w->x1AC) && dot < 1225.0) {
            if (!w->x1AC) {
                w->x1AC = 1;
                w->x1B4 = 0;
                w->x1B0 = ramp0(aq, 6000.0f);
            }
            *out = *out - drive + fl * radius;
            fx = fl;
        } else {
            *out = 0.0f;
            k = -invR * (vt.x * side.x + vt.y * side.y + vt.z * side.z);
            ws->xC4 = k;
            fx = q;
        }
        res.x = fx * side.x + ft * fwd.x;
        res.y = fx * side.y + ft * fwd.y;
        res.z = fx * side.z + ft * fwd.z;
    } else {
        if (p2 || (p1 && fabs(ws->xC4) < 10.0)) {
            *out = 0.0f;
            ws->xC4 = 0.0f;
        } else if (p1) {
            *out = 0.0f;
            if (ws->xC4 > *out) {
                ws->xC4 = ws->xC4 - car->xB88 * 1.4;
            } else {
                ws->xC4 = ws->xC4 + car->xB88 * 1.4;
            }
        } else {
            f32 h;
            *out = 0.0f;
            k = -invR * (vt.x * side.x + vt.y * side.y + vt.z * side.z);
            ws->xC4 = k;
            h = car->xB8C * 0.6;
            if (ws->xC4 > h * 2.0) {
                ws->xC4 = ws->xC4 - h;
            } else if (ws->xC4 < -h * 2.0) {
                ws->xC4 = ws->xC4 + h;
            } else if (N.y > 0.98) {
                ws->xC4 = ws->xC4 * 0.95;
            }
        }
        res.x = fl * side.x + ft * fwd.x;
        res.y = fl * side.y + ft * fwd.y;
        res.z = fl * side.z + ft * fwd.z;
    }
    dl = lat;
    al = fabsf(lat);
    if (!w->x1AC && (al > 7.0 || (rear & gtd(dl, 4.0)) || (front & gtd(-4.0, dl)))) {
        if (!w->x1AC) {
            w->x1B4 = 2;
            w->x1AC = 1;
            w->x1B0 = ramp0(al, 8.0f);
        }
    } else if (idx >= 2) {
        Wheel *o = (idx == 2) ? &car->wheels[1] : &car->wheels[0];
        if (o->x1AC && o->x1B4 == 2 && !w->x1AC) {
            w->x1AC = 1;
            w->x1B4 = 2;
            w->x1B0 = o->x1B0;
        }
    }
    if (car->xBDC) {
        if (car->xBD4 < 0.51) {
            Vec3f_Scale(&res, &res, car->xBD4);
            w->x1AC = 1;
            w->x1B4 = 0;
            w->x1B0 = 1.0f;
            if (car->xBD4 < 0.35 && idx >= 2) {
                Vec3f t;
                f32 sn;
                f32 cs;
                f32 r = car->x1F8 * 1.2;
                t = car->x2B4.r[2];
                Math_SinCos((car->xBD8 + 0.1) * 4.084070445, &sn, &cs);
                Vec3f_Scale(&t, &t, cs * r);
                Vec3f_Add(&res, &res, &t);
            }
        } else {
            f32 r;
            if (car->xBD8 < 2.0) {
                w->x1AC = 1;
                w->x1B4 = 0;
                w->x1B0 = 1.0f;
            }
            r = pow(0.75, car->xBD8) * 1.2 + 1.0;
            Vec3f_Scale(&res, &res, r);
            *out = *out * r;
        }
    }
    return res;
}
/* agent54: code now identical except the frame: ours is 0x128, original 0x130 (8 unused bytes at
 * 208(sp), above the last spill slot rear@207). All other words match (46 words differ = the
 * frame-relative offsets). Fixes this pass: tireF inline uses one local r and a single return
 * (gives the mov.s from f0); res in the drive branch goes through fx (fl / q) so q lands in s1;
 * gtd() inline for the (dl > 4.0) & rear store-flags; al = fabsf(lat) and dl = lat before the if;
 * spin value of xC4 reuses k, and the threshold uses a new h; t = r[2] (was r[1]);
 * r = x1F8 * 1.2 before Math_SinCos; w->x1B4 = 2 in the o-copy branch; ws->xC4 > *out.
 * Unused char/s32 arrays shift the spill slots (locals sit below them), so the 8 bytes are
 * probably a spill slot reload allocated and then did not use, or a caller-save slot. */
