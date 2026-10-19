#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x104]; void *x104[4][60]; u8 pad2[0x10]; s32 x4D4[4]; void *x4E4[9]; s32 x508; } World;
typedef struct { u8 pad[0xC]; u32 xC; } Thing;
void func_8005D4E0(void *);

void World_RemoveObject(World *w, Thing *t, s32 grp, s32 idx) {
    void **list;
    s32 *count;
    s32 i;

    if (t->xC < 2) {
        list = w->x4E4;
        count = &w->x508;
    } else {
        list = w->x104[grp];
        count = &w->x4D4[grp];
    }
    func_8005D4E0(list[idx]);
    for (i = idx; i < *count - 1; i++) {
        list[i] = list[i + 1];
    }
    list[*count - 1] = NULL;
    (*count)--;
}
