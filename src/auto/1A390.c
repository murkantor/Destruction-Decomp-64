#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { s32 x0; s32 x4; } HeapBlock;
typedef struct {
    f32 scale;
    f32 size;
    Vec3f vel;
    s8 count;
    u8 pad[3];
} WeatherType;
typedef struct {
    void *head;
    u8 pad4[0x28];
    void *bufs[2];
    HeapBlock blk[2];
    s8 frame;
    s8 type;
} Weather;

extern Vec3f D_800B8790;
extern s32 D_800958B4;
extern u8 D_8008F820;
extern u8 D_80131F08;
extern WeatherType D_8008F824[];
extern Weather D_800D1AA0;
extern s8 D_800D1AE4;
extern s8 D_800D1AE5;
extern f32 D_8012435C;
extern Vec3f D_80124588;
extern Vec3f D_801242AC;
extern u8 gMainHeap[];
extern u8 D_8008F620[], D_8008F720[];
extern void Heap_Alloc(void *, HeapBlock *, s32, s32, s32);
extern void *HeapBlock_Ptr(HeapBlock *);
extern void func_80010950(Weather *, s32, void *, s32, s32, s32, s32, s32);
extern int rand(void);
extern void *Particle_Spawn(Weather *, Vec3f, Vec3f, Vec3f, f32, f32, s32, s32, s32, s32, s32, s32);

void Weather_Init(void) {
    Vec3f vel = D_800B8790;
    Vec3f acc = D_800B8790;
    s32 i;
    s32 type;
    f32 range;
    f32 half;
    f32 r;
    Weather *w;
    Vec3f p;

    if (D_800958B4 == 10) {
        range = 1400.0f;
        type = 1;
        D_8008F820 = 0;
        D_80131F08 = 0;
        D_8008F824[0].count = 90;
        D_8008F824[1].count = 90;
        D_800D1AE4 = 0;
        D_800D1AE5 = type;
        D_8012435C = 10.0f;
        half = 700.0f;
        D_80124588.x = half;
        D_80124588.y = half;
        D_80124588.z = half;
        D_801242AC.x = -half;
        D_801242AC.y = -half;
        D_801242AC.z = -half;
        w = &D_800D1AA0;
        for (i = 0; i < 2; i++) {
            Heap_Alloc(gMainHeap, &w->blk[i], (D_8008F824[type].count + 40) * 16, 7, 0);
            w->bufs[i] = HeapBlock_Ptr(&w->blk[i]);
        }
        if (type == 0) {
            func_80010950(w, 2, D_8008F620, 0, 3, 8, 8, 1);
        } else {
            func_80010950(w, 1, D_8008F720, 0, 3, 8, 8, 1);
        }
        vel = D_8008F824[type].vel;
        if (type == 0) {
            r = rand() * (1.0f / 32767) * 6.0;
            vel.z = vel.x = (r - 3.0) * 10.0;
        }
        for (i = 0; i < D_8008F824[type].count; i++) {
            p.x = D_801242AC.x + rand() * (range * (1.0f / 32767));
            p.y = D_801242AC.y + rand() * (range * (1.0f / 32767));
            p.z = D_801242AC.z + rand() * (range * (1.0f / 32767));
            r = rand() * (1.0f / 32767) * -0.8;
            vel.y = (D_8008F824[type].vel.y + r) * 10.0;
            Particle_Spawn(w, p, vel, acc, D_8008F824[type].size, 0, 0xFF, 1, 0xA0, 0, 0, 1);
        }
    }
}
