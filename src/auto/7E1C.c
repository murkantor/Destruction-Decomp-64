#include "common.h"
#include "m2c.h"

typedef struct { s16 x0; u8 pad[6]; u8 x8; u8 x9; u8 pad2[2]; } E;
extern E D_8007EDF8[];
extern s8 D_800D31CC;
extern s32 D_8011EEF8;
void List_RemoveById(s32, s32);

void func_8000721C(void) {
    s32 i;
    for (i = 0; i < D_800D31CC; i++) {
        if (D_8007EDF8[i].x9) {
            if (D_8007EDF8[i].x8) {
                D_8007EDF8[i].x8 = 0;
            } else {
                List_RemoveById(D_8011EEF8, D_8007EDF8[i].x0);
                D_8007EDF8[i].x9 = 0;
            }
        }
    }
}
