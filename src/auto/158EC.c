#include "common.h"
#include "m2c.h"

typedef struct { f32 m[4][4]; } Mtx4;
extern f64 D_800B8680;
extern void ModelInst_SetMatrix(s32, Mtx4 *);
typedef struct { f32 x, y, z; } Vec3;
void Mtx_FromRotPosScaled(s32 a0, Vec3 pos, f32 (*rot)[3]) {
    Mtx4 mtx;
    u8 unused[0x100];
    s32 i, j;
    for (i = 0; i < 3; i++) {
        mtx.m[i][0] = rot[i][0];
        mtx.m[i][1] = rot[i][1];
        mtx.m[i][2] = rot[i][2];
        mtx.m[i][3] = 0;
    }
    mtx.m[3][0] = pos.x;
    mtx.m[3][1] = pos.y;
    mtx.m[3][2] = pos.z;
    mtx.m[3][3] = 1.0f;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            mtx.m[i][j] *= D_800B8680;
        }
    }
    ModelInst_SetMatrix(a0, &mtx);
}
