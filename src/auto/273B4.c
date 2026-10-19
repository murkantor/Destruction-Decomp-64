#include "common.h"
#include "m2c.h"

extern s32 D_800958A4;
typedef struct { u8 pad[0x3C]; s32 x3C[3]; } Obj;
s32 Obj_HasValue(Obj *obj, s32 value) {
    char unused[1];
    u8 found;
    s32 n;
    s32 i;

    
    n = (D_800958A4 == 2) ? 3 : 2;
    found = 0;
    for (i = 0; i < n; i++) {
        if (obj->x3C[i] == value) {
            found = 1;
        }
    }
    return found;
}
