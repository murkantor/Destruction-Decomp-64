#include "common.h"
#include "m2c.h"

#include "pool.h"
typedef struct {
    f32 x, y, z;
} Vec3f;
typedef struct Particle {
    struct Particle *next;
    Vec3f pos;
    Vec3f vel;
    Vec3f acc;
    f32 x28;
    f32 x2C;
    u8 life;
} Particle;
typedef struct {
    Particle *head;
    s32 count;
} ParticleList;
extern Pool gParticlePool;
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);

void Particles_Update(ParticleList *l) {
    Particle *p = l->head;
    Particle *prev = NULL;
    Particle *next;

    while (p != NULL) {
        if (p->life == 0) {
            next = p->next;
            if (prev != NULL) {
                prev->next = next;
            } else {
                l->head = next;
            }
            Pool_Free(&gParticlePool, p);
            p = next;
            l->count--;
        } else {
            Vec3f_Add(&p->vel, &p->vel, &p->acc);
            Vec3f_Add(&p->pos, &p->pos, &p->vel);
            p->x28 += p->x2C;
            if (p->life != 0xFF) {
                p->life--;
            }
            prev = p;
            p = p->next;
        }
    }
}
