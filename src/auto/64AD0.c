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

u8 List_Contains(LNode **, s32);

ResType *Res_FindType(s32 key) {
    s32 i;
    ResType *found = NULL;

    for (i = 0; i < gNumResTypes; i++) {
        if (List_Contains(&gResTypes[i].list, key)) {
            found = &gResTypes[i];
            break;
        }
    }
    return found;
}
