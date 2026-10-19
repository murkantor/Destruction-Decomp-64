#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } CarData;
typedef struct { CarData *x0; u8 pad[0x64]; } Car;
extern Car D_800958F0[];
extern s8 D_800D31CC;
s32 rand(void);

CarData *Car_PickRandomActive(void) {
    s32 i = rand() % D_800D31CC;
    if (D_800958F0[i].x0->x10 <= 0) {
        for (i = 0; i < D_800D31CC; i++) {
            if (D_800958F0[i].x0->x10 >= 0) {
                break;
            }
        }
    }
    if (D_800958F0[i].x0->x10 <= 0) {
        return NULL;
    }
    return D_800958F0[i].x0;
}
