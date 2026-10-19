typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[4]; s32 x4; u8 pad8[0x94 - 0x8]; s32 x94; } Obj;
extern void Vec3f_Set(Vec3f *, s32, s32, s32);
extern void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
extern void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
extern void Vec3f_Length(f32 *, Vec3f *);
extern void Vec3f_SetLength(Vec3f *, Vec3f *, f32);
extern void func_80010B28(Obj *, Vec3f, Vec3f, Vec3f, f32, f32, u16, u8, s32, s32, s32, s32);

void func_80016C1C(Obj *obj, Vec3f *from, Vec3f *to, Vec3f vel, f32 a, f32 b, s32 c, s32 d, s32 mode) {
    Vec3f p;
    Vec3f diff;
    Vec3f step;
    f32 len[2];
    struct { s32 x0; Vec3f pos; Vec3f v; u8 pad[0x14]; } q;
    Vec3f zero;
    f32 t;

    Vec3f_Sub(&diff, to, from);
    Vec3f_Length(len, &diff);
    Vec3f_SetLength(&step, &diff, 6.0f);
    p = *from;
    for (t = 0.0f; t < len[0]; t += 6.0) {
        q.pos = p;
        q.v = vel;
        if (obj->x4 < obj->x94) {
            Vec3f_Set(&zero, 0, 0, 0);
            switch (mode) {
            case 0:
                func_80010B28(obj, q.pos, q.v, zero, a, b, c, d, 0xFF, 0, 0, 0);
                break;
            case 1:
                func_80010B28(obj, q.pos, q.v, zero, a, b, c, d, 0xFF, 0, 3, 3);
                break;
            }
        }
        Vec3f_Add(&p, &p, &step);
    }
}
