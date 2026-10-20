#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 m[3][3]; } Mtx3;
typedef struct {
    u8 pad0[4];
    u8 w;
    u8 h;
    u8 pad6[0xE];
    f32 x14;
    f32 x18;
    f32 x1C;
    f32 x20;
} Grid;
typedef struct { u8 pad[0xC]; Grid *grid; } Track;
typedef struct { u8 pad[0x70]; Track *track; } Race;
typedef struct { u8 pad[0x1100]; Race *race; } Game;
typedef struct { u8 pad[0x18]; s32 x18; } Model;
typedef struct {
    u8 pad0[4];
    f32 radius;
    u8 pad8[0xC];
    Vec3f offs;
    s32 x20;
    u8 pad24[0xC];
    Model *x30;
} Def;
typedef struct {
    s32 id;
    Vec3f pos;
    Mtx3 rot;
} Entry;
typedef struct {
    s16 type;
    u8 pad2[2];
    Vec3f pos;
    Mtx3 rot;
    Vec3f center;
    u8 pad40[0x18];
    s32 x58;
    u8 pad5C[4];
    s32 x60;
    s32 x64;
    s32 x68;
    Def *def;
    s32 x70;
    Model *x74;
    u8 x78[0x28];
} Obj;

extern Game *gGame;
extern s32 D_800960B4;
extern Obj *D_800960B0;
extern u32 Res_GetSize(s32);
extern void *Mem_AllocAligned(s32, s32, s32);
extern void *Mem_Alloc(s32);
extern void Res_Load(s32, void *);
extern Def *Res_Get(s32);
extern void Grid_AddObjToCellsInRect(void *obj, f32 x0, f32 z0, f32 x1, f32 z1);
extern void ModelInst_Init(void *, Model *);
extern void Vec3f_Scale(Vec3f *, Vec3f *, f32);
extern void ObjList_Add(Obj *);

static inline void Clampf(f32 *v, f32 lo, f32 hi) {
    if (*v < lo) {
        *v = lo;
    } else if (hi < *v) {
        *v = hi;
    }
}

Entry *Track_LoadProps(s32 res) {
    Grid *grid = gGame->race->track->grid;
    u32 size;
    Entry *data;
    Entry *e;
    Obj *o;
    Def *def;
    s32 i;
    f32 minx, minz, maxx, maxz;
    f32 r;
    f32 x0, x1, z0, z1;

    size = Res_GetSize(res);
    data = Mem_AllocAligned(size, 0x3F, 0);
    D_800960B4 = size / sizeof(Entry);
    D_800960B0 = Mem_Alloc(D_800960B4 * sizeof(Obj));
    Res_Load(res, data);
    /* an id and 12 floats a record: GCC computes the centre below from the
       values it copied into o->rot and o->pos, still in registers, so they must be
       in host order before the copy */
    PORT_CONVERT(data, size / 4, 4);
    minx = grid->x14 + 1.0f;
    minz = grid->x18 + 1.0f;
    maxx = (grid->w * grid->x1C + grid->x14) - 1.0f;
    maxz = (grid->h * grid->x20 + grid->x18) - 1.0f;
    for (i = 0; i < D_800960B4; i++) {
        e = &data[i];
        o = &D_800960B0[i];
        def = Res_Get(e->id);
        o->type = 0x1F;
        o->def = def;
        o->x68 = def->x20;
        o->x64 = 0;
        o->x58 = 0;
        o->rot = e->rot;
        o->pos = e->pos;
        o->x60 = 1;
        o->center.x = o->rot.m[0][0] * def->offs.x + o->rot.m[1][0] * def->offs.y + o->rot.m[2][0] * def->offs.z + o->pos.x;
        o->center.y = o->rot.m[0][1] * def->offs.x + o->rot.m[1][1] * def->offs.y + o->rot.m[2][1] * def->offs.z + o->pos.y;
        o->center.z = o->rot.m[0][2] * def->offs.x + o->rot.m[1][2] * def->offs.y + o->rot.m[2][2] * def->offs.z + o->pos.z;
        r = o->def->radius;
        x0 = o->center.x - r - 10.0f;
        x1 = o->center.x + r + 10.0f;
        z0 = o->center.z - r - 10.0f;
        z1 = o->center.z + r + 10.0f;
        Clampf(&x0, minx, maxx);
        Clampf(&x1, minx, maxx);
        Clampf(&z0, minz, maxz);
        Clampf(&z1, minz, maxz);
        Grid_AddObjToCellsInRect(o, x0, z0, x1, z1);
        o->x74 = def->x30;
        o->x70 = def->x30->x18;
        ModelInst_Init(o->x78, o->x74);
        if (o->x68 == 1) {
            Vec3f_Scale((Vec3f *)o->rot.m[0], (Vec3f *)o->rot.m[0], 0.75f);
            Vec3f_Scale((Vec3f *)o->rot.m[1], (Vec3f *)o->rot.m[1], 0.75f);
            Vec3f_Scale((Vec3f *)o->rot.m[2], (Vec3f *)o->rot.m[2], 0.75f);
        }
        if (e->id == 0xC991) {
            Vec3f_Scale((Vec3f *)o->rot.m[0], (Vec3f *)o->rot.m[0], 1.4f);
            Vec3f_Scale((Vec3f *)o->rot.m[1], (Vec3f *)o->rot.m[1], 1.4f);
            Vec3f_Scale((Vec3f *)o->rot.m[2], (Vec3f *)o->rot.m[2], 1.4f);
        }
        ObjList_Add(o);
    }
    return data;
}
