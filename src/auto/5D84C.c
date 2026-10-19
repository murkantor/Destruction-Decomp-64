#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0xC];
    u8 *items[61];
    s32 count;
} ItemList;

void func_8005CC4C(ItemList *list) {
    s32 i;

    for (i = 0; i < list->count; i++) {
        if (*(u32 *)(list->items[i] + 0xC) >= 2) {
            list->items[i][0x45] = 1;
        }
    }
}
