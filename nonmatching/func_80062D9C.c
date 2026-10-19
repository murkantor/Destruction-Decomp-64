typedef struct { s32 x0; s32 x4; s32 x8; } InitConfig;
extern InitConfig gInitConfig;
extern s32 D_800C06F0;
extern f32 D_800C06F4;
extern s32 D_800C06F8;
extern s32 D_800C06FC;
extern f64 D_800BBF30;
extern s32 func_80062F34(void);
extern void func_80062E4C(s64);
void func_80062D9C(void) {
    s64 t;
    switch (gInitConfig.x8) {
    case 0:
        D_800C06F0 = 60;
        break;
    case 1:
    case 2:
        D_800C06F0 = 50;
        break;
    }
    if (func_80062F34() > 0) {
        D_800C06F4 = D_800BBF30 / (f32)func_80062F34();
    } else {
        D_800C06F4 = 1.0f;
    }
    t = 0;
    D_800C06F8 = t >> 32;
    D_800C06FC = t;
    func_80062E4C(t);
}
