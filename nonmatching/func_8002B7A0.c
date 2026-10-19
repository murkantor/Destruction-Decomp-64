typedef struct { u8 pad[0x10]; s8 x10; } CarInfo;
typedef struct {
    u8 pad[0x28]; s32 x28; u8 pad2C[4]; s32 x30; s32 x34; u8 pad38[4]; CarInfo *x3C; u8 x40;
    u8 pad41[0x58 - 0x41]; s32 x58; u8 pad5C[0xBE4 - 0x5C]; u8 xBE4;
} Car;
typedef struct { u8 pad[4]; s32 x4; u8 pad8[0x24 - 8]; s32 x24; } Event;
typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game;
extern Game *gGame;
extern void *gUi;
extern s32 D_800958A8;
extern s32 D_800958B4;
extern s32 D_80095850[];
extern u8 D_80091B14[];
extern void *D_8011EEF8;
s32 func_8001B298(s32);
void func_8001F6C4(Car *, u8);
void func_8001F000(Car *, u8, s32);
void func_80007FD4(void *, Car *);
void func_8001A22C(void);
void func_8001A220(void);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);
void Msg_Queue(s32, s32, s32, s32);
void Announcer_QueueRandom(s32 *list, s32 n, s32 arg, u8 force, u8 unique);

static inline void HideMarker(Car *car) {
    if (D_80091B14[car->x3C->x10]) {
        D_80091B14[car->x3C->x10] = 0;
        Ui_RemoveElement(gUi, car->x3C->x10 + 0xE2);
    }
}

#define SHOW_MARKER(car) \
    if (!D_80091B14[car->x3C->x10]) { \
        D_80091B14[car->x3C->x10] = 1; \
        Ui_AddElement(gUi, 2, car->x3C->x10 + 0xE2); \
        if (gGame->x10FC == 0x1C) { \
            list[0] = 0x86; \
            list[1] = 0x87; \
            if (D_800958A8 == 2) { \
                Msg_Queue(0x87, 0, 1, 0); \
            } else { \
                Announcer_QueueRandom(list, 2, 0, 1, 0); \
            } \
        } \
    }

void func_8002B7A0(Car *car, Event *ev, u8 flag) {
    s32 a;
    s32 b;
    s32 i;
    s32 list[2];

    switch (ev->x4) {
    case 0:
        break;
    case 1:
        if (D_800958A8 == 2) {
            if (D_800958B4 != 8 || ev->x24 != D_80095850[func_8001B298(car->x34)]) {
                func_8001F6C4(car, flag);
            }
        }
        break;
    case 2:
        func_80007FD4(D_8011EEF8, car);
        if (flag) {
            func_8001A22C();
            car->xBE4 = 0;
        } else {
            func_8001A220();
            car->xBE4 = 1;
        }
        break;
    case 3:
        if (D_800958A8 != 2) {
            if (D_800958B4 == 8) {
                i = func_8001B298(car->x34);
                if (D_80095850[i] == ev->x24) {
                    break;
                }
                D_80095850[i] = ev->x24;
            }
            func_8001F000(car, flag, ev->x24);
        }
        break;
    case 4:
        a = 0;
        b = 0;
        if (gGame->x10FC == 0x19) {
            break;
        }
        if (D_800958B4 == 8 && (ev->x24 - car->x28 < -25 || ev->x24 - car->x28 > 25) &&
            !D_80091B14[car->x3C->x10]) {
            D_80091B14[car->x3C->x10] = 1;
            Ui_AddElement(gUi, 2, car->x3C->x10 + 0xE2);
            if (D_800958A8 != 3 && gGame->x10FC == 0x1C) {
                list[0] = 0x86;
                list[1] = 0x87;
                if (D_800958A8 == 2) {
                    Msg_Queue(0x87, 0, 1, 0);
                } else {
                    Announcer_QueueRandom(list, 2, 0, 1, 0);
                }
            }
        }
        switch (D_800958B4) {
        case 4:
            if (ev->x24 == 0) {
                a = 0x2D;
                b = 0;
            } else {
                a = 0;
                b = 0x152;
            }
            break;
        case 5:
            a = 0x29;
            b = 0x18;
            break;
        case 7:
            if (ev->x24 == 0) {
                a = 0x2E;
                b = 0;
            } else {
                a = 0;
                b = 8;
            }
            break;
        case 9:
            if (ev->x24 == 0) {
                a = 0x4A;
                b = 0x38;
            } else {
                a = 0xBB;
                b = 0xAC;
            }
            break;
        case 10:
            a = 0x28;
            b = 0xE;
            break;
        }
        if ((a != 0) | (b != 0)) {
            if (flag) {
                car->x28 = a;
                if (car->x30 == 0) {
                    if (car->x58 == 1) {
                        HideMarker(car);
                    } else {
                        SHOW_MARKER(car)
                    }
                    car->x40 = 1;
                }
            } else {
                car->x28 = b;
                if (car->x30 == 0) {
                    if (car->x58 == -1) {
                        HideMarker(car);
                    } else {
                        SHOW_MARKER(car)
                    }
                    car->x40 = 1;
                }
            }
        }
        break;
    }
}
