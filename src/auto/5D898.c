#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0xC];
    u8 *items[61];
    s32 count;
} ItemList;

void func_8005CC98(ItemList *list) {
    s32 i;

    for (i = 0; i < list->count; i++) {
        list->items[i][0x45] = 0;
    }
}
