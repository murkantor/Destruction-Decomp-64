#include "common.h"
#include "m2c.h"

void Mem_RelocatePtrs(s32 *arr, s32 add, s32 n) {
    s32 i;

    for (i = 0; i < n; i++) {
        if (arr[i] != 0) {
            arr[i] += add;
        }
    }
}
