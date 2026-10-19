#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { s32 idx; u8 r, g, b, pad; } LitVert;
typedef struct { s32 start; s32 count; } Cell;
typedef struct {
    s32 w;
    s32 h;
    u8 pad8[0x10];
    u16 *idx;
    u8 pad1C[0x10];
    Cell *cells;
} Grid;
typedef struct {
    s16 x, y, z;
    u8 pad6[6];
    u8 r, g, b, pad;
} Vert;
typedef struct {
    Vec3f pos;
    LitVert *saved;
    s32 savedCount;
    LitVert *cur;
    s32 curCount;
    s32 x0;
    s32 x1;
    s32 z0;
    s32 z1;
} Light;
typedef struct { u8 pad0[0x10]; f32 x10; } Obj;
typedef struct { u8 pad0[0xC]; Obj *xC; } ObjHolder;
typedef struct { u8 pad0[0x70]; ObjHolder *x70; } World;
typedef struct { u8 pad0[0x1100]; World *x1100; } Game;

extern Game *gGame;
void func_8001A478(Grid *, s32, s32, s32 *, s32 *);
void *memcpy(void *, const void *, u32);
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);

void Light_UpdateVertexColors(Light *light, Grid *grid, Vert *verts, f32 m[4][4], u8 update)
{
    Obj *o = gGame->x1100->x70->xC;
    Vec3f t;
    Vec3f d;
    s32 x1, z1, x0, z0;
    f32 dd;
    f32 inv;
    f32 scale;
    s32 x, z;
    s32 n;
    s32 k;
    s32 start;
    LitVert *s;
    s32 i;

    scale = o->x10;
    t.x = m[3][0] * scale;
    t.y = m[3][1] * o->x10;
    t.z = m[3][2] * o->x10;
    inv = 1.0 / (scale * 20.0f);
    light->pos = t;
    light->pos.x += m[0][0] * (o->x10 * 20.0f);
    light->pos.y += m[0][1] * (o->x10 * 20.0f);
    light->pos.z += m[0][2] * (o->x10 * 20.0f);
    func_8001A478(grid, light->pos.x + o->x10 * 20.0f, light->pos.z + o->x10 * 20.0f, &x1, &z1);
    func_8001A478(grid, light->pos.x - o->x10 * 20.0f, light->pos.z - o->x10 * 20.0f, &x0, &z0);
    inv = inv * inv;
    if (x0 < 0)
        x0 = 0;
    if (z0 < 0)
        z0 = 0;
    if (x1 >= grid->w)
        x1 = grid->w;
    if (z1 >= grid->h)
        z1 = grid->h;

    s = light->saved;
    light->curCount = 0;
    if (x0 <= x1 && z0 <= z1) {
        for (z = z0; z <= z1; z++) {
            for (x = x0; x <= x1; x++) {
                n = grid->cells[z * grid->w + x].count;
                start = grid->cells[z * grid->w + x].start;
                if (n != 0) {
                    if (x >= light->x0 && x <= light->x1 && z >= light->z0 && z <= light->z1) {
                        while (s->idx != grid->idx[start])
                            s++;
                        memcpy((LitVert *)(light->curCount * 8 + (u32)light->cur), s, n * sizeof(LitVert));
                        s += n;
                        light->curCount += n;
                    } else {
                        for (k = start; k < start + n; k++) {
                            u32 vi = grid->idx[k];
                            light->cur[light->curCount].idx = vi;
                            light->cur[light->curCount].r = verts[vi].r;
                            light->cur[light->curCount].g = verts[vi].g;
                            light->cur[light->curCount].b = verts[vi].b;
                            light->curCount++;
                        }
                    }
                }
            }
        }
    }

    s = light->saved;
    if (light->x0 <= light->x1 && light->z0 <= light->z1) {
        for (z = light->z0; z <= light->z1; z++) {
            for (x = light->x0; x <= light->x1; x++) {
                n = grid->cells[z * grid->w + x].count;
                if (x >= x0 && x <= x1 && z >= z0 && z <= z1) {
                    s += n;
                } else {
                    for (k = 0; k < n; k++, s++) {
                        verts[s->idx].r = s->r;
                        verts[s->idx].g = s->g;
                        verts[s->idx].b = s->b;
                    }
                }
            }
        }
    }

    light->savedCount = light->curCount;
    memcpy(light->saved, light->cur, light->curCount * sizeof(LitVert));

    if (update) {
        s = light->cur;
        for (k = 0; k < light->curCount; s++, k++) {
            s32 vi = s->idx;
            s32 c[3];
            d.x = verts[vi].x;
            d.y = verts[vi].y;
            d.z = verts[vi].z;
            Vec3f_Sub(&d, &d, &light->pos);
            Vec3f_Dot(&dd, &d, &d);
            dd = 255.0f - dd * inv * 255.0f;
            if (0.0f < dd) {
                c[0] = s->r + dd;
                c[1] = s->g + dd;
                c[2] = s->b + dd;
                if (c[0] >= 256)
                    c[0] = 255;
                if (c[1] >= 256)
                    c[1] = 255;
                if (c[2] >= 256)
                    c[2] = 255;
                verts[vi].r = c[0];
                verts[vi].g = c[1];
                verts[vi].b = c[2];
            }
        }
    }
    light->x0 = x0;
    light->x1 = x1;
    light->z0 = z0;
    light->z1 = z1;
}
