#include "common.h"
#include "m2c.h"

typedef struct { u8 buf[0xC]; s32 len; u8 f10; u8 f11; u8 f12; u8 f13; u8 f14; u8 pad[3]; } Cheat;
typedef struct { u8 f0; u8 f1; u8 pad2[2]; Cheat c[4]; } CheatState;
typedef struct { u8 pad0[0xC]; u32 xC; u32 x10; u8 pad14[0x9C]; } Ctrl;
extern CheatState D_801253F8;
extern char D_800B9248[];
extern char D_800B924C[];
extern char D_800B9254[];
extern char D_800B9258[];
extern char D_800B925C[];
extern char D_800B9264[];
extern char D_800B9268[];
extern char D_800B9270[];
Ctrl *func_80062F14(void);
void *memset(void *, s32, u32);
int strcmp(const char *, u8 *);
void func_80063564(s32, s32);

static inline void Cheat_Reset(s8 p) {
    memset(D_801253F8.c[p].buf, 0, 0xB);
    D_801253F8.c[p].len = 0;
}

void Cheat_UpdateInput(void) {
    Ctrl *c;
    s32 i;

    c = func_80062F14();
    for (i = 0; i < 4; i++) {
        if (c[i].x10 & 0x10) {
            Cheat_Reset(i);
            continue;
        }
        if (D_801253F8.c[i].len < 10 && (c[i].xC & 0x10)) {
            if (c[i].x10 & 0x8000) {
                D_801253F8.c[i].buf[D_801253F8.c[i].len] = 'A';
            }
            if (c[i].x10 & 0x4000) {
                D_801253F8.c[i].buf[D_801253F8.c[i].len] = 'B';
            }
            if (c[i].x10 & 0x1) {
                D_801253F8.c[i].buf[D_801253F8.c[i].len] = 'R';
            }
            if (c[i].x10 & 0x2) {
                D_801253F8.c[i].buf[D_801253F8.c[i].len] = 'L';
            }
            if (c[i].x10 & 0x8) {
                D_801253F8.c[i].buf[D_801253F8.c[i].len] = 'U';
            }
            if (c[i].x10 & 0x4) {
                D_801253F8.c[i].buf[D_801253F8.c[i].len] = 'D';
            }
            if (D_801253F8.c[i].buf[D_801253F8.c[i].len] != 0) {
                D_801253F8.c[i].len++;
            }
            if (strcmp(D_800B9248, D_801253F8.c[i].buf) == 0) {
                D_801253F8.f0 = 1;
            }
            if (strcmp(D_800B924C, D_801253F8.c[i].buf) == 0) {
                D_801253F8.f1 = 1;
            }
            if (strcmp(D_800B9254, D_801253F8.c[i].buf) == 0) {
                D_801253F8.c[i].f10 = 1;
            }
            if (strcmp(D_800B9258, D_801253F8.c[i].buf) == 0) {
                D_801253F8.c[i].f11 = 1;
            }
            if (strcmp(D_800B925C, D_801253F8.c[i].buf) == 0) {
                D_801253F8.c[i].f12 = 1;
            }
            if (strcmp(D_800B9264, D_801253F8.c[i].buf) == 0) {
                D_801253F8.c[i].f13 = 1;
            }
            if (strcmp(D_800B9268, D_801253F8.c[i].buf) == 0) {
                D_801253F8.c[i].f14 = 1;
            }
            if (strcmp(D_800B9270, D_801253F8.c[i].buf) == 0) {
                func_80063564(0x2E, 1);
            }
        }
    }
}
