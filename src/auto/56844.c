#include "common.h"
#include "m2c.h"

typedef struct Emitter { struct Emitter *next; s32 count; void *items[1]; } Emitter;
extern u8 gParticlePool[];
void Pool_Free(void *, void *);

void Emitter_FreeAll(Emitter *e, void *pool) {
    Emitter *next;

    while (e != NULL) {
        while (e->count != 0) {
            e->count--;
            Pool_Free(pool, e->items[e->count]);
        }
        next = e->next;
        Pool_Free(gParticlePool, e);
        e = next;
    }
}
