typedef struct { f32 x, y, z; } Vec3f;
typedef struct { s32 x0; s32 count; s32 idx[4]; } Face;
typedef struct {
    u8 pad0[0xA4];
    Vec3f pos;
    Vec3f vel;
    Vec3f rot;
    u8 padC8[0x13C - 0xC8];
    Face *faces;
    u8 pad140[4];
    Vec3f *normals;
    s32 numFaces;
    u8 pad14C[0x15C - 0x14C];
    Vec3f *verts;
} Phys;
typedef struct {
    u8 pad0[0x1EC];
    Phys p;
} Car;
typedef struct { s32 x0; Vec3f n; s32 count; Vec3f v[4]; } Poly;
typedef struct {
    f32 x0, x4, x8;
    u8 padC[4];
    Vec3f x10;
    f32 x1C;
    Vec3f x20;
    Vec3f x2C;
    u8 pad38[4];
    Vec3f x3C;
    Vec3f x48;
    u8 pad54[8];
} Hit;
typedef struct { u8 pad[0x10]; s32 x10; } Race;
typedef struct { u8 pad[0x1100]; Race *race; } Game;
extern Game *gGame;
extern const f64 D_800B94E8;
extern const f64 D_800B94F0;
extern const f64 D_800B94F8;
extern const f64 D_800B9500;
extern const f64 D_800B9508;
extern f32 D_800D2E78;
u8 Poly_IntersectSegment(Poly *, f32 *, Poly *, f32 *, Vec3f *);
void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Set(Vec3f *, f32, f32, f32);
void Car_ApplyCollisionImpulse(Car *, Car *, Hit *);
f32 sqrtf(f32);

static inline void LoadPoly(Poly *p, Phys *ph, s32 i) {
    Face *fc = &ph->faces[i];
    Vec3f *verts = ph->verts;
    s32 k;

    p->x0 = fc->x0;
    p->n = ph->normals[i];
    p->count = fc->count;
    for (k = 0; k < p->count; k++) {
        p->v[k] = verts[fc->idx[k]];
    }
}

u8 Car_CollideCars(Car *a, Car *b) {
    Poly pa;
    Poly pb;
    Hit hit;
    Vec3f pts[2];
    f32 d[2];
    Vec3f posA;
    Vec3f posB;
    Vec3f mid;
    Vec3f ra;
    Vec3f rb;
    Vec3f va;
    Vec3f vb;
    Vec3f dv;
    Vec3f n1;
    Vec3f n2;
    Phys *A = &a->p;
    Phys *B = &b->p;
    s32 i;
    s32 j;
    s32 k;
    u8 found = 0;
    f32 best = -200.0f;
    f32 dotB;
    f32 dotA;
    f32 depth;
    f32 sa;
    f32 sb;
    f32 speed;
    f32 m;
    f32 f;
    Face *fc;
    Vec3f *verts;

    for (i = 0; i < A->numFaces; i++) {
        LoadPoly(&pa, A, i);
        for (j = 0; j < B->numFaces; j++) {
            LoadPoly(&pb, B, j);
            if (!Poly_IntersectSegment(&pa, &d[0], &pb, &d[1], pts)) {
                continue;
            }
            posA = A->pos;
            posB = B->pos;
            mid.x = (pts[0].x + pts[1].x) * D_800B94E8;
            mid.y = (pts[0].y + pts[1].y) * D_800B94E8;
            mid.z = (pts[0].z + pts[1].z) * D_800B94E8;
            {
                f64 off = D_800B94F0;
                if (!gGame->race->x10) {
                    off = D_800B94F8;
                }
                mid.y += off;
            }
            ra.x = mid.x - posA.x;
            ra.y = mid.y - posA.y;
            ra.z = mid.z - posA.z;
            rb.x = mid.x - posB.x;
            rb.y = mid.y - posB.y;
            rb.z = mid.z - posB.z;
            Vec3f_Cross(&va, &A->rot, &ra);
            Vec3f_Add(&va, &va, &A->vel);
            Vec3f_Cross(&vb, &B->rot, &rb);
            Vec3f_Add(&vb, &vb, &B->vel);
            dv.x = va.x - vb.x;
            dv.y = va.y - vb.y;
            dv.z = va.z - vb.z;
            dotB = pb.n.x * dv.x + pb.n.y * dv.y + pb.n.z * dv.z;
            dotA = -(pa.n.x * dv.x + pa.n.y * dv.y + pa.n.z * dv.z);
            if (!(dotB < -best) && !(dotA < -best)) {
                continue;
            }
            if (dotB < dotA) {
                depth = d[0] * D_800B9500;
                n1 = pb.n;
                n2.x = -n1.x;
                n2.y = -n1.y;
                n2.z = -n1.z;
            } else {
                depth = d[1] * D_800B9508;
                n2 = pa.n;
                n1.x = -n2.x;
                n1.y = -n2.y;
                n1.z = -n2.z;
            }
            sa = n1.x * ra.x + n1.y * ra.y + n1.z * ra.z;
            sb = n2.x * rb.x + n2.y * rb.y + n2.z * rb.z;
            if ((sa < 0.0f) & (sb < 0.0f)) {
            } else if (found) {
                continue;
            }
            speed = sqrtf(dv.x * dv.x + dv.y * dv.y + dv.z * dv.z) * D_800D2E78;
            f = 0.1f;
            if ((sa < 0.0f) & (sb < 0.0f)) {
                if (dotB < dotA) {
                    best = -dotB;
                } else {
                    best = -dotA;
                }
            }
            found = 1;
            m = (depth <= speed) ? depth : speed;
            if (!(m <= f)) {
                f = m;
            }
            hit.x0 = 0.5f;
            hit.x1C = f;
            hit.x4 = 0.1f;
            hit.x8 = 0.1f;
            hit.x10 = n1;
            hit.x2C = ra;
            hit.x48 = rb;
            if (0.0 < best) {
                hit.x20 = va;
                hit.x3C = vb;
            } else {
                Vec3f_Set(&hit.x20, 0.0f, 0.0f, 0.0f);
                Vec3f_Set(&hit.x3C, 0.0f, 0.0f, 0.0f);
            }
        }
    }
    if (found) {
        Car_ApplyCollisionImpulse(a, b, &hit);
    }
    return found;
}
/* agent45: needs --flags -ffast-math (sqrt.s). Same size as original now (inline LoadPoly,
   f32 d[2] for da/db, off temp for mid.y, f = 0.1f placed before the best update).
   Remaining: (1) FP callee regs: best gets f24, D_800B94E8 const f22 (orig swapped;
   global-alloc priority best 13 refs/324 vs const DF 10/319); (2) normals index uses a
   shared i*3 giv instead of i*12; (3) orig reloads mid.x/y/z from the stack after the
   off branch, ours keeps them in registers. */
