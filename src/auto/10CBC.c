#include "common.h"
#include "m2c.h"

typedef struct Elem {
    s32 x0;
    u8 pad4[0xC];
    void *x10;
    u8 pad14[0xC];
    s32 x20;
} Elem;

typedef struct Node {
    Elem *e;
    struct Node *next;
} Node;

typedef struct {
    s32 x0;
    u8 x4[0x20];
} Entry;

typedef struct {
    u8 pad0[0x8938];
    Entry entries[15];
    Node *list;
    u8 pad8B58[0xA];
    s16 count;
} Ui;

extern Ui *gUi;
s32 Ui_GetStyleRes(s32);
void Res_Load(s32, void *);
void Ui_AddResource(Ui *, void *);

void Ui_SetElementColor(s32 id, s32 res) {
#ifdef TARGET_PC
    PORT_DMA_BUF(u8, buf, 0x2400); /* 0x28 bytes */
#else
    u8 buf[0x28];
#endif
    Node *n;
    Elem *found = NULL;
    s8 i;

    for (n = gUi->list; n != NULL; n = n->next) {
        if (n->e->x0 == id) {
            found = n->e;
            break;
        }
    }
    for (i = 0; i < gUi->count; i++) {
        if (gUi->entries[i].x0 == res) {
            break;
        }
    }
    if (i == gUi->count) {
        Res_Load(Ui_GetStyleRes(res), buf);
        Ui_AddResource(gUi, buf);
    }
    {
        Ui *ui = gUi;
        found->x20 = res;
        found->x10 = ui->entries[i].x4;
    }
}
