typedef struct {
    s32 total;
    u16 *src;
    s32 idx;
    s32 padC;
    u64 interval;
    u64 next;
} Reveal;

typedef struct {
    u8 pad0[0x10];
    u16 **x10;
} Target;

typedef struct {
    u8 pad0[0x1058];
    u64 x1058;
} Game;

extern Game *gGame;
extern void *gUi;
extern Reveal D_800D1AE8;
extern u8 D_8007EF33;
u64 Time_Get(u64 *);
void Ui_AddElement(void *, s32, s32);

s32 func_8000E6E0(Target *t) {
    s32 i;
    u16 *dst;
    u16 *src;
    u64 now;

    if (D_800D1AE8.idx < 11 && Time_Get(&gGame->x1058) >= D_800D1AE8.next) {
        i = D_800D1AE8.idx;
        dst = *t->x10;
        src = D_800D1AE8.src;
        for (; i < D_800D1AE8.total; i += 11) {
            dst[i] = src[i];
        }
        now = Time_Get(&gGame->x1058);
        D_800D1AE8.next = D_800D1AE8.interval + now;
        D_800D1AE8.idx++;
        return 0;
    }
    if (!D_8007EF33 && D_800D1AE8.idx >= 11) {
        Ui_AddElement(gUi, 2, 0xB3);
        D_8007EF33 = 1;
    }
    return 0;
}
