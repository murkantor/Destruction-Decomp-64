typedef struct {
    s32 x0;
    u8 pad4[0xC];
    s32 x10;
} RaceState;
typedef struct {
    u8 pad[0x1100];
    RaceState *race;
} GameState;
typedef struct {
    s8 b0;
    s8 b1;
    s8 b2;
    s8 b3;
    s16 x4;
} Input;
typedef struct {
    u8 pad[0x2C];
    s32 tick;
} Brain;
typedef struct {
    u8 pad[0xD];
    u8 xD;
    u8 padE[0x1E];
    Input *input;
    u8 pad30[8];
    Brain *brain;
} Driver;
extern GameState *gGame;
extern s32 D_800958A8;
extern s32 D_800958AC;
extern volatile u32 D_800958B4;
void func_80005180(Driver *);
void func_800051F4(Driver *);
void func_80005DCC(Driver *);
void func_80006378(Driver *);

void func_80004E00(Driver *d) {
    Brain *b = d->brain;
    RaceState *r = gGame->race;
    s32 n;
    s32 ok;

    if (d->xD != 0 || (r->x0 == 1 && r->x10 == r->x0)) {
        d->input->b0 = 0;
        d->input->b1 = 0;
        d->input->b2 = 1;
        d->input->b3 = 0;
        d->input->x4 = 0;
        return;
    }
    if (b->tick == 0) {
        if (D_800958A8 == 3 && D_800958AC == 2) {
            func_800051F4(d);
        } else {
            func_80005180(d);
            func_80005DCC(d);
        }
    } else {
        func_80006378(d);
    }
    n = b->tick + 1;
    b->tick = n;
    ok = n < 5;
    if (D_800958B4 >= 4) {
        ok = n < 3;
    }
    if (!ok) {
        b->tick = 0;
    }
}
