#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    s16 fw;
    s16 fh;
} Viewport;

typedef struct {
    u8 pad0[0x6C];
    s32 x6C;
    s32 x70;
} Screen;

extern Viewport D_801260D8[];
extern u8 *D_8011EEF0;
void func_8005B5BC(void *, s32, s32, s32, s32, s32, s32);

void func_800627F0(Screen *s, s32 n, u8 flip) {
    s16 hh = s->x70 / 2;
    s16 hw = s->x6C / 2;
    s32 i;
    f32 f;

    switch (n) {
        case 1:
            D_801260D8[0].x = 0;
            D_801260D8[0].y = 0;
            D_801260D8[0].w = s->x6C;
            D_801260D8[0].h = s->x70;
            D_801260D8[0].fw = s->x6C;
            D_801260D8[0].fh = s->x70;
            break;
        case 2:
            D_801260D8[0].x = 0;
            D_801260D8[0].y = 0;
            D_801260D8[0].w = s->x6C;
            D_801260D8[0].h = hh;
            D_801260D8[0].fw = s->x6C;
            D_801260D8[0].fh = s->x70;
            D_801260D8[1].x = 0;
            D_801260D8[1].y = hh;
            D_801260D8[1].w = s->x6C;
            D_801260D8[1].h = hh;
            D_801260D8[1].fw = s->x6C;
            D_801260D8[1].fh = s->x70;
            break;
        case 3:
            D_801260D8[0].x = 0;
            D_801260D8[0].y = 0;
            D_801260D8[0].w = hw;
            D_801260D8[0].h = hh;
            D_801260D8[0].fw = s->x6C;
            D_801260D8[0].fh = s->x70;
            D_801260D8[1].x = hw;
            D_801260D8[1].y = 0;
            D_801260D8[1].w = hw;
            D_801260D8[1].h = hh;
            D_801260D8[1].fw = s->x6C;
            D_801260D8[1].fh = s->x70;
            D_801260D8[2].x = 0;
            D_801260D8[2].y = hh;
            D_801260D8[2].w = hw;
            D_801260D8[2].h = hh;
            D_801260D8[2].fw = s->x6C;
            D_801260D8[2].fh = s->x70;
            break;
        case 4:
            D_801260D8[0].x = 0;
            D_801260D8[0].y = 0;
            D_801260D8[0].w = hw;
            D_801260D8[0].h = hh;
            D_801260D8[0].fw = s->x6C;
            D_801260D8[0].fh = s->x70;
            D_801260D8[1].x = hw;
            D_801260D8[1].y = 0;
            D_801260D8[1].w = hw;
            D_801260D8[1].h = hh;
            D_801260D8[1].fw = s->x6C;
            D_801260D8[1].fh = s->x70;
            D_801260D8[2].x = 0;
            D_801260D8[2].y = hh;
            D_801260D8[2].w = hw;
            D_801260D8[2].h = hh;
            D_801260D8[2].fw = s->x6C;
            D_801260D8[2].fh = s->x70;
            D_801260D8[3].x = hw;
            D_801260D8[3].y = hh;
            D_801260D8[3].w = hw;
            D_801260D8[3].h = hh;
            D_801260D8[3].fw = s->x6C;
            D_801260D8[3].fh = s->x70;
            break;
    }
    for (i = 0; i < n; i++) {
        D_801260D8[i].x0 = 0;
        D_801260D8[i].x4 = 0;
        D_801260D8[i].x8 = 0;
        f = D_801260D8[i].fw;
        if (flip) {
            f = -f;
        }
        func_8005B5BC(D_8011EEF0 + 0x198 + i * 0x1C, D_801260D8[i].x, D_801260D8[i].y, D_801260D8[i].w,
                      D_801260D8[i].h, f, D_801260D8[i].fh);
    }
}
