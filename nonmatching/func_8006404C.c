typedef struct LNode {
    s32 value;
    struct LNode *next;
} LNode;

s32 func_8006404C(LNode **list, s32 value) {
    LNode *n = *list;
    s32 found = 0;

    if (n != NULL) {
        found = n->value == value;
        while (n->next != NULL) {
            n = n->next;
            if (n->value == value) {
                found = 1;
                break;
            }
        }
    }
    return found;
}
