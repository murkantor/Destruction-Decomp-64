#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { s16 x, y, z; u8 pad[0xA]; } Vtx;
typedef struct { s32 min[3]; s32 max[3]; } Box;
extern Box D_800BBD94;

Box Gfx_CalcBounds(Gfx *dl) {
    Box b;
    Gfx c;
    Box sub;
    Vtx *v;
    s32 n;
    s32 i;
    u32 w0;
    u32 w1;
    u32 op;

    b = D_800BBD94;
    for (;;) {
        c = *dl++;
        w0 = c.w0;
        w1 = c.w1;
        op = w0;
        switch (w0) {
        case 0xDE000000:
            sub = Gfx_CalcBounds((Gfx *)w1);
            if (b.min[0] > sub.min[0]) {
                b.min[0] = sub.min[0];
            }
            if (b.min[1] > sub.min[1]) {
                b.min[1] = sub.min[1];
            }
            if (b.min[2] > sub.min[2]) {
                b.min[2] = sub.min[2];
            }
            if (b.max[0] < sub.max[0]) {
                b.max[0] = sub.max[0];
            }
            if (b.max[1] < sub.max[1]) {
                b.max[1] = sub.max[1];
            }
            if (b.max[2] < sub.max[2]) {
                b.max[2] = sub.max[2];
            }
            break;
        case 0xDE010000:
            dl = (Gfx *)w1;
            break;
        case 0xDF000000:
            return b;
        default:
            if ((op >> 24) == 1) {
                n = (w0 & 0xFC00) >> 10;
                v = (Vtx *)w1;
                for (i = 0; i < n; i++) {
                    s32 x = v[i].x;
                    s32 y = v[i].y;
                    s32 z = v[i].z;
                    if (x < b.min[0]) {
                        b.min[0] = x;
                    }
                    if (y < b.min[1]) {
                        b.min[1] = y;
                    }
                    if (z < b.min[2]) {
                        b.min[2] = z;
                    }
                    if (b.max[0] < x) {
                        b.max[0] = x;
                    }
                    if (b.max[1] < y) {
                        b.max[1] = y;
                    }
                    if (b.max[2] < z) {
                        b.max[2] = z;
                    }
                }
            }
            break;
        }
    }
}
