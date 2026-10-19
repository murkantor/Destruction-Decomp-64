typedef struct {
    u8 pad0[0xC];
    void *(*load)(s32);
    u8 pad10[0x4];
    void **x14;
    u8 pad18[0x4];
    s32 x1C;
} Res;

extern Res *Res_Find(s32);
extern void func_80063FA0(void ***, void *);
extern u8 D_800B4D70;

void *Res_Get(s32 id)
{
    Res *r = Res_Find(id);
    void *p;
    if (r == NULL)
        return NULL;
    if (D_800B4D70 == 0 && r->x1C > 0) {
        p = *r->x14;
    } else {
        p = r->load(id);
        func_80063FA0(&r->x14, p);
    }
    r->x1C++;
    return p;
}
