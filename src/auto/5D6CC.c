#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[2]; s16 x2; s32 x4[0x10]; u8 x44; u8 pad45[0x13]; } Item5CACC;
typedef struct { u8 pad0[0xC]; Item5CACC *xC[0x3D]; s32 x100; } List5CACC;

void List_RemoveById(List5CACC *list, s16 id) {
    s32 i;
    Item5CACC *src;
    char unused[1];

    for (i = 0; i < list->x100; i++) {
        if (list->xC[i]->x2 == id) {
            break;
        }
    }
    list->x100--;
    src = list->xC[list->x100];
    *list->xC[i] = *src;
    list->xC[list->x100]->x44 = 0;
}
