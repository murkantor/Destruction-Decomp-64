typedef struct LNode {
    s32 value;
    struct LNode *next;
} LNode;
void Mem_Free(void *);

void func_80063FEC(LNode **list, s32 value) {
    LNode *n = *list;
    LNode *prev;

    if (n != NULL) {
        if (n->value == value) {
            *list = n->next;
        } else {
            for (prev = n; ; prev = n) {
                n = prev->next;
                if (n->value == value) {
                    break;
                }
            }
            prev->next = n->next;
        }
        Mem_Free(n);
    }
}
