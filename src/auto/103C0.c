#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;
typedef struct { u8 pad[0x10A8]; u64 clock; u8 pad10B0[0x10FC - 0x10B0]; s16 x10FC; } Game;
extern Game *gGame[];
extern char gSaveSlotText[][0x32];
extern char D_800B8270[];
u64 Time_Get(u64 *);
TimeHMS Time_Split(u64 t);
int sprintf(char *, const char *, ...);

static inline void FormatTime(TimeHMS hms, char *p) {
    sprintf(p, D_800B8270, hms.m, hms.s, hms.frac);
    if (hms.s < 10) {
        p[2] = '0';
    }
    if (hms.frac < 100) {
        if (hms.frac >= 10 && hms.frac < 100) {
            p[5] = '0';
        } else if (hms.frac < 10) {
            p[5] = '0';
        }
    }
    do { p[7] = 0; } while (0);
}

s32 Hud_FormatTimeLimit(s32 arg0, s8 slot) {
    u64 limit = 0x45D964B800ULL;
    u64 now;
    char buf[8];
    TimeHMS tmp;

    if (gGame[0]->x10FC == 0x1F) {
        return 0;
    }
    if (gGame[0]->x10FC != 0x1B) {
        now = Time_Get(&gGame[0]->clock);
        if (now >= limit) {
            tmp = Time_Split(0);
        } else {
            now = limit - now;
            tmp = Time_Split(now);
        }
    } else {
        tmp = Time_Split(limit);
    }
    FormatTime(tmp, buf);
    gSaveSlotText[slot + 9][0] = buf[0];
    gSaveSlotText[slot + 9][1] = buf[1];
    gSaveSlotText[slot + 9][2] = buf[2];
    gSaveSlotText[slot + 9][3] = buf[3];
    gSaveSlotText[slot + 9][4] = 0;
    return 0;
}
