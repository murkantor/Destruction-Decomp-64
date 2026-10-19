#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { s16 x, y, z; u16 flag; s16 s, t; u8 r, g, b, a; } Vtx;
typedef struct { u8 pad[0x18]; } HeapBlock;
typedef struct { u16 v[4][2]; } UV4;
typedef struct {
    u8 pad0[0x128];
    Vec3f x128[2];
    Vec3f x140[4];
    Vec3f x170[2];
    Vec3f x188[8];
} CarStats;
typedef struct {
    u8 pad0[0xB3C];
    HeapBlock xB3C;
} CarA;
typedef struct {
    u8 pad0[0xB3C];
    u8 xB3C[8];
    u8 xB44[8];
    u8 xB4C[8];
    u8 xB54[8];
    u8 xB5C[8];
    u8 xB64[8];
} Car;

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

extern UV4 D_800B89F0;
extern UV4 D_800B8A00;
extern u8 gMainHeap[];
extern u8 D_8008F930[];
extern u8 D_80090930[];
extern s32 D_800958B4;
void Heap_Alloc(void *, void *, s32, s32, s32);
void *HeapBlock_Ptr(void *);

void Car_BuildLightDisplayLists(Car *car, CarStats *s) {
    UV4 uvA = D_800B89F0;
    UV4 uvB = D_800B8A00;
    Gfx *gfx;
    Vtx *vtx;
    s32 i;
    s32 j;

    Heap_Alloc(gMainHeap, car->xB3C, 0xC0, 7, 0);
    gfx = HeapBlock_Ptr(car->xB3C);
    Heap_Alloc(gMainHeap, car->xB54, 0x80, 7, 0);
    vtx = HeapBlock_Ptr(car->xB54);
    GFX2(gfx, 0xD9FFF9FF, 0);
    GFX2(gfx, 0xD9FFFFFF, 0x401);
    GFX2(gfx, 0xE3001001, 0);
    GFX2(gfx, 0xFD18001F, D_8008F930);
    GFX2(gfx, 0xF5181000, 0x07017C5F);
    GFX2(gfx, 0xE6000000, 0);
    GFX2(gfx, 0xF4000000, 0x0707C07C);
    GFX2(gfx, 0xE7000000, 0);
    GFX2(gfx, 0xF5181000, 0x00017C5F);
    GFX2(gfx, 0xF2000000, 0x0007C07C);
    GFX2(gfx, 0xE200001C, 0x005049D8);
    GFX2(gfx, 0x01008010, vtx);
    GFX2(gfx, 0x05000204, 0);
    GFX2(gfx, 0x05040600, 0);
    GFX2(gfx, 0x05080A0C, 0);
    GFX2(gfx, 0x050C0E08, 0);
    GFX2(gfx, 0xDF000000, 0);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            vtx->x = (s->x128[i].x + s->x140[j].x) * 100.0;
            vtx->y = (s->x128[i].y + s->x140[j].y) * 100.0;
            vtx->z = (s->x128[i].z + s->x140[j].z) * 100.0;
            vtx->flag = 0;
            vtx->s = uvA.v[j][0];
            vtx->t = uvA.v[j][1];
            vtx->r = 0x82;
            vtx->g = 0;
            vtx->b = (i == 0) ? 0xB : 0xF5;
            vtx->a = 0;
            vtx++;
        }
    }
    if (D_800958B4 == 2 || D_800958B4 == 11) {
        Heap_Alloc(gMainHeap, car->xB44, 0xC0, 7, 0);
        gfx = HeapBlock_Ptr(car->xB44);
        Heap_Alloc(gMainHeap, car->xB5C, 0x80, 7, 0);
        vtx = HeapBlock_Ptr(car->xB5C);
        GFX2(gfx, 0xD9FFF9FF, 0);
        GFX2(gfx, 0xD9FFFFFF, 0x401);
        GFX2(gfx, 0xE3001001, 0);
        GFX2(gfx, 0xFD18001F, D_8008F930);
        GFX2(gfx, 0xF5181000, 0x07017C5F);
        GFX2(gfx, 0xE6000000, 0);
        GFX2(gfx, 0xF4000000, 0x0707C07C);
        GFX2(gfx, 0xE7000000, 0);
        GFX2(gfx, 0xF5181000, 0x00017C5F);
        GFX2(gfx, 0xF2000000, 0x0007C07C);
        GFX2(gfx, 0xE200001C, 0x005049D8);
        GFX2(gfx, 0x01008010, vtx);
        GFX2(gfx, 0x05000204, 0);
        GFX2(gfx, 0x05040600, 0);
        GFX2(gfx, 0x05080A0C, 0);
        GFX2(gfx, 0x050C0E08, 0);
        GFX2(gfx, 0xDF000000, 0);
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 4; j++) {
                vtx->x = (s->x128[i].x + s->x140[j].x * 0.4) * 100.0;
                vtx->y = (s->x128[i].y + s->x140[j].y * 0.4) * 100.0;
                vtx->z = (s->x128[i].z + s->x140[j].z * 0.4) * 100.0;
                vtx->flag = 0;
                vtx->s = uvA.v[j][0];
                vtx->t = uvA.v[j][1];
                vtx->r = 0x82;
                vtx->g = 0;
                vtx->b = (i == 0) ? 0xB : 0xF5;
                vtx->a = 0;
                vtx++;
            }
        }
        Heap_Alloc(gMainHeap, car->xB4C, 0xC0, 7, 0);
        gfx = HeapBlock_Ptr(car->xB4C);
        Heap_Alloc(gMainHeap, car->xB64, 0x80, 7, 0);
        vtx = HeapBlock_Ptr(car->xB64);
        GFX2(gfx, 0xD9FFF9FF, 0);
        GFX2(gfx, 0xD9FFFFFF, 0x401);
        GFX2(gfx, 0xE3001001, 0);
        GFX2(gfx, 0xFD18001F, D_80090930);
        GFX2(gfx, 0xF5181000, 0x07017C5F);
        GFX2(gfx, 0xE6000000, 0);
        GFX2(gfx, 0xF4000000, 0x0707C07C);
        GFX2(gfx, 0xE7000000, 0);
        GFX2(gfx, 0xF5181000, 0x00017C5F);
        GFX2(gfx, 0xF2000000, 0x0007C07C);
        GFX2(gfx, 0xE200001C, 0x005049D8);
        GFX2(gfx, 0x01008010, vtx);
        GFX2(gfx, 0x05000204, 0);
        GFX2(gfx, 0x05060402, 0);
        GFX2(gfx, 0x05080A0C, 0);
        GFX2(gfx, 0x050E0C0A, 0);
        GFX2(gfx, 0xDF000000, 0);
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 4; j++) {
                vtx->x = (s->x170[i].x + s->x188[i * 4 + j].x) * 100.0;
                vtx->y = (s->x170[i].y + s->x188[i * 4 + j].y) * 100.0;
                vtx->z = (s->x170[i].z + s->x188[i * 4 + j].z) * 100.0;
                vtx->flag = 0;
                vtx->s = uvB.v[j][0];
                vtx->t = uvB.v[j][1];
                vtx->r = 0x7E;
                vtx->g = 0;
                vtx->b = (i == 0) ? 0xB : 0xF5;
                vtx->a = 0;
                vtx++;
            }
        }
    }
}
