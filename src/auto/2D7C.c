#include "common.h"
#include "m2c.h"

typedef struct {
    s32 a;
    void *ptr;
} Pair;
typedef struct {
    u8 pad[0x10];
    void *x10;
    s32 count;
    Pair *pairs;
    void *x1C;
} Obj;
void Mem_Free(void *);

void func_8000217C(Obj *o) {
    s32 i;

    for (i = 0; i < o->count; i++) {
        Mem_Free(o->pairs[i].ptr);
        o->pairs[i].ptr = NULL;
    }
    Mem_Free(o->pairs);
    o->pairs = NULL;
    if (o->x10 != NULL) {
        Mem_Free(o->x10);
        o->x10 = NULL;
    }
    Mem_Free(o->x1C);
    o->x1C = NULL;
    Mem_Free(o);
}
