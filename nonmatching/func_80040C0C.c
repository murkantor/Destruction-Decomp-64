typedef struct { u8 pad[0x10]; s32 x10; } Menu;
typedef struct { s32 x0; s32 x4; u8 pad8[0x60]; } Player;
typedef struct {
    s32 count;
    s32 x4;
    s32 mode;
    s32 xC;
    s32 track;
    s32 x14[2];
    s32 sel[4];
    u8 pad2C[0x34];
    Player players[4];
} Setup;
typedef struct {
    s32 x0;
    u8 flags[4];
    u8 pad8[8];
    s32 numCars;
} SelState;
extern Setup D_800958A4;
extern SelState D_80097340;
extern Player D_80095904[];
extern u8 D_80095CC8[];
extern s32 D_80125BE4[];
extern s32 D_80096EA0;
void Timer_Start(void *, s64);
void Ui_RemapTextures(void);
void func_80063564(s32, s32);

s32 func_80040C0C(Menu *m) {
    s32 i, j;
    s32 v;
    u8 again;

    Timer_Start(m, 300000000);
    if (!D_80095CC8[D_80125BE4[m->x10]]) {
        return 0;
    }
    D_800958A4.track = D_80125BE4[m->x10];
    D_80097340.x0 = 0;
    D_80097340.numCars = 24;
    if (D_800958A4.count == 1) {
        m->x10 = D_800958A4.sel[0];
    } else {
        for (i = 0; i < D_800958A4.count; i++) {
            if (D_800958A4.mode == 2 && D_800958A4.count == 4) {
                D_800958A4.players[i].x0 = 1;
            } else {
                D_800958A4.players[i].x0 = 0;
            }
            v = D_800958A4.sel[i];
            again = 1;
            if (D_800958A4.mode == 2 && D_800958A4.count == 4) {
                switch (i) {
                    case 0:
                        D_80095904[0].x4 = v;
                        D_80097340.flags[0] = 0;
                        break;
                    case 1:
                        D_80097340.flags[1] = 0;
                        D_80095904[1].x4 = D_800958A4.players[0].x4;
                        D_800958A4.sel[1] = D_800958A4.sel[0];
                        break;
                    case 2:
                        if (D_800958A4.sel[0] == v) {
                            v = (v + 1) % D_80097340.numCars;
                        }
                        D_80095904[2].x4 = v;
                        D_800958A4.sel[2] = v;
                        D_80097340.flags[2] = 0;
                        break;
                    case 3:
                        D_80097340.flags[3] = 0;
                        D_80095904[3].x4 = D_800958A4.players[2].x4;
                        D_800958A4.sel[3] = D_800958A4.sel[2];
                        break;
                }
            } else {
                do {
                    for (j = 0; j < D_800958A4.count; j++) {
                        if (i != j && D_800958A4.sel[j] == v) {
                            v = (v + 1) % D_80097340.numCars;
                            break;
                        }
                    }
                    if (j == D_800958A4.count) again = 0;
                } while (again);
                D_800958A4.players[i].x4 = v;
                D_800958A4.sel[i] = v;
                D_80097340.flags[i] = 0;
            }
        }
    }
    Ui_RemapTextures();
    func_80063564(0x18, 1);
    D_80096EA0 = 1;
    return 1;
}
