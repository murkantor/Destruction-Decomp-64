typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f min; Vec3f max; } AABB;
typedef struct { s32 x0; Vec3f n; s32 count; Vec3f v[4]; } Poly;
typedef struct { s32 x0; Poly p; } FaceEnt;
typedef struct {
    s32 x0;
    f32 x4;
    f32 x8;
    s32 xC;
    Vec3f n;
    s32 x1C;
    Vec3f vel;
    Vec3f rel;
    Vec3f x3C;
    Vec3f x48;
    u8 pad54[0xC];
} Contact;
typedef struct {
    u8 pad0[0xC];
    f32 xC;
    u8 pad10[0x1C];
    Vec3f pos0;
    u8 pad38[0xA4 - 0x38];
    Vec3f pos1;
    Vec3f vel1;
    Vec3f angVel1;
    Vec3f m[3];
    u8 padEC[0x120 - 0xEC];
    f32 x120;
    u8 pad124[0xC];
    f32 x130;
    f32 x134;
    f32 x138;
} Body;

extern f32 D_800D2E78;
void Vec3f_Set(Vec3f *, f32, f32, f32);
void Vec3f_TransformTransposed(Vec3f *, Vec3f *, Vec3f *, Vec3f *);
s32 Track_GetFacesInRadius(Vec3f *, f32, FaceEnt *);
void Vec3f_Length(f32 *, Vec3f *);
void Points_ComputeAABB(Poly *, AABB *);
u8 func_80032FC0(AABB *, AABB *);
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Normalize(Vec3f *, Vec3f *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
void func_800329C0(s32, Contact *);
void Physics_AddContact(void *, s32 *, Contact *, Body *);
f32 sqrtf(f32);
f64 fabs(f64);
f32 fabsf(f32);

void Car_SweepTrackFaces(Body *b, void *buf, s32 *cnt) {
    FaceEnt faces[128];
    Vec3f center;
    AABB box;
    Vec3f pts[7];
    Vec3f A[7];
    Vec3f B[7];
    Vec3f pos0;
    Vec3f pos1;
    Poly *hitFaces[10];
    Vec3f tmp;
    Vec3f tmp2;
    f32 len;
    AABB fbox;
    Vec3f p0;
    Vec3f p1;
    Vec3f hit;
    Vec3f d;
    Vec3f mv;
    Vec3f en;
    f32 dist;
    f32 dist2;
    Contact contact;
    f32 dd;
    Vec3f c1;
    Vec3f c2;
    f32 dd2;
    f32 dd3;
    f32 ex, ey, ez;
    f32 r, x, y, z, w, top;
    f32 diam;
    f32 k;
    f32 t;
    f32 depth;
    f32 lim;
    f32 d0, d1;
    f32 frac, tol;
    s32 nFaces;
    s32 iter;
    s32 nHit;
    s32 i, j, e;
    s32 bestPt;
    s32 flag2;
    s32 edgeHit;
    s32 inside;
    s32 flag3;
    s32 found;
    s32 hitf;
    s32 dup;
    s32 fp;
    Poly *prevFace;
    Poly *bestFace;
    Poly *p;
    Vec3f *n;
    Vec3f *vp;
    Vec3f *pa;
    Vec3f *pb;

    center = b->pos1;
    ex = fabs(b->x130 * b->m[0].x) + fabs(b->x134 * b->m[1].x) + fabs(b->x138 * b->m[2].x);
    ey = fabs(b->x130 * b->m[0].y) + fabs(b->x134 * b->m[1].y) + fabs(b->x138 * b->m[2].y);
    ez = fabs(b->x130 * b->m[0].z) + fabs(b->x134 * b->m[1].z) + fabs(b->x138 * b->m[2].z);
    prevFace = NULL;
    if (b->xC == 2000.0) {
        r = 0.9f;
        x = b->x130 - r;
        y = 0.4f;
        z = b->x138 - r;
        Vec3f_Set(&pts[0], x, y, z);
        Vec3f_Set(&pts[1], x, y, -z);
        Vec3f_Set(&pts[2], -x, y, -z);
        Vec3f_Set(&pts[3], -x, y, z);
        w = 0.0f;
        Vec3f_Set(&pts[4], w, y, z);
        Vec3f_Set(&pts[5], w, y, -z);
        top = 0.55f;
    } else {
        r = 0.5f;
        x = b->x130 - r;
        w = 0.0f;
        z = b->x138 - r;
        Vec3f_Set(&pts[0], x, w, z);
        Vec3f_Set(&pts[1], x, w, -z);
        Vec3f_Set(&pts[2], -x, w, -z);
        Vec3f_Set(&pts[3], -x, w, z);
        Vec3f_Set(&pts[4], w, w, z);
        Vec3f_Set(&pts[5], w, w, -z);
        top = 0.4f;
    }
    Vec3f_Set(&pts[6], w, top, w);
    diam = r * 2.0;
    for (i = 0; i < 7; i++) {
        Vec3f_TransformTransposed(&tmp, b->m, &b->pos0, &pts[i]);
        A[i] = tmp;
        Vec3f_TransformTransposed(&tmp2, b->m, &b->pos1, &pts[i]);
        B[i] = tmp2;
    }
    box.max.x = center.x + ex;
    box.max.y = center.y + ey;
    box.max.z = center.z + ez;
    box.min.x = center.x - ex;
    box.min.y = center.y - ey;
    box.min.z = center.z - ez;
    nFaces = Track_GetFacesInRadius(&center, b->x120, faces);
    pos0 = b->pos0;
    pos1 = b->pos1;
    Vec3f_Length(&len, &b->angVel1);
    k = len * D_800D2E78 * 9.0 + 0.05;
    iter = 0;
    nHit = 0;
    if (k > 1.0) {
        k = 1.0f;
    }
    do {
        t = 2.0f;
        depth = 0.0f;
        fp = 0;
        bestPt = -1;
        flag2 = 0;
        bestFace = NULL;
        for (j = 0; j < nFaces; j++) {
            p = &faces[j].p;
            n = &p->n;
            Points_ComputeAABB(p, &fbox);
            if (!func_80032FC0(&fbox, &box)) {
                continue;
            }
            for (i = 0; i < 7; i++) {
                p0 = A[i];
                p1 = B[i];
                vp = &p->v[p->count - 1];
                d.x = p0.x - vp->x;
                d.y = p0.y - vp->y;
                d.z = p0.z - vp->z;
                d0 = d.x * n->x + d.y * n->y + d.z * n->z - r;
                d.x = p1.x - vp->x;
                d.y = p1.y - vp->y;
                d.z = p1.z - vp->z;
                d1 = d.x * n->x + d.y * n->y + d.z * n->z - r;
                frac = 0.0f;
                tol = frac;
                inside = 1;
                edgeHit = 0;
                flag3 = 0;
                lim = k;
                if (n->y > 0.94) {
                    lim = 1.3f;
                }
                hitf = 0;
                if (d0 >= 0.0 && d1 < 0.0) {
                    frac = d0 / (d0 - d1);
                    if ((frac < t) | fp) {
                        hitf = 1;
                        tol = r;
                    }
                } else if (d0 < 0.0 && -lim < d0) {
                    if ((depth < -d0) | fp) {
                        tol = 0.15f;
                        frac = 0.0f;
                        hitf = 1;
                    }
                } else if (t > 1.0 && d1 < 0.0 && -diam < d1 && depth < -d1) {
                    f32 s = sqrtf(fabsf(r * r - (d1 + r) * (d1 + r)));
                    frac = 1.0f;
                    hitf = 1;
                    edgeHit = 1;
                    tol = s * 0.9;
                    if (-diam * 0.2 < d1) {
                        flag3 = 1;
                    }
                }
                if (!hitf) {
                    continue;
                }
                hit.x = p0.x + frac * (p1.x - p0.x);
                hit.y = p0.y + frac * (p1.y - p0.y);
                hit.z = p0.z + frac * (p1.z - p0.z);
                found = 0;
                for (e = 0; e < p->count; e++) {
                    Vec3f_Sub(&mv, &p->v[e], vp);
                    Vec3f_Cross(&en, &mv, n);
                    Vec3f_Normalize(&en, &en);
                    Vec3f_Sub(&d, &hit, vp);
                    Vec3f_Dot(&dist, &d, &en);
                    if (tol < dist) {
                        inside = 0;
                        break;
                    }
                    if (flag3) {
                        Vec3f_Sub(&d, &p0, vp);
                        Vec3f_Dot(&dist2, &d, &en);
                        if (tol < dist2) {
                            tmp = en;
                            found = 1;
                            d1 = dist - r;
                        }
                    }
                    vp = &p->v[e];
                }
                if (inside) {
                    t = frac;
                    bestFace = p;
                    bestPt = i;
                    if (edgeHit) {
                        fp = 1;
                        flag2 = 1;
                        depth = -d1;
                        if (!found) {
                            tmp = *n;
                        }
                    } else if (d0 < 0.0) {
                        flag2 = 1;
                        fp = 0;
                        depth = -d0;
                    } else {
                        flag2 = 0;
                        fp = 0;
                        depth = -d1;
                    }
                }
            }
        }
        iter++;
        if (t <= 1.0) {
            pa = &A[bestPt];
            pb = &B[bestPt];
            if (t > 0.01) {
                t = t - 0.01;
            }
            contact.n = bestFace->n;
            if (flag2) {
                if (fp) {
                    contact.n = tmp;
                }
                Vec3f_Scale(&mv, &contact.n, depth + 0.01);
                prevFace = NULL;
                p0 = mv;
            } else {
                mv.x = t * (pb->x - pa->x);
                mv.y = t * (pb->y - pa->y);
                mv.z = t * (pb->z - pa->z);
                if (prevFace != NULL) {
                    Vec3f_Dot(&dd, &bestFace->n, &prevFace->n);
                    if (dd < 0.0) {
                        Vec3f_Cross(&c1, &prevFace->n, &bestFace->n);
                        Vec3f_Normalize(&c1, &c1);
                        Vec3f_Cross(&c2, &c1, &prevFace->n);
                        Vec3f_Dot(&dd2, &c2, &c2);
                        if (dd2 > 0.0) {
                            contact.n = c2;
                            Vec3f_Dot(&dd3, &bestFace->n, &contact.n);
                            if (dd3 < 0.0) {
                                Vec3f_Scale(&contact.n, &contact.n, -1.0f);
                                dd3 = -dd3;
                            }
                            if (dd3 != 0.0) {
                                f32 s = 1.0 / dd3;
                                if (fabs(s) < 10000.0) {
                                    depth *= s;
                                }
                            }
                        }
                    }
                }
                Vec3f_Scale(&p0, &contact.n, depth + 0.01);
                prevFace = bestFace;
            }
            for (i = 0; i < 7; i++) {
                Vec3f_Add(&A[i], &A[i], &mv);
                Vec3f_Add(&B[i], &B[i], &p0);
            }
            Vec3f_Add(&pos0, &pos0, &mv);
            Vec3f_Add(&pos1, &pos1, &p0);
            contact.x1C = 0;
            p1.x = pa->x - bestFace->n.x * r;
            p1.y = pa->y - bestFace->n.y * r;
            p1.z = pa->z - bestFace->n.z * r;
            Vec3f_Sub(&contact.rel, &p1, &pos0);
            Vec3f_Cross(&contact.vel, &b->angVel1, &contact.rel);
            Vec3f_Add(&contact.vel, &contact.vel, &b->vel1);
            Vec3f_Set(&contact.x48, 0.0f, 0.0f, 0.0f);
            Vec3f_Set(&contact.x3C, 0.0f, 0.0f, 0.0f);
            func_800329C0(bestFace->x0, &contact);
            contact.x4 = contact.x4 * 0.1;
            contact.x8 = contact.x8 * 0.1;
            contact.xC = bestFace->x0;
            dup = 0;
            for (i = 0; i < nHit; i++) {
                if (hitFaces[i] == bestFace) {
                    dup = 1;
                    break;
                }
            }
            if (!dup) {
                Physics_AddContact(buf, cnt, &contact, b);
                hitFaces[nHit++] = bestFace;
            }
            Vec3f_Dot(&dd, &contact.n, &contact.n);
            if (iter >= 7) {
                pos1 = pos0;
                break;
            }
        }
    } while (t <= 1.0);
    b->pos1 = pos1;
}
/* agent51: first draft written from the asm; needs --flags -ffast-math (sqrt.s). Same size as
   the original (0xf74). Known differences: frame is 16 bytes smaller (locals 8 bytes short,
   spill area 8 short: Contact size or an extra local), and register allocation in the pts
   setup: original gives w (the 0.0 used in pts[4..6]) f20 shared with -x, y f22, -z f24,
   x f26, z f28, r f30, and top (0.55/0.4) lives in a2 as an int constant (lui/ori a2 in
   each branch, common "mfc1 a1,f20; jal Vec3f_Set; move a3,a1" tail). Writing the pts[6]
   call in each branch instead of using top gets the a2 form but loses 0x18 bytes.
   Later loops look structurally right; registers not checked in detail. */
