#include "common.h"
#include "m2c.h"

typedef struct { s32 key; s8 val; } KeyMap;
extern KeyMap D_8007FCF0[];
extern s32 *D_800D1C18;
extern s32 D_80124470;

void func_80012984(s32 key) {
    KeyMap *m = D_8007FCF0;

    while (m->key != -1) {
        if (key == m->key) {
            D_800D1C18[D_80124470] = m->val;
            D_80124470++;
            return;
        }
        m++;
    }
    D_800D1C18[D_80124470] = 0;
    D_80124470++;
}
