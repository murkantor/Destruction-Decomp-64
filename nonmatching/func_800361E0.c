typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } TrackPt;
typedef struct { u8 pad[0x30]; TrackPt *pts; u8 pad34[4]; s32 count; } Track;
typedef struct { u8 pad[0x1100]; Track *track; } Game;
typedef struct { u8 pad[0x1A0]; u8 x1A0; u8 pad1A1[0x1BC - 0x1A1]; } Sub;
typedef struct {
    u8 pad[0x28];
    s32 x28;
    u8 pad2C[0x58 - 0x2C];
    s32 x58;
    u8 pad5C[0x2A8 - 0x5C];
    Vec3f x2A8;
    u8 pad2B4[0x370 - 0x2B4];
    Sub subs[4];
    u8 padA60[0xBAC - 0xA60];
    u8 xBAC;
    u8 padBAD[3];
    f32 xBB0;
    f32 xBB4;
    f32 xBB8;
    s32 xBBC;
    f32 xBC0;
    s32 xBC4;
    f32 xBC8;
    f32 xBCC;
    s32 xBD0;
} Obj;

extern Game *gGame;
s32 rand(void);
void Vec3f_Set(Vec3f *, f32, f32, f32);

void func_800361E0(Obj *o) {
    s32 n = gGame->track->count;
    s32 i;
    TrackPt *p;

    o->xBBC = o->x28 + o->x58 * 3;
    if (o->xBBC >= n) {
        o->xBBC -= n;
    } else if (o->xBBC < 0) {
        o->xBBC += n;
    }
    o->xBAC = 1;
    o->xBC4 = 0;
    o->xBD0 = 0;
    o->xBC8 = 1.0f;
    o->xBCC = 0.1f;
    o->xBC0 = rand() * (1.0f / 32767.0f) * 0.64 + 0.18;
    Vec3f_Set(&o->x2A8, 0.0f, 0.0f, 0.0f);
    for (i = 3; i >= 0; i--) {
        o->subs[i].x1A0 = 0;
    }
    p = gGame->track->pts;
    p += o->xBBC;
    o->xBB0 = p->a.x * o->xBC0 + p->b.x * (1.0 - o->xBC0);
    o->xBB4 = p->a.y * o->xBC0 + p->b.y * (1.0 - o->xBC0);
    o->xBB8 = p->a.z * o->xBC0 + p->b.z * (1.0 - o->xBC0);
    o->xBB4 += 2.0;
}
