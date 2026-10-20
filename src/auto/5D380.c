#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0xC];
    u8 *items[61];
    s32 count;
} ItemList;

void Sound_InitForMode(ItemList *);

s32 func_8005C780(ItemList *list) {
    u8 *p = *(u8 **)((u8 *)list + 0xFC);
    s32 i;

    for (i = 0; i < 60; i++) {
        list->items[i] = p;
        p[0x44] = 0;
        *(s16 *)p = 0x3F4;
        p += 0x58;
    }
    list->count = 0;
    Sound_InitForMode(list);
    return 0;
}
