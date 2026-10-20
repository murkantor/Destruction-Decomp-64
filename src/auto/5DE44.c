#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x40]; s32 x40; u8 pad2[0xC]; } Node50;
typedef struct { u8 pad[0xC]; u32 xC; } Item;
typedef struct {
    u8 pad[0x104];
    Node50 *lists[4][60];
    Node50 *pools[4];
    s32 counts[4];
    Node50 *list8[8];
    Node50 *pool8;
    s32 count8;
} Mgr;
void Snd_InitFromTemplate(Node50 *, Item *);

s32 DrawList_Insert(Mgr *m, s32 idx, s32 pos, Item *item) {
    Node50 **list;
    s32 *cnt;
    Node50 *p;
    s32 max;
    s32 j;
    s32 k;
    Node50 *pool;
    s32 ret;

    if (item->xC < 2) {
        if (m->count8 == 8) {
            return 0;
        }
        ret = 1;
        list = m->list8;
        cnt = &m->count8;
        pool = m->pool8;
        max = 8;
    } else {
        if (m->counts[idx] == 60) {
            return 0;
        }
        ret = 1;
        list = m->lists[idx];
        cnt = &m->counts[idx];
        pool = m->pools[idx];
        max = 60;
    }
    for (j = *cnt; pos < j; j--) {
        list[j] = list[j - 1];
    }
    p = pool;
    for (k = 0; k < max; k++, p++) {
        if (p->x40 == 0) {
            break;
        }
    }
    Snd_InitFromTemplate(p, item);
    list[pos] = p;
    (*cnt)++;
    return ret;
}
