#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0xC];
    s32 flags;
    u8 pad10[0xA0];
} Entry;

Entry *Pad_GetLatched(void);

s32 AnyEntryFlag10(void) {
    s32 found = 0;
    Entry *p = Pad_GetLatched();
    s32 i;

    for (i = 0; i < 4; i++) {
        if (p[i].flags & 0x10) {
            found = 1;
        }
    }
    return found;
}
