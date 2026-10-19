#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xA4]; f32 pos[3]; u8 padB0[0x18]; f32 rot[3][3]; u8 padEC[0x30]; f32 scale; } Body;

void Frame_ToScaledMatrix(f32 m[4][4], u8 *o) {
    Body *b = (Body *)(o + 0x1EC);
    s32 i;

    for (i = 0; i < 3; i++) {
        m[i][0] = b->rot[i][0] * b->scale;
        m[i][1] = b->rot[i][1] * b->scale;
        m[i][2] = b->rot[i][2] * b->scale;
        m[i][3] = 0;
    }
    m[3][0] = b->pos[0];
    m[3][1] = b->pos[1];
    m[3][2] = b->pos[2];
    m[3][3] = 1.0f;
}
