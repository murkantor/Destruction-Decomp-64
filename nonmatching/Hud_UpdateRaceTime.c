typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;
typedef struct { u8 pad[0xC]; s16 xC; u8 padE[0x10A8 - 0xE]; u64 x10A8; u8 pad10B0[0x10FC - 0x10B0]; s16 x10FC; } Game;
typedef struct { u8 pad[0x60]; u64 x60; u64 laps[4]; s32 x88; } CarData;
typedef struct { CarData *x0; u8 pad[0x64]; } Car;
typedef struct { s32 x0; s32 x4; u8 pad[0x10]; } Rec18;

extern Game *gGame;
extern Car D_800958F0[];
extern s32 D_800958B4;
extern s32 D_80095FF8[];
extern s32 D_80095908;
extern u8 D_80097335;
extern Rec18 D_80130528[];
extern char D_8007F828[];
u64 Time_Get(u64 *);
TimeHMS Time_Split(u64 t);
void Time_Format(TimeHMS t, char *buf, s32 len);
char *strcpy(char *, const char *);
void func_80063380(s32);
void func_80063564(s32, s32);
void func_800100BC(s32, s32);

s32 Hud_UpdateRaceTime(s32 arg0, s8 idx) {
    u64 t = 0;
    char buf[0x10];
    TimeHMS hms;
    s8 laps;
    s8 i;

    if (gGame->xC == 2) {
        if (gGame->x10FC == 0x1F || gGame->x10FC == 0x21) {
            return 0;
        }
        laps = D_800958F0[idx].x0->x88 < 0 ? 0 : D_800958F0[idx].x0->x88;
        if (gGame->x10FC != 0x1B) {
            if (laps < D_80095FF8[D_800958B4]) {
                t = Time_Get(&gGame->x10A8) - D_800958F0[idx].x0->x60;
            }
            for (i = 0; i < laps; i++) {
                t += D_800958F0[idx].x0->laps[i];
            }
        }
        hms = Time_Split(t);
        if ((f64)t >= 11280000000000.0) {
            func_80063380(2);
            func_80063564(0x19, 1);
            D_80095908 = 0;
        }
        Time_Format(hms, buf, 9);
        strcpy(D_8007F828, buf);
        if (D_80097335) {
            if (D_80130528[D_800958B4].x0 || D_80130528[D_800958B4].x4) {
                func_800100BC(0x1FF, 0x111FC);
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }
    return 0;
}
