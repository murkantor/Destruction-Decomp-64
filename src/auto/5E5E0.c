#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; } UiElem;
typedef struct UiNode { UiElem *elem; struct UiNode *next; } UiNode;
typedef struct { u8 pad[0x8B54]; UiNode *head; } Ui;
UiElem *Ui_FindElement(Ui *, s32, s32);
void *Mem_Alloc(u32 size);

void Ui_InsertElementBefore(Ui *ui, s32 a, s32 key, s32 b) {
    UiNode *n;
    UiNode *prev;
    UiNode *cur;
    UiElem *e;
    UiElem *ce;

    if (ui->head == NULL) {
        UiElem *e0 = Ui_FindElement(ui, a, b);
        n = Mem_Alloc(8);
        if (n == NULL) {
            return;
        }
        n->elem = e0;
        n->next = ui->head;
        ui->head = n;
    } else {
    e = Ui_FindElement(ui, a, b);
    prev = ui->head;
    cur = prev;
    ce = cur->elem;
    while (ce->x0 != key) {
        prev = cur;
        cur = prev->next;
        ce = cur->elem;
    }
    n = Mem_Alloc(8);
    if (n == NULL) {
        return;
    }
    n->elem = e;
    n->next = cur;
    if (prev != ui->head) {
        prev->next = n;
    } else {
        ui->head = n;
    }
    }
}
