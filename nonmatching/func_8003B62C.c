typedef struct { u8 pad[0x10A8]; u64 x10A8; u8 pad10B0[0x4C]; s16 x10FC; } Game;
extern Game *gGame;
extern void *gUi[];
extern s16 D_8011EEEA;
void func_8005DC68(void *);
void func_80063564(s32, s32);

static inline s32 Time_Elapsed(u64 *t, u64 limit) {
    return *t >= limit;
}

s32 func_8003B62C(void) {
    s32 r = 0;

    if (gGame->x10FC != D_8011EEEA) {
        return 0;
    }
    if (Time_Elapsed(&gGame->x10A8, 4000000000ULL)) {
        func_8005DC68(gUi[0]);
        func_80063564(0, 1);
        r = 1;
    }
    return r;
}
