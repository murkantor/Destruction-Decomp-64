#include "common.h"
#include "m2c.h"

typedef struct {
    s32 value;
    s32 key;
} KeyValue;
extern KeyValue D_80137104[];

s32 func_80010228(s32 key) {
    s8 i;

    for (i = 0; D_80137104[i].value != 0; i++) {
        if (D_80137104[i].key == key) {
            break;
        }
    }
    return D_80137104[i].value;
}
