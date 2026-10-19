#include "common.h"
#include "m2c.h"

void Mem_Fill(u8 *p, s32 v, s32 n) {
    char unused[1];
    s32 i;

    for (i = n - 1; i != -1; i--) {
        *p++ = v;
    }
}
