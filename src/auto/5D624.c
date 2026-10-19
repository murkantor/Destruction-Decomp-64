#include "common.h"
#include "m2c.h"

typedef struct { s32 x0[4]; u16 x10; u16 x12; s32 x14[17]; } Blk58;
typedef struct { u8 pad[0xC]; Blk58 *xC[61]; s32 x100; } Pool58;
s32 Pool_PushCopy(Pool58 *p, Blk58 *b) {
    s32 ret = 0;
    s32 n = p->x100;
    if (n < 60) {
        if ((b->x10 != 0 && b->x10 < 0x27) || (b->x10 >= 0x65 && b->x10 < 0xF8)) {
            *p->xC[n] = *b;
            ret = 1;
            p->x100++;
        }
    }
    return ret;
}
