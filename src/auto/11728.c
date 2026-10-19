#include "common.h"
#include "m2c.h"

typedef struct Pool {
    void *free;
    u32 itemSize;
    void *base;
    u32 size;
    u32 sizeFree;
} Pool;

typedef struct { f32 x, y, z; } Vec3f;

typedef struct Particle {
    struct Particle *next;
    Vec3f pos;
    Vec3f vel;
    Vec3f acc;
    f32 x28;
    f32 x2C;
    u8 x30;
    u8 x31;
    u8 x32;
    u8 x33;
    u8 x34;
    u8 x35;
    u8 x36;
    u8 x37;
    u8 x38;
    u8 x39;
    u8 x3A;
    s32 x3C;
} Particle;

typedef struct {
    Particle *head;
    s32 count;
} ParticleList;

extern Pool gParticlePool;
void *Pool_Alloc(Pool *pool);

Particle *Particle_Spawn(ParticleList *list, Vec3f pos, Vec3f vel, Vec3f acc, f32 f28, f32 f2C,
                        u16 b30, u8 b31, u8 b32, u8 b3A, u8 b33, u8 b34) {
    Particle *p;
    Particle *prev;
    Particle *it;

    if (gParticlePool.sizeFree <= 0x2000) {
        return NULL;
    }
    p = Pool_Alloc(&gParticlePool);
    if (p == NULL) {
        return NULL;
    }
    p->pos = pos;
    p->vel = vel;
    p->acc = acc;
    p->x30 = b30;
    p->x28 = f28;
    p->x2C = f2C;
    p->x31 = b31;
    p->x32 = b32;
    p->x33 = b33;
    p->x34 = b34;
    p->x38 = 0;
    p->x35 = 0xFF;
    p->x36 = 0xFF;
    p->x37 = 0xFF;
    p->x3A = b3A;
    p->x3C = 0;
    p->x39 = p->x30;
    prev = NULL;
    for (it = list->head; it != NULL; it = it->next) {
        if (it->x33 >= b33) {
            break;
        }
        prev = it;
    }
    if (prev != NULL) {
        p->next = prev->next;
        prev->next = p;
    } else {
        p->next = list->head;
        list->head = p;
    }
    list->count++;
    return p;
}
