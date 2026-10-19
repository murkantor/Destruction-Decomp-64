typedef struct { f32 x, y, z; } Vec3f;
typedef struct { s16 x, y, z; u8 pad[10]; } DVtx;
typedef struct { u8 pad[0x14]; u16 count; u8 pad16[0x24 - 0x16]; DVtx *verts; } Mesh;
typedef struct {
    u8 pad[0x190];
    Mesh *mesh;
    u8 pad194[0xB54 - 0x194];
    u8 xB54[8];
    u8 xB5C[8];
    u8 xB64[8];
} Player;

extern s32 D_800958B4;
extern void func_80032B24(Player *p, Vec3f *pos, Vec3f *out);
extern void Vec3f_Scale(Vec3f *out, Vec3f *v, f32 s);
extern void Vec3f_Sub(Vec3f *out, Vec3f *a, Vec3f *b);
extern void Vec3f_Length(f32 *out, Vec3f *v);
extern void *HeapBlock_Ptr(void *blk);

static inline void deform(DVtx *verts, s32 count, s32 *box, Vec3f *cp, f32 strength)
{
    DVtx *v = verts;
    Vec3f center = *cp;
    Vec3f unused;
    s32 i;
    Vec3f p;
    Vec3f d;
    f32 len;
    f32 f;

    for (i = 0; i < count; i++) {
        if (v->x <= box[1] && v->x >= box[0] && v->z <= box[3] && v->z >= box[2]) {
            p.x = v->x;
            p.y = v->y;
            p.z = v->z;
            Vec3f_Sub(&d, &center, &p);
            Vec3f_Length(&len, &d);
            len = len * 0.00111111111111111111;
            f = strength - len;
            v->x = v->x * f;
            v->z = v->z * f;
        }
        v++;
    }
}

void func_80028A4C(Player *p, s32 n, Vec3f pos, s32 type)
{
    s32 box[4];
    Vec3f c;
    f32 strength;

    if (n < 2) {
        return;
    }
    func_80032B24(p, &pos, &c);
    Vec3f_Scale(&c, &c, 100.0f);
    box[0] = c.x - 50.0f;
    box[1] = c.x + 50.0f;
    box[2] = c.z - 50.0f;
    box[3] = c.z + 50.0f;
    if (type == 5 || type == 2) {
        n += 10;
    }
    strength = 0.95 - n * 0.0015;
    deform(p->mesh->verts, p->mesh->count, box, &c, strength);
    deform(HeapBlock_Ptr(p->xB54), 8, box, &c, strength);
    if (D_800958B4 == 2 || D_800958B4 == 11) {
        deform(HeapBlock_Ptr(p->xB5C), 8, box, &c, strength);
        deform(HeapBlock_Ptr(p->xB64), 8, box, &c, strength);
    }
}
