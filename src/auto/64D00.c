#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0xC];
    s32 items[256];
} List;
typedef struct {
    u8 pad[0x334];
    List list;
    u8 pad2[0x1044 - 0x334 - sizeof(List)];
    s16 count;
} GameState;
extern GameState *gGame;
void Handler_CallX4ById(List *, s32);

s32 ObjList_Add(s32 x) {
    GameState *g = gGame;
    List *l = &g->list;

    if (g->count < 256) {
        Handler_CallX4ById(l, x);
        l->items[g->count++] = x;
        return 0;
    }
    return 1;
}
