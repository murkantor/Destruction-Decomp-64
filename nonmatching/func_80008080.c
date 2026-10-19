typedef struct { f32 x, y, z; } Vec3f;

typedef struct {
    u8 pad0[0x30];
    s32 x30;
} Car;

typedef struct {
    u8 pad0[0x74];
    Car *x74[1];
} Race;

typedef struct {
    u8 pad0[0x1058];
    u64 x1058;
    u8 pad1060[0x1100 - 0x1060];
    Race *x1100;
} Game;

typedef struct {
    s16 x0;
    s16 x2;
    s32 x4;
    u8 pad8[0xC - 0x8];
    s32 xC;
    s16 x10;
    u8 pad12[0x14 - 0x12];
    s32 x14;
    Vec3f x18;
    Vec3f x24;
    f32 x30;
    s32 x34;
    u64 x38;
    s32 x40;
    u8 x44;
    u8 x45;
    u8 pad46[0x48 - 0x46];
    Car *x48;
    s32 x4C;
    u8 x50;
} Obj;

extern Game *gGame;
extern s32 D_8007EECC;
extern u64 D_80131F90[];
extern s32 D_80132164[];
extern u8 D_8007EE48[];
s16 func_80064D70(void);
u64 Time_Get(u64 *);
Vec3f func_8001FBA8(Car *);
Vec3f func_8001FB88(Car *);

s16 func_80008080(s32 idx, s32 kind, Obj *obj) {
    Car *car = gGame->x1100->x74[idx];
    u64 *t;
    u64 v;

    obj->x0 = 0x3F4;
    obj->x4 = 0;
    obj->x2 = func_80064D70();
    t = &D_80131F90[kind];
    obj->x10 = kind;
    obj->x14 = D_8007EECC;
    if (*t != (u64)-1) {
        v = *t + Time_Get(&gGame->x1058);
    } else {
        v = -1;
    }
    obj->x38 = v;
    obj->x18 = func_8001FBA8(car);
    obj->x24 = func_8001FB88(car);
    obj->x30 = D_80132164[kind];
    obj->x44 = 1;
    obj->x34 = 0;
    obj->x40 = 0;
    obj->x45 = 0;
    obj->x4C = -1;
    if (car->x30 == 0) {
        obj->x48 = car;
        obj->xC = 2;
    } else {
        obj->x48 = NULL;
        obj->xC = 3;
    }
    obj->x50 = D_8007EE48[obj->xC];
    return obj->x2;
}
