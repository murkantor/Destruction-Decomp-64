#include "common.h"
#include "m2c.h"

typedef struct LNode {
    s32 value;
    struct LNode *next;
} LNode;
typedef struct {
    s32 x0;
    u32 offset;
    u8 pad8[4];
    s32 (*create)(s32);
    void (*destroy)(s32);
    LNode *list;
    u32 size;
    s32 count;
} ResType;
extern ResType gResTypes[];
extern s32 gNumResTypes;

void List_Remove(LNode **, s32);

void Res_FreeAll(void) {
    s32 i;

    for (i = 0; i < gNumResTypes; i++) {
        while (gResTypes[i].list != NULL) {
            List_Remove(&gResTypes[i].list, gResTypes[i].list->value);
        }
        gResTypes[i].count = 0;
    }
}
