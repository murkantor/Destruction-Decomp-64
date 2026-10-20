/*
 * dd64.ini: settings next to the executable. Written with every setting and
 * its default on first run; unknown keys are kept when it is saved again.
 */
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "port.h"
#include "port_config.h"
#include "port_ini.h"

PortConfig gPortConfig;

#define MAX_ENTRIES 512

typedef struct {
    char section[32];
    char key[48];
    char value[128];
} IniEntry;

static IniEntry sEntries[MAX_ENTRIES];
static int sNumEntries;
static char sBaseDir[1024];
static char sIniPath[1100];

const char *Port_BaseDir(void) {
    if (sBaseDir[0] == '\0') {
        const char *b = SDL_GetBasePath();
        SDL_strlcpy(sBaseDir, b != NULL ? b : "", sizeof(sBaseDir));
    }
    return sBaseDir;
}

static char *trim(char *s) {
    char *e;

    while (*s == ' ' || *s == '\t') {
        s++;
    }
    e = s + strlen(s);
    while (e > s && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\r' || e[-1] == '\n')) {
        *--e = '\0';
    }
    return s;
}

static IniEntry *find(const char *section, const char *key) {
    int i;

    for (i = 0; i < sNumEntries; i++) {
        if (SDL_strcasecmp(sEntries[i].section, section) == 0 && SDL_strcasecmp(sEntries[i].key, key) == 0) {
            return &sEntries[i];
        }
    }
    return NULL;
}

const char *Port_IniGet(const char *section, const char *key, const char *def) {
    IniEntry *e = find(section, key);

    return e != NULL ? e->value : def;
}

int Port_IniGetInt(const char *section, const char *key, int def) {
    IniEntry *e = find(section, key);

    return e != NULL ? atoi(e->value) : def;
}

void Port_IniSet(const char *section, const char *key, const char *value) {
    IniEntry *e = find(section, key);

    if (e == NULL) {
        if (sNumEntries == MAX_ENTRIES) {
            return;
        }
        e = &sEntries[sNumEntries++];
        SDL_strlcpy(e->section, section, sizeof(e->section));
        SDL_strlcpy(e->key, key, sizeof(e->key));
    }
    SDL_strlcpy(e->value, value, sizeof(e->value));
}

void Port_IniSetInt(const char *section, const char *key, int value) {
    char buf[32];

    SDL_snprintf(buf, sizeof(buf), "%d", value);
    Port_IniSet(section, key, buf);
}

static void parse(const char *text) {
    char section[32] = "";
    char line[512];
    const char *p = text;

    while (*p != '\0') {
        size_t n = strcspn(p, "\n");
        char *s, *eq;

        if (n >= sizeof(line)) {
            n = sizeof(line) - 1;
        }
        memcpy(line, p, n);
        line[n] = '\0';
        p += strcspn(p, "\n");
        if (*p == '\n') {
            p++;
        }
        s = trim(line);
        if (*s == '\0' || *s == ';' || *s == '#') {
            continue;
        }
        if (*s == '[') {
            char *e = strchr(s, ']');
            if (e != NULL) {
                *e = '\0';
                SDL_strlcpy(section, trim(s + 1), sizeof(section));
            }
            continue;
        }
        eq = strchr(s, '=');
        if (eq != NULL) {
            *eq = '\0';
            Port_IniSet(section, trim(s), trim(eq + 1));
        }
    }
}

/* Every setting with its default and a line of help, in file order */
typedef struct {
    const char *section, *key;
    int *field;
    int def;
    const char *help;
} Setting;

