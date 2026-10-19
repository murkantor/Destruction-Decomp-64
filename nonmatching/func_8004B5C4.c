typedef struct {
    u8 pad[0x10];
    s32 value;
} MenuItem;
extern s32 D_800958A8;
extern u32 D_800958B4;
extern s32 D_80097D50;
extern u8 D_80097335;
extern u8 D_80097336;
extern u8 D_8009734C;
extern u8 D_8009734D;
extern u8 D_800972F0;
s32 func_80049FC0(s32);
u8 func_8004FD80(s32 *);
u8 func_80050910(s32);
void func_80063564(s32, s32);
void func_80063380(s32);

s32 func_8004B5C4(MenuItem *item) {
    u8 done = 0;
    s32 next;
    s32 slot;
    s32 n;

    if (D_800958A8 == 3) {
        item->value = 0;
        done = 1;
    } else {
        next = func_80049FC0(item->value);
        if (next == -1) {
            done = 1;
        } else if (D_800958B4 >= 12) {
            done = 1;
        } else {
            item->value = next;
        }
    }
    if (done) {
        item->value = 0;
        D_80097335 = 0;
        if (D_80097D50 != -1) {
            D_8009734C = 0;
            D_8009734D = 1;
            func_80063564(func_8004FD80(&slot) ? (func_80050910(slot) ? 11 : 6) : 6, 1);
        } else if (D_800972F0) {
            func_80063564(0x2E, 1);
        } else {
            func_80063380(2);
            func_80063564(25, 1);
            item->value = 0;
        }
        D_80097336 = 0;
    }
    return 1;
}
