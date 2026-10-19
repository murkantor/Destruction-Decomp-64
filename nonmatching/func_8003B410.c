typedef struct {
    u8 pad[0x10A8];
    s32 x10A8;
    u32 x10AC;
    u8 pad10B0[0x4C];
    s16 x10FC;
} GameState;
extern GameState *gGame;
extern s16 D_8011EEEA;
extern s32 gUi;
void func_8000E688(GameState *, s64);
void func_8005DC68(s32);
void func_80063564(s32, s32);

s32 func_8003B410(void) {
    s32 ret = 0;

    if (gGame->x10FC == D_8011EEEA) {
        if (gGame->x10A8 != 0 || gGame->x10AC >= 4000000000LL) {
            func_8000E688(gGame, 4000000000LL);
            func_8005DC68(gUi);
            func_80063564(1, 1);
            ret = 1;
        }
        return ret;
    }
    return 0;
}
