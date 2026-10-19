#include "common.h"
#include "m2c.h"

s32 Array_Find(s32 value, s32 *arr, s32 n) {
    s32 found = -1;
    s32 i;

    for (i = 0; i < n; i++) {
        if (arr[i] == value) {
            found = i;
            break;
        }
    }
    return found;
}
