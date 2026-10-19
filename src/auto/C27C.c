#include "common.h"
#include "m2c.h"

typedef struct { f32 pos[3]; u8 padC[0x18]; f32 rot[3][3]; } Frame;

void Frame_ToMatrix(f32 m[4][4], u8 *o, s32 flag) {
    Frame *f;
    s32 i;

    if (!flag) {
        f = (Frame *)(o + 0x218);
    } else {
        f = (Frame *)(o + 0x290);
    }
    for (i = 0; i < 3; i++) {
        m[i][0] = f->rot[i][0];
        m[i][1] = f->rot[i][1];
        m[i][2] = f->rot[i][2];
        m[i][3] = 0;
    }
    m[3][0] = f->pos[0];
    m[3][1] = f->pos[1];
    m[3][2] = f->pos[2];
    m[3][3] = 1.0f;
}
