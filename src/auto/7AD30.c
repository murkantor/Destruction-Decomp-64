#include "common.h"
#include "m2c.h"

extern u8 _ctype[];

s32 toupper(s32 c) {
    if (_ctype[c & 0xFF] & 4) {
        c -= 0x20;
    }
    return c;
}
