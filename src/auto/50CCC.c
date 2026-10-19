#include "common.h"
#include "m2c.h"

typedef struct { s32 a, b; } Pair8;
typedef struct {
    u8 a[9];
    u8 b[9];
    u8 c[14];
    s32 x20;
    s32 x24;
    s64 x28;
    s64 x30;
} SaveEntry;
typedef struct {
    s32 a;
    u8 name[12];
    s32 b;
} SaveGroup;
typedef struct {
    s32 x0;
    s32 x4;
    u8 x8[16];
    SaveEntry e[12];
    SaveGroup g[4];
} SaveData;
extern SaveData D_80097A10;

void Save_CopyToGlobal(SaveData *s) {
    s32 i, j;
    D_80097A10.x0 = s->x0;
    D_80097A10.x4 = s->x4;
    for (i = 0; i < 12; i++) {
        for (j = 0; j < 8; j++) {
            D_80097A10.e[i].a[j] = s->e[i].a[j];
            D_80097A10.e[i].b[j] = s->e[i].b[j];
            D_80097A10.e[i].c[j] = s->e[i].c[j];
        }
        D_80097A10.e[i].x20 = s->e[i].x20;
        D_80097A10.e[i].x28 = s->e[i].x28;
        D_80097A10.e[i].x30 = s->e[i].x30;
        D_80097A10.x8[i] = s->x8[i];
    }
    D_80097A10.g[0].a = s->g[0].a;
    D_80097A10.g[0].b = s->g[0].b;
    for (j = 0; j < 8; j++) D_80097A10.g[0].name[j] = s->g[0].name[j];
    D_80097A10.g[1].a = s->g[1].a;
    D_80097A10.g[1].b = s->g[1].b;
    for (j = 0; j < 8; j++) D_80097A10.g[1].name[j] = s->g[1].name[j];
    D_80097A10.g[2].a = s->g[2].a;
    D_80097A10.g[2].b = s->g[2].b;
    for (j = 0; j < 8; j++) D_80097A10.g[2].name[j] = s->g[2].name[j];
    D_80097A10.g[3].a = s->g[3].a;
    D_80097A10.g[3].b = s->g[3].b;
    for (j = 0; j < 8; j++) D_80097A10.g[3].name[j] = s->g[3].name[j];
}
