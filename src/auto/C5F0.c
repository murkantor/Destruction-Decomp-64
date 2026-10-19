#include "common.h"
#include "m2c.h"

typedef struct { f32 m[4][4]; } Mtx4;
typedef struct { f32 x0, x4, x8; u8 padC[0x18]; f32 rot[3][3]; u8 pad[0x1BC - 0x48]; } Node;
typedef struct { u8 pad[0x390]; Node a[1]; } Model;
extern void Mtx4_Mul(Mtx4 *, s32, s32);
void Mtx_FromRotPos(s32 a0, s32 a1, s32 m, s32 idx, s32 sel) {
    Node *n;
    Mtx4 mtx;
    s32 i;
    if (sel == 0) {
        n = (Node *)(idx * 0x1BC + m + 0x390);
    } else {
        n = (Node *)(idx * 0x1BC + m + 0x408);
    }
    for (i = 0; i < 3; i++) {
        mtx.m[i][0] = n->rot[i][0];
        mtx.m[i][1] = n->rot[i][1];
        mtx.m[i][2] = n->rot[i][2];
        mtx.m[i][3] = 0;
    }
    mtx.m[3][0] = n->x0;
    mtx.m[3][1] = n->x4;
    mtx.m[3][2] = n->x8;
    mtx.m[3][3] = 1.0f;
    Mtx4_Mul(&mtx, a1, a0);
}
