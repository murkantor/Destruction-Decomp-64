typedef struct {
    u8 pad[0x28];
} Clock;

typedef struct {
    u8 pad0[0xC];
    s16 mode;
    u8 padE[0x326];
    u8 x334[0x40C];
    void *x740[(0x1046 - 0x740) / 4];
    u8 pad1044[2];
    s16 x1046;
    u8 pad1048[0x10];
    Clock clocks[3];
    u8 pad10D0[0x28];
    s32 x10F8;
    s16 x10FC;
    u8 pad10FE[2];
    struct Race *race;
} Game;

typedef struct Race {
    u8 pad0[0x10];
    s32 x10;
    u8 x14[0xC];
    u8 x20[0xC];
} Race;

typedef struct {
    s32 x0;
    s32 x4;
    u8 pad8[8];
} TrackEntry;

typedef struct {
    u8 pad0[0x10];
} PlayerSub;

typedef struct {
    u8 pad0[0xC];
    s32 xC;
    u8 pad10[0x18];
    PlayerSub x28;
    s32 x38;
    u8 pad3C[0x2C];
} Player;

typedef struct {
    u8 x0;
    u8 x1;
    u8 pad2[2];
    s32 x4;
    s32 x8;
    s32 xC;
    s32 x10;
    s32 x14;
    u8 pad18[0x8];
    s32 x20;
    s32 x24;
    s32 x28;
    s32 x2C;
    Player players[4];
} Cfg;

extern u8 D_800958A0[];
extern u8 D_800958A1;
extern s32 D_800958A4[];
extern s32 D_800958A8[];
extern s32 D_800958AC[];
extern s32 D_800958B0[];
extern s32 D_800958B4;
extern s32 D_800958C0;
extern s32 D_800958C4;
extern s32 D_800958C8;
extern s32 D_800958CC;
extern Player D_800958D0[];
extern s32 D_800958DC[];
extern Player D_800958F8[];
extern s32 D_80095908;

extern Game *gGame[];
extern u64 gGameTime;
extern u64 D_80095830;
extern TrackEntry *D_80095CA8[];
extern s32 D_80095CB8[];
extern s32 D_801252F4;
extern s8 D_800D31CC[];

void func_800635B0(void *, void *);
void func_800638C8(void *);
void func_800638C0(void *);
void func_80064948(void *);
void func_8004396C(void);
void Race_LoadTrackResources(s32);
void List_BuildSorted(void *);
s32 func_80062F9C(Player *);
void func_80062FD4(void *);

static inline void Clock_Start(s32 idx, void *src) {
    gGame[0]->x10F8++;
    func_800635B0(&gGame[0]->clocks[idx], src);
}

void func_8002C318(void) {
    Race *race;
    s32 i;
    s32 sel;
    s8 j;
    Player *p;

    Clock_Start(1, &gGameTime);
    func_800638C8(&gGame[0]->clocks[1]);
    D_80095830 = 0;
    p = D_800958F8;
    for (i = 0; i < 4; i++) {
        gGame[0]->x740[gGame[0]->x1046++] = p++;
    }
    race = gGame[0]->race;
    switch (gGame[0]->mode) {
    case -1:
        break;
    case 0:
        D_800D31CC[0] = 1;
        Clock_Start(2, &gGame[0]->clocks[0]);
        func_800638C8(&gGame[0]->clocks[2]);
        break;
    case 1:
        D_800D31CC[0] = 1;
        if (D_800958A0[0]) {
            D_800958A0[0] = 0;
            D_800958A4[0] = 1;
            D_800958A8[0] = 0;
            D_800958AC[0] = 0;
            D_800958B0[0] = 0;
            D_800958C8 = 2;
            D_800958C0 = 0;
            D_800958C4 = 1;
            D_800958CC = 3;
            D_800958B4 = D_80095CA8[0]->x0;
        }
        D_80095908 = D_800958A8[0];
        func_80064948(D_800958A0 + 0x58);
        D_800958A1 = 0;
        break;
    case 2:
        sel = -1;
        if (gGame[0]->x10FC == 25) {
            func_8004396C();
        }
        D_800D31CC[0] = D_800958A4[0];
        if (gGame[0]->x10FC == 25) {
            goto clear;
        }
        if (D_800958A8[0] == 0) {
            sel = D_801252F4;
        }
        if (D_800958A8[0] == 1) {
            for (i = 0; i < D_80095CB8[D_800958B0[0]]; i++) {
                if (D_80095CA8[D_800958B0[0]][i].x0 == D_800958A4[4]) {
                    sel = i;
                    break;
                }
            }
        }
        if (sel == -1) {
            goto clear;
        }
        switch ((u32)D_80095CA8[D_800958B0[0]][sel].x4) {
        case 2:
            D_800958A1 = 1;
            break;
        case 3:
            D_800958A1 = 1;
            break;
        case 0:
            D_800958A1 = 0;
            break;
        case 1:
        clear:
            D_800958A1 = 0;
            break;
        }
        Race_LoadTrackResources(D_800958B4);
        Clock_Start(2, &gGame[0]->clocks[0]);
        func_800638C0(&gGame[0]->clocks[2]);
        if (race->x10 != 1) {
            List_BuildSorted(race->x20);
        }
        List_BuildSorted(race->x14);
        break;
    }
    for (j = 0; j < D_800D31CC[0]; j++) {
        D_800958D0[j].xC = func_80062F9C(&D_800958D0[j]);
    }
    func_80062FD4(gGame[0]->x334);
}
