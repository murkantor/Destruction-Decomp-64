typedef struct LNode {
    s32 value;
    struct LNode *next;
} LNode;

void func_8006409C(LNode **src, LNode **dst, s32 value) {
    LNode *n = *src;
    LNode *prev;

    if (n == *dst) {
        return;
    }
    if (n->value == value) {
        *src = n->next;
    } else {
        prev = n;
        while (prev->next->value != value) {
            prev = prev->next;
        }
        n = prev->next;
        prev->next = n->next;
    }
    n->next = NULL;
    if (*dst != NULL) {
        n->next = *dst;
    }
    *dst = n;
}
