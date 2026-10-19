#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0xC];
    u8 *items[61];
    s32 count;
} ItemList;

u8 *func_8005CC00(ItemList *list, s16 id) {
    s32 i;

    for (i = 0; i < list->count; i++) {
        if (*(s16 *)(list->items[i] + 2) == id) {
            return list->items[i];
        }
    }
    return NULL;
}
