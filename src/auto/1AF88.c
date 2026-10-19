#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x18];
    s16 *items;
    u8 pad1C[0x8];
    s32 count;
} List16;

void func_8001A388(List16 *list, s16 value) {
    list->items[list->count] = value;
    list->count++;
}
