#include "common.h"
#include "m2c.h"

typedef struct {
    s16 x0;
    s16 id;
    u8 pad4[0x8];
    u32 xC;
    u8 pad10[0x38 - 0x10];
    u64 x38;
    u8 x40[4];
    u8 x44;
    u8 x45;
    u8 pad46[0x58 - 0x46];
} Elem;
typedef struct {
    u8 pad0[0xC];
    Elem *items[(0x100 - 0xC) / 4];
    s32 count;
} ElemList;
typedef struct { u8 pad[0x1058]; u64 x1058; } Game;
extern Game *gGame;
extern u64 gGameTime;
void func_800091AC(ElemList *);
u64 Time_Get(u64 *);
void List_RemoveById(ElemList *, s32);

void ElemList_ExpireAndSortById(ElemList *l) {
    s32 i, j;
    u64 t1, t2, now;
    Elem *e;
    Elem tmp;

    func_800091AC(l);
    i = 0;
    t1 = Time_Get(&gGameTime);
    t2 = Time_Get(&gGame->x1058);
    for (; i < l->count; i++) {
        e = l->items[i];
        now = t2;
        if (e->xC < 2) {
            now = t1;
        }
        if (e->x44 == 1 && now >= e->x38 && !e->x45) {
            List_RemoveById(l, e->id);
            i--;
        }
    }
    for (i = 0; i < l->count; i++) {
        for (j = i + 1; j < l->count; j++) {
            if (l->items[i]->id > l->items[j]->id) {
                tmp = *l->items[j];
                *l->items[j] = *l->items[i];
                *l->items[i] = tmp;
            }
        }
    }
}
