#include "common.h"
#include "m2c.h"

typedef struct {
    s32 value;
    s32 key;
} KeyValue;
extern KeyValue D_8007F84C[0x94];

s32 Ui_LookupTextId(s32 key) {
    s32 i;

    for (i = 0; i < 0x94; i++) {
        if (D_8007F84C[i].key == key) {
            break;
        }
    }
    return D_8007F84C[i].value;
}
