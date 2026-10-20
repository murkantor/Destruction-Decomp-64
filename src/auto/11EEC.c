#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { f32 x, y, z; } Vec3f;
typedef struct { s16 x, y, z; u16 flag; s16 s, t; u8 r, g, b, a; } Vtx;
typedef struct {
    void *free;
    u32 itemSize;
    void *base;
    u32 size;
    u32 sizeFree;
} Pool;
typedef struct Particle {
    struct Particle *next;
    Vec3f pos;
    u8 pad10[0x28 - 0x10];
    f32 scale;
    u8 pad2C[4];
    u8 x30;
    u8 x31;
    u8 x32;
    u8 x33;
    u8 x34;
    u8 r;
    u8 g;
    u8 b;
    u8 pad38;
    u8 x39;
    u8 pad3A[2];
    u32 dl;
} Particle;
typedef struct {
    Particle *list;
    u8 pad4[0xC];
    s32 type;
    u32 dl;
    s32 blend;
    u8 pad1C[4];
    s32 x20;
    s32 x24;
} PSys;
typedef struct { Vec3f pos; s32 s; s32 t; } Corner;

extern Pool gParticlePool;
extern void Vec3f_SetLength(Vec3f *out, Vec3f *v, f32 len);
extern Gfx *Particle_LoadTexFrame(PSys *sys, s32 tex, Gfx *gfx);
extern void *Pool_Alloc(Pool *pool);
extern void func_80057334(void *p);
extern void guPosition(void *m, f32 r, f32 p, f32 h, f32 s, f32 x, f32 y, f32 z);

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *Particles_Draw(PSys *sys, f32 (*m)[4], Gfx *gfx)
{
    Vec3f right;
    Vec3f up;
    Corner c[4];
    Vec3f pos;
    Particle *p;
    s32 alpha;
    s32 tex;
    s32 j;
    f32 scale;
    Vtx *mem;
    s32 lastTex;
    Vtx *vv;
    Corner *cc;
    f32 s;
    f32 rx, ry, rz;

    if (sys->list == NULL) {
        return gfx;
    }
    right.x = m[0][0];
    right.y = m[1][0];
    right.z = m[2][0];
    up.x = m[0][1];
    up.y = m[1][1];
    up.z = m[2][1];
    Vec3f_SetLength(&right, &right, 1.0f);
    Vec3f_SetLength(&up, &up, 1.0f);
    GFX2(gfx, 0xD9FFFFFF, 5);
    if (sys->blend != 0) {
        if (sys->blend == 4) {
            GFX2(gfx, 0xFC41C283, 0x5587DFEF);
        }
    } else {
        GFX2(gfx, 0xFC40C281, 0xFF87FFFF);
    }
    if (sys->type >= 0) if (sys->type < 3) {
        GFX2(gfx, 0xFA000000, 0xFFFFFFFF);
        GFX2(gfx, 0xFB000000, 0);
        GFX2(gfx, 0xE200001C, 0x504A50);
        gfx = Particle_LoadTexFrame(sys, 0, gfx);
    }
    switch (sys->type) {
        case 0:
            c[0].pos.x = up.x;
            c[0].pos.y = up.y;
            c[0].pos.z = up.z;
            c[0].s = (sys->x20 << 4) - 8;
            c[0].t = 16;
            c[1].pos.x = -right.x - up.x;
            c[1].pos.y = -right.y - up.y;
            c[1].pos.z = -right.z - up.z;
            c[1].s = 16;
            c[1].t = (sys->x24 << 5) - 16;
            c[2].pos.x = right.x - up.x;
            c[2].pos.y = right.y - up.y;
            c[2].pos.z = right.z - up.z;
            c[2].s = (sys->x20 << 5) - 16;
            c[2].t = (sys->x24 << 5) - 16;
            break;
        case 1:
            c[0].pos.x = -right.x + up.x;
            c[0].pos.y = -right.y + up.y;
            c[0].pos.z = -right.z + up.z;
            c[0].s = 16;
            c[0].t = 16;
            c[1].pos.x = -right.x - up.x;
            c[1].pos.y = -right.y - up.y;
            c[1].pos.z = -right.z - up.z;
            c[1].s = 16;
            c[1].t = (sys->x24 << 5) - 16;
            c[2].pos.x = right.x - up.x;
            c[2].pos.y = right.y - up.y;
            c[2].pos.z = right.z - up.z;
            c[2].s = (sys->x20 << 5) - 16;
            c[2].t = (sys->x24 << 5) - 16;
            c[3].pos.x = right.x + up.x;
            c[3].pos.y = right.y + up.y;
            c[3].pos.z = right.z + up.z;
            c[3].s = (sys->x20 << 5) - 16;
            c[3].t = 16;
            break;
        case 2: {
            f64 rx2 = right.x * 0.111111111111111105;
            f64 ux2 = up.x * 3.0;
            f64 ry2 = right.y * 0.111111111111111105;
            f64 uy2 = up.y * 3.0;
            f64 rz2 = right.z * 0.111111111111111105;
            f64 uz2 = up.z * 3.0;
            c[0].pos.x = -rx2 + ux2;
            c[0].pos.y = -ry2 + uy2;
            c[0].pos.z = -rz2 + uz2;
            c[0].s = 16;
            c[0].t = 16;
            c[1].pos.x = -rx2 - ux2;
            c[1].pos.y = -ry2 - uy2;
            c[1].pos.z = -rz2 - uz2;
            c[1].s = 16;
            c[1].t = (sys->x24 << 5) - 16;
            c[2].pos.x = rx2 - ux2;
            c[2].pos.y = ry2 - uy2;
            c[2].pos.z = rz2 - uz2;
            c[2].s = (sys->x20 << 5) - 16;
            c[2].t = (sys->x24 << 5) - 16;
            c[3].pos.x = rx2 + ux2;
            c[3].pos.y = ry2 + uy2;
            c[3].pos.z = rz2 + uz2;
            c[3].s = (sys->x20 << 5) - 16;
            c[3].t = 16;
            break;
        }
    }
    lastTex = -1;
    for (p = sys->list; p != NULL; p = p->next) {
        if (gParticlePool.sizeFree <= 0x2000) {
            return gfx;
        }
        pos = p->pos;
        alpha = p->x30 * p->x31;
        if (p->x32 < alpha) {
            alpha = p->x32;
        }
        scale = p->scale;
        if (scale == 0.0f || p->x30 == 0) {
            continue;
        }
        mem = Pool_Alloc(&gParticlePool);
        if (mem == NULL) {
            return gfx;
        }
        switch (sys->type) {
            case 0: {
                Vtx *vtx;
                tex = (p->x39 - p->x30) * (p->x34 - p->x33 + 1) / p->x39 + p->x33;
                vtx = mem;
                if (tex != lastTex) {
                    lastTex = tex;
                    gfx = Particle_LoadTexFrame(sys, lastTex, gfx);
                }
                for (j = 0; j < 3; j++) {
                    mem[j].x = pos.x + scale * c[j].pos.x;
                    mem[j].y = pos.y + scale * c[j].pos.y;
                    mem[j].z = pos.z + scale * c[j].pos.z;
                    mem[j].s = c[j].s;
                    mem[j].t = c[j].t;
                    mem[j].r = p->r;
                    mem[j].g = p->g;
                    mem[j].b = p->b;
                    mem[j].a = alpha;
                }
                GFX2(gfx, 0x01003006, vtx);
                GFX2(gfx, 0x05000204, 0);
                break;
            }
            case 1:
            case 2: {
                Vtx *vtx;
                tex = (p->x39 - p->x30) * (p->x34 - p->x33 + 1) / p->x39 + p->x33;
                vtx = mem;
                if (tex != lastTex) {
                    lastTex = tex;
                    gfx = Particle_LoadTexFrame(sys, lastTex, gfx);
                }
                for (j = 0; j < 4; j++) {
                    mem[j].x = pos.x + scale * c[j].pos.x;
                    mem[j].y = pos.y + scale * c[j].pos.y;
                    mem[j].z = pos.z + scale * c[j].pos.z;
                    mem[j].s = c[j].s;
                    mem[j].t = c[j].t;
                    mem[j].r = p->r;
                    mem[j].g = p->g;
                    mem[j].b = p->b;
                    mem[j].a = alpha;
                }
                GFX2(gfx, 0x01004008, vtx);
                GFX2(gfx, 0x06000204, 0x406);
                break;
            }
            case 3:
            case 4:
                s = p->x30 * 0.05f;
                rx = s * ((f32)(((u32)p & 0xFF00) >> 8) - 127.0f);
                ry = s * ((f32)(((u32)p & 0xFF0) >> 4) - 127.0f);
                rz = s * ((f32)((u32)p & 0xFF) - 127.0f);
                guPosition(mem, rx, ry, rz, p->scale, pos.x, pos.y, pos.z);
                GFX2(gfx, 0xDA380003, mem);
                GFX2(gfx, 0xDE000000, p->dl ? p->dl : sys->dl);
                break;
        }
        func_80057334(mem);
    }
    return gfx;
}
