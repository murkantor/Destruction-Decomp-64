#include "common.h"
#include "m2c.h"

typedef struct { s16 id; } Item;
typedef void *(*HandlerFn)(void *, void *, s32, s32);
typedef struct { s16 id; u8 pad[0xE]; HandlerFn fn; } Handler;
typedef struct { u8 pad[0x340]; void *x340[(0x1044 - 0x340) / 4]; s16 x1044; } Game;
extern Game *gGame;
extern s32 D_80095810;
extern Handler D_80095630[];

void *func_80024390(void *acc, s32 b, s32 c) {
    s32 i, j;
    void *obj;
    Item *it;

    for (i = 0; i < gGame->x1044; i++) {
        obj = gGame->x340[i];
        it = obj;
        for (j = 0; j < D_80095810; j++) {
            if (D_80095630[j].id == it->id) {
                if (D_80095630[j].fn != NULL) {
                    acc = D_80095630[j].fn(obj, acc, b, c);
                }
                break;
            }
        }
    }
    return acc;
}
