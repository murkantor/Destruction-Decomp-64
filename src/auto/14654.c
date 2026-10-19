#include "common.h"
#include "m2c.h"

typedef struct {
    s32 w[5];
} Blk14;

typedef struct {
    s32 x0;
    Blk14 x4;
    u8 x18;
    u8 x19;
    u8 x1A;
    u8 pad1B;
    f32 x1C;
    s32 x20;
    s32 x24;
} Entry28;

extern Entry28 D_80124B20[];
extern s32 D_80131F28;

void func_80013A54(s32 a0, Blk14 *a1, u8 a2, u8 a3, s16 a4) {
    D_80124B20[D_80131F28].x0 = a0;
    D_80124B20[D_80131F28].x4.w[0] = a1->w[0];
    D_80124B20[D_80131F28].x4.w[1] = a1->w[1];
    D_80124B20[D_80131F28].x4.w[2] = a1->w[2];
    D_80124B20[D_80131F28].x4.w[3] = a1->w[3];
    D_80124B20[D_80131F28].x4.w[4] = a1->w[4];
    D_80124B20[D_80131F28].x18 = a2;
    D_80124B20[D_80131F28].x19 = a2;
    D_80124B20[D_80131F28].x1A = a3;
    D_80124B20[D_80131F28].x1C = a4 * 0.001;
    D_80124B20[D_80131F28].x20 = 0;
    if (D_80124B20[D_80131F28].x1C < 0.001) {
        D_80124B20[D_80131F28].x1C = 0.001f;
    }
    D_80131F28++;
}
