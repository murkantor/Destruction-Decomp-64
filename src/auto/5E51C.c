#include "common.h"
#include "m2c.h"

typedef struct UiNode { void *elem; struct UiNode *next; } UiNode;
typedef struct { u8 pad[0x8B54]; UiNode *list; } Ui;
extern void *Ui_FindElement(Ui *);
extern void *Mem_Alloc(u32);
void Ui_AddElement(Ui *ui) {
    UiNode *n;
    UiNode *head = ui->list;
    if (head == NULL) {
        void *e = Ui_FindElement(ui);
        UiNode *nn = Mem_Alloc(8);
        if (nn != NULL) {
            nn->elem = e;
            nn->next = ui->list;
            ui->list = nn;
        }
    } else {
        void *e;
        UiNode *nn;
        n = head;
        if (n->next != NULL) {
            do {
                n = n->next;
            } while (n->next != NULL);
        }
        e = Ui_FindElement(ui);
        nn = Mem_Alloc(8);
        if (nn != NULL) {
            nn->elem = e;
            nn->next = NULL;
            n->next = nn;
        }
    }
}
