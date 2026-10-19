#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f r[3]; } Mtx3;
extern Mtx3 D_800B8A80;
typedef struct { u8 pad0[0x23C]; Vec3f m[3]; } Obj1FD10;
void Mtx2x3_RotateY(Vec3f *, f32);
void func_80039DC4(Vec3f *, f32);
void func_8003A020(Vec3f *, f32);

void Mtx3_SetRotation(Obj1FD10 *obj, Vec3f rot) {
    Mtx3 id = D_800B8A80;

    obj->m[0] = id.r[0];
    obj->m[1] = id.r[1];
    obj->m[2] = id.r[2];
    Mtx2x3_RotateY(obj->m, -rot.x);
    func_80039DC4(obj->m, rot.y);
    func_8003A020(obj->m, rot.z);
}
