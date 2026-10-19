#include "common.h"
#include "m2c.h"

extern s32 D_80124364;
extern s32 D_80126068[];
void Msg_Queue(s32, s32, s32, s32);

void func_8000870C(s32 *arr, s32 n, s32 val) {
    s8 i, j;
    for (i = 0; i < D_80124364; i++) {
        for (j = 0; j < n; j++) {
            if (arr[j] == D_80126068[i]) {
                D_80126068[i] = val;
                return;
            }
        }
    }
    Msg_Queue(val, 0, 1, 0);
}
