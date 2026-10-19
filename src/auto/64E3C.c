#include "common.h"
#include "m2c.h"

typedef struct ListNode { u8 pad[4]; struct ListNode *next; } ListNode;
typedef struct {
    u8 pad0[0xC];
    s32 items[0x280];
    ListNode *xA0C;
    u8 padA10[0x300];
    s16 xD10;
    s16 xD12;
} ObjList;
s32 Array_Find(s32 value, s32 *arr, s32 n);
void Array_SwapRemove(s32 *arr, s32 idx, s32 n);
void func_80023DDC(ObjList *l, s32 item);
void Mem_Free(void *);

s32 ObjList_Clear(ObjList *l) {
    s32 item;
    s32 idx;
    ListNode *node;

    while (l->xD10 > 0) {
        item = l->items[l->xD10 - 1];
        idx = Array_Find(item, l->items, l->xD10);
        if (idx >= 0) {
            Array_SwapRemove(l->items, idx, l->xD10);
            l->xD10--;
        }
        func_80023DDC(l, item);
    }
    node = l->xA0C;
    while (node != NULL) {
        ListNode *n = node;
        node = node->next;
        Mem_Free(n);
    }
    l->xD10 = 0;
    l->xD12 = 0;
    return 0;
}
