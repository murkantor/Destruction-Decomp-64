#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[2];
    s16 x2;
    s32 x4;
    u8 pad8[0x44 - 0x8];
    u8 x44;
    u8 pad45[0x58 - 0x45];
} Node;

typedef struct {
    u8 pad0[2];
    u16 x2;
    u8 pad4[0xC - 0x4];
    u32 xC;
} Item;

typedef struct {
    u8 pad0[0xC];
    Node *active[61];
    s32 nActive;
    Node *lists[4][60];
    Node *pools[4];
    s32 counts[4];
    Node *list8[8];
    Node *pool8;
    s32 count8;
} Mgr;

extern Mgr *volatile D_8011EEF8;
u8 DrawList_Insert(Mgr *m, s32 idx, s32 pos, Item *item);
void func_8005D448(Node *dst, Item *src);

static inline void Remove(Mgr *m, s16 key) {
    s32 j;

    for (j = 0; j < m->nActive; j++) {
        if (m->active[j]->x2 == key) {
            break;
        }
    }
    m->nActive--;
    *m->active[j] = *m->active[m->nActive];
    m->active[m->nActive]->x44 = 0;
}

void DrawList_Update(Mgr *m, s32 idx, s32 *pos, Item *item) {
    s32 end;
    Node **arr;
    s32 i;
    s16 key;
    Node *e;

    if (item->xC < 2) {
        end = m->count8;
        arr = m->list8;
    } else {
        end = D_8011EEF8->counts[idx];
        arr = D_8011EEF8->lists[idx];
    }
    key = item->x2;
    for (i = *pos; i <= end; i++) {
        if (i == end) {
            if (!DrawList_Insert(m, idx, i, item)) {
                Remove(m, key);
            }
            break;
        }
        e = arr[i];
        if (key == e->x2) {
            func_8005D448(e, item);
            break;
        }
        if (key < e->x2) {
            if (!DrawList_Insert(D_8011EEF8, idx, i, item)) {
                Remove(m, key);
            }
            break;
        }
    }
    *pos = i + 1;
}