static const Setting sSettings[] = {
    { "video", "window_width", &gPortConfig.windowWidth, 960, "Window size in pixels" },
    { "video", "window_height", &gPortConfig.windowHeight, 720, NULL },
    { "video", "fullscreen", &gPortConfig.fullscreen, 0, "1 = start fullscreen (Alt+Enter or F11 toggles)" },
    { "video", "fullscreen_exclusive", &gPortConfig.fullscreenExclusive, 0,
      "0 = borderless desktop fullscreen, 1 = exclusive mode" },
    { "video", "vsync", &gPortConfig.vsync, 1, "0 = off, 1 = on, -1 = adaptive" },
    { "video", "max_fps", &gPortConfig.maxFps, 60,
      "Frame cap: 30 (the N64's usual rate; Steam Deck) or 60. The game swaps frames on the 60 Hz video retrace, as on the N64, so other caps round to these" },
    { "video", "msaa", &gPortConfig.msaa, 4, "Multisample anti-aliasing: 1 (off), 2, 4 or 8" },
    { "video", "n64_aa", &gPortConfig.n64AA, 0, "1 = resolve anti-aliasing the way the N64 did (softer)" },
    { "video", "anisotropy", &gPortConfig.anisotropy, 1, "Anisotropic filtering: 1 (off) to 16" },
    { "video", "internal_height", &gPortConfig.internalHeight, 0,
      "Lines the game is drawn at: 0 = the window's, 240 = the N64's" },
    { "video", "widescreen", &gPortConfig.widescreen, 1,
      "1 = a wide window shows more of the 3D view, unstretched, with the HUD and menus 4:3 in the middle" },
    { "video", "stretch", &gPortConfig.stretch, 0, "0 = keep 4:3 with side bars, 1 = stretch to the window" },
    { "video", "texture_filter", &gPortConfig.textureFilter, 1, "0 = nearest, 1 = bilinear, 2 = three-point (N64)" },
    { "audio", "volume", &gPortConfig.volume, 100, "Master volume, 0-100" },
    { "audio", "latency", &gPortConfig.audioLatency, 80,
      "Output delay in milliseconds (N64: about 20); raise it if the sound stutters" },
    { "input", "rumble", &gPortConfig.rumble, 1, "1 = Rumble Pak on every controller that can rumble" },
    { "input", "rumble_strength", &gPortConfig.rumbleStrength, 100, "0-100" },
    { "input", "deadzone", &gPortConfig.deadzone, 12, "Stick dead zone, percent of full tilt" },
    { "input", "stick_range", &gPortConfig.stickRange, 80,
      "Stick value at full tilt: an N64 controller reads about 80, the maximum is 127" },
    { "input", "keyboard_player", &gPortConfig.keyboardPlayer, 1, "Controller the keyboard drives: 1-4, 0 = none" },
    { "saves", "pak_on_all_ports", &gPortConfig.pakOnAllPorts, 0,
      "1 = every controller has a Controller Pak; 0 = only controller 1" },
};

void Port_ConfigLoad(void) {
    size_t size;
    char *text;
    size_t i;

    SDL_snprintf(sIniPath, sizeof(sIniPath), "%sdd64.ini", Port_BaseDir());
    text = SDL_LoadFile(sIniPath, &size);
    if (text != NULL) {
        parse(text);
        SDL_free(text);
    }
    for (i = 0; i < SDL_arraysize(sSettings); i++) {
        *sSettings[i].field = Port_IniGetInt(sSettings[i].section, sSettings[i].key, sSettings[i].def);
    }
    if (text == NULL) {
        Port_ConfigSave();
    }
}

void Port_ConfigSave(void) {
    FILE *f = fopen(sIniPath, "w");
    const char *section = "";
    size_t i;
    int j;

    if (f == NULL) {
        Port_Log("could not write %s", sIniPath);
        return;
    }
    fprintf(f, "; Destruction Derby 64 settings. Delete this file to restore the defaults.\n");
    for (i = 0; i < SDL_arraysize(sSettings); i++) {
        const Setting *s = &sSettings[i];
        Port_IniSetInt(s->section, s->key, *s->field);
        if (strcmp(section, s->section) != 0) {
            section = s->section;
            fprintf(f, "\n[%s]\n", section);
            /* entries other code keeps in this section (key bindings and the like) */
        }
        if (s->help != NULL) {
            fprintf(f, "; %s\n", s->help);
        }
        fprintf(f, "%s = %d\n", s->key, *s->field);
    }
    /* everything else, by section */
    for (j = 0; j < sNumEntries; j++) {
        int known = 0;
        for (i = 0; i < SDL_arraysize(sSettings); i++) {
            if (SDL_strcasecmp(sEntries[j].section, sSettings[i].section) == 0 &&
                SDL_strcasecmp(sEntries[j].key, sSettings[i].key) == 0) {
                known = 1;
                break;
            }
        }
        if (!known) {
            int first = 1, k;
            for (k = 0; k < j; k++) {
                if (SDL_strcasecmp(sEntries[k].section, sEntries[j].section) == 0) {
                    first = 0;
                    break;
                }
            }
            if (first) {
                fprintf(f, "\n[%s]\n", sEntries[j].section);
                for (k = j; k < sNumEntries; k++) {
                    int known2 = 0;
                    size_t m;
                    if (SDL_strcasecmp(sEntries[k].section, sEntries[j].section) != 0) {
                        continue;
                    }
                    for (m = 0; m < SDL_arraysize(sSettings); m++) {
                        if (SDL_strcasecmp(sEntries[k].section, sSettings[m].section) == 0 &&
                            SDL_strcasecmp(sEntries[k].key, sSettings[m].key) == 0) {
                            known2 = 1;
                        }
                    }
                    if (!known2) {
                        fprintf(f, "%s = %s\n", sEntries[k].key, sEntries[k].value);
                    }
                }
            }
        }
    }
    fclose(f);
}
