typedef struct {
    void *cur;
    void *def;
} Current;
extern Current D_800CCAC0;
void func_80065A58(s32);

void func_80066194(u8 *obj, s32 arg) {
    if (obj != NULL && *(s32 *)(obj + 0x10) < 0) {
        D_800CCAC0.cur = obj;
        func_80065A58(arg);
        D_800CCAC0.cur = D_800CCAC0.def;
    }
}
