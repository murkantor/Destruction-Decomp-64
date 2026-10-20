/*
 * Developer option: boot straight into a race, skipping the logos, the
 * Controller Pak screens and the menus. DD64_BOOT (environment) lists
 * key=value pairs, all optional:
 *
 *   track    0-11: 0 pit, 1 jungle, 2 industrial, 3 ground zero (arenas),
 *            4 city, 5 coastal, 6 swamp, 7 airport, 8 sewer, 9 desert,
 *            10 glacier, 11 death. Default 5.
 *   type     0 championship, 1 single race, 2 time trial (tracks 4-11). Default 1.
 *   cup      difficulty / cup 0-3. Default 0.
 *   car      the player's car 0-23 (spec 1000 + 100 * car). Default 0.
 *   players  1-4. Default 1.
 *
 * e.g. DD64_BOOT="track=5,type=1,car=3". This sets what the menus set and
 * starts the race at the course intro, as PakMenu_CheckPaks does after the
 * car select screen.
 */
#include <stdlib.h>
#include <string.h>

#include "common.h"

extern s32 D_800958A4; /* human players */
extern s32 D_800958A8; /* game type */
extern s32 D_800958AC; /* multiplayer sub-type */
extern s32 D_800958B0; /* cup */
extern s32 D_800958B4; /* track */
extern s32 D_800958BC; /* derby round */
extern s32 D_800958C0[4]; /* each player's car */
extern u8 D_800958A0;
void func_8002D0E0(s32 cup);
void Players_Reset(void);
void func_80063380(s32 mode);
void Game_SetNextMode(s32 mode, s32 arg);
void Port_Log(const char *fmt, ...);

static s32 option(const char *spec, const char *key, s32 def) {
    const char *p = strstr(spec, key);
    size_t n = strlen(key);

    while (p != NULL) {
        if ((p == spec || p[-1] == ',' || p[-1] == ' ') && p[n] == '=') {
            return atoi(p + n + 1);
        }
        p = strstr(p + 1, key);
    }
    return def;
}

/* An option within lo..hi (the game indexes tables with these unchecked) */
static s32 optionIn(const char *spec, const char *key, s32 def, s32 lo, s32 hi) {
    s32 v = option(spec, key, def);

    if (v < lo || v > hi) {
        Port_Log("devboot: %s=%d is not %d-%d; using %d", key, v, lo, hi, def);
        return def;
    }
    return v;
}

/* Called by Boot_LoadData once the game is initialised and its data loaded */
void Port_DevBoot(void) {
    const char *spec = getenv("DD64_BOOT");
    s32 i, car, players, menu;

    if (spec == NULL) {
        return;
    }
    car = optionIn(spec, "car", 0, 0, 23);
    players = optionIn(spec, "players", 1, 1, 4);
    D_800958A4 = players;
    D_800958A8 = optionIn(spec, "type", 1, 0, 2);
    D_800958AC = 0;
    D_800958B0 = optionIn(spec, "cup", 0, 0, 3);
    D_800958B4 = optionIn(spec, "track", 5, 0, 11);
    D_800958BC = 0;
    for (i = 0; i < 4; i++) {
        D_800958C0[i] = i == 0 ? car : i;
    }
    func_8002D0E0(D_800958B0);
    D_800958A0 = 1;
    Players_Reset();
    menu = option(spec, "menu", 0);
    if (menu > 0) {
        /* a menu screen instead of the race: menu=23 (0x17) is track select */
        Game_SetNextMode(menu, 1);
        Port_Log("devboot: menu mode %d", menu);
        return;
    }
    func_80063380(2);          /* major mode: race */
    Game_SetNextMode(0x1A, 1); /* course intro, then countdown and race */
    Port_Log("devboot: track %d type %d cup %d car %d players %d", D_800958B4, D_800958A8, D_800958B0, car,
             players);
}
