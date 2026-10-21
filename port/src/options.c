/*
 * The PC options: pages of settings for the in-game PC options page
 * (port/game/pcmenu.c draws it), each a [section] key of dd64.ini. A change takes
 * effect at once where it can; the ones that touch the window or the renderer
 * are applied on the main thread (Port_OptionsApplyPending), which also saves
 * the file. Rows marked "restart" take effect the next time the game starts.
 */
#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>

#include "port.h"
#include "port_config.h"

typedef struct {
    const char *label;
    int *field;
    const int *values; /* the setting's value for each choice */
    const char *const *names;
    int count;
    int video;         /* applied on the main thread (the window, the renderer) */
    int restart;
} Option;

#define N(a) (int)(sizeof(a) / sizeof((a)[0]))

static const int kOffOn[] = { 0, 1 };
static const char *const kOffOnN[] = { "off", "on" };

static const int kDisplay[] = { 0, 1, 2 }; /* window, borderless fullscreen, exclusive (two fields) */
static const char *const kDisplayN[] = { "window", "fullscreen", "exclusive" };
static int sDisplay;

static const int kWindow[] = { 0, 1, 2, 3, 4, 5 };
static const char *const kWindowN[] = { "960x720", "1280x720", "1280x960", "1600x900", "1920x1080", "2560x1440" };
static const int kWindowW[] = { 960, 1280, 1280, 1600, 1920, 2560 };
static const int kWindowH[] = { 720, 720, 960, 900, 1080, 1440 };
static int sWindow;

static const int kAspect[] = { 0, 1, 2 }; /* 4:3, widescreen, stretched (two fields) */
static const char *const kAspectN[] = { "4:3", "widescreen", "stretched" };
static int sAspect;

static const int kVsync[] = { 0, 1, -1, 2 };
static const char *const kVsyncN[] = { "off", "on", "adaptive", "VRR" };
static const int kFps[] = { 0, 30, 40, 60, 75, 90, 120, 144, 165, 240 };
static const char *const kFpsN[] = { "no limit", "30", "40", "60", "75", "90", "120", "144", "165", "240" };
static const int kInterp[] = { 0, 1, 2 };
static const char *const kInterpN[] = { "off", "smooth", "low latency" };
static const int kMsaa[] = { 1, 2, 4, 8 };
static const char *const kMsaaN[] = { "off", "2x", "4x", "8x" };
static const int kFilter[] = { 0, 1, 2 };
static const char *const kFilterN[] = { "nearest", "bilinear", "n64 3-point" };
static const int kAniso[] = { 1, 2, 4, 8, 16 };
static const char *const kAnisoN[] = { "off", "2x", "4x", "8x", "16x" };
static const int kRenderer[] = { 0, 1 };
static const char *const kRendererN[] = { "opengl", "sdl gpu - vulkan" };
static const int kRes[] = { 0, 240, 480, 720, 1080 };
static const char *const kResN[] = { "window", "240 - n64", "480", "720", "1080" };

static const int kVolume[] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
static const char *const kVolumeN[] = { "0%", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%" };
static const int kLatency[] = { 40, 60, 80, 100, 150 };
static const char *const kLatencyN[] = { "40 ms", "60 ms", "80 ms", "100 ms", "150 ms" };

static const int kStrength[] = { 25, 50, 75, 100 };
static const char *const kStrengthN[] = { "25%", "50%", "75%", "100%" };
static const int kDeadzone[] = { 0, 4, 8, 12, 16, 20, 25, 30 };
static const char *const kDeadzoneN[] = { "0%", "4%", "8%", "12%", "16%", "20%", "25%", "30%" };
static const int kRange[] = { 64, 70, 75, 80, 85, 90, 100, 110, 127 };
static const char *const kRangeN[] = { "64", "70", "75", "80 - n64", "85", "90", "100", "110", "127" };
static const int kKeyboard[] = { 0, 1, 2, 3, 4 };
static const char *const kKeyboardN[] = { "nobody", "player 1", "player 2", "player 3", "player 4" };
static const int kPaks[] = { 0, 1 };
static const char *const kPaksN[] = { "player 1 only", "every player" };

static const Option kVideo[] = {
    { "display", &sDisplay, kDisplay, kDisplayN, N(kDisplay), 1, 0 },
    { "window size", &sWindow, kWindow, kWindowN, N(kWindow), 1, 0 },
    { "screen shape", &sAspect, kAspect, kAspectN, N(kAspect), 1, 0 },
    { "vsync", &gPortConfig.vsync, kVsync, kVsyncN, N(kVsync), 1, 0 },
    { "frame limit", &gPortConfig.maxFps, kFps, kFpsN, N(kFps), 1, 0 },
    { "interpolation", &gPortConfig.interpolation, kInterp, kInterpN, N(kInterp), 1, 0 },
    { "3d resolution", &gPortConfig.internalHeight, kRes, kResN, N(kRes), 1, 0 },
    { "anti-aliasing", &gPortConfig.msaa, kMsaa, kMsaaN, N(kMsaa), 1, 0 },
    { "n64 aa blur", &gPortConfig.n64AA, kOffOn, kOffOnN, N(kOffOn), 1, 0 },
    { "texture filter", &gPortConfig.textureFilter, kFilter, kFilterN, N(kFilter), 1, 0 },
    { "clean cut-outs", &gPortConfig.edgeFill, kOffOn, kOffOnN, N(kOffOn), 1, 0 },
    { "anisotropy", &gPortConfig.anisotropy, kAniso, kAnisoN, N(kAniso), 1, 1 },
    { "fps counter", &gPortConfig.showFps, kOffOn, kOffOnN, N(kOffOn), 1, 0 },
    { "renderer", &gPortConfig.renderer, kRenderer, kRendererN, N(kRenderer), 0, 1 },
};
static const Option kAudio[] = {
    { "master volume", &gPortConfig.volume, kVolume, kVolumeN, N(kVolume), 0, 0 },
    { "audio delay", &gPortConfig.audioLatency, kLatency, kLatencyN, N(kLatency), 0, 1 },
};
static const Option kGame[] = {
    { "double field", &gPortConfig.doubleField, kOffOn, kOffOnN, N(kOffOn), 0, 0 },
    { "cpu unlockables", &gPortConfig.cpuUnlockables, kOffOn, kOffOnN, N(kOffOn), 0, 0 },
};
static const Option kControls[] = {
    { "rumble", &gPortConfig.rumble, kOffOn, kOffOnN, N(kOffOn), 0, 0 },
    { "rumble strength", &gPortConfig.rumbleStrength, kStrength, kStrengthN, N(kStrength), 0, 0 },
    { "stick dead zone", &gPortConfig.deadzone, kDeadzone, kDeadzoneN, N(kDeadzone), 0, 0 },
    { "stick range", &gPortConfig.stickRange, kRange, kRangeN, N(kRange), 0, 0 },
    { "right stick camera", &gPortConfig.freeCamera, kOffOn, kOffOnN, N(kOffOn), 0, 0 },
    { "analog pedals", &gPortConfig.analogPedals, kOffOn, kOffOnN, N(kOffOn), 0, 0 },
    { "keyboard drives", &gPortConfig.keyboardPlayer, kKeyboard, kKeyboardN, N(kKeyboard), 0, 0 },
    { "raphnet adapter", &gPortConfig.raphnet, kOffOn, kOffOnN, N(kOffOn), 0, 1 },
    { "controller paks", &gPortConfig.pakOnAllPorts, kPaks, kPaksN, N(kPaks), 0, 1 },
};

static SDL_AtomicInt sApplyVideo, sSave; /* for the main thread */
static int sCaptureRow;

typedef struct {
    const char *name;
    const Option *opts;
    int count;
    int bind; /* a bindings page: 1 keyboard, 2 gamepad */
} Page;

static const Page kPages[] = {
    { "video", kVideo, N(kVideo), 0 },
    { "sound", kAudio, N(kAudio), 0 },
    { "controls", kControls, N(kControls), 0 },
    { "game", kGame, N(kGame), 0 },
    { "keys", NULL, 0, 1 },
    { "pad", NULL, 0, 2 },
};

int Port_InputCount(void);
const char *Port_InputLabel(int i);
const char *Port_InputBinding(int device, int i);
void Port_InputResetBinding(int device, int i);
void Port_InputBeginCapture(int device, int i);
int Port_InputCapturing(void);

static int sOpen;

/* The derived rows (display, window size, screen shape) from their settings */
static void loadDerived(void) {
    int i;

    sDisplay = !gPortConfig.fullscreen ? 0 : gPortConfig.fullscreenExclusive ? 2 : 1;
    sAspect = gPortConfig.stretch ? 2 : gPortConfig.widescreen ? 1 : 0;
    sWindow = 0;
    for (i = 0; i < N(kWindowW); i++) {
        if (kWindowW[i] == gPortConfig.windowWidth && kWindowH[i] == gPortConfig.windowHeight) {
            sWindow = i;
        }
    }
}

static void storeDerived(void) {
    gPortConfig.fullscreen = sDisplay != 0;
    gPortConfig.fullscreenExclusive = sDisplay == 2;
    gPortConfig.stretch = sAspect == 2;
    gPortConfig.widescreen = sAspect == 1;
    gPortConfig.windowWidth = kWindowW[sWindow];
    gPortConfig.windowHeight = kWindowH[sWindow];
}

static int choice(const Option *o) {
    int i, best = 0;

    for (i = 0; i < o->count; i++) {
        if (o->values[i] == *o->field) {
            return i;
        }
        /* not one of the choices (edited by hand): the nearest below it */
        if (o->values[i] < *o->field) {
            best = i;
        }
    }
    return best;
}

int Port_OptPages(void) {
    return N(kPages);
}

const char *Port_OptPageName(int page) {
    return kPages[page].name;
}

int Port_OptCount(int page) {
    return kPages[page].bind ? Port_InputCount() + 1 : kPages[page].count; /* + reset all */
}

const char *Port_OptLabel(int page, int i) {
    if (kPages[page].bind) {
        return i < Port_InputCount() ? Port_InputLabel(i) : "reset all";
    }
    return kPages[page].opts[i].label;
}

/* The row's value as shown */
const char *Port_OptValue(int page, int i) {
    if (kPages[page].bind) {
        static char text[48];
        int dev = kPages[page].bind - 1;
        if (i >= Port_InputCount()) {
            return "";
        }
        if (Port_InputCapturing() && i == sCaptureRow) {
            return dev == 0 ? "press a key" : "press a button";
        }
        SDL_strlcpy(text, Port_InputBinding(dev, i), sizeof(text));
        for (char *p = text; *p != '\0'; p++) {
            if (*p == ',') {
                *p = '/'; /* the font has no comma tail */
            }
        }
        return text[0] != '\0' ? text : "-";
    }
    {
        const Option *o = &kPages[page].opts[i];
        return o->names[choice(o)];
    }
}

int Port_OptNeedsRestart(int page, int i) {
    return kPages[page].bind ? 0 : kPages[page].opts[i].restart;
}

int Port_OptIsBindPage(int page) {
    return kPages[page].bind != 0;
}

int Port_OptCapturing(void) {
    return Port_InputCapturing();
}

/* Left (-1) or right (+1) on a row: the next choice, applied and saved. On a
   bindings page: right (or A) waits for the new key or button, left restores the
   default; on "reset all", either resets every input. */
void Port_OptChange(int page, int i, int dir) {
    if (kPages[page].bind) {
        int dev = kPages[page].bind - 1, k;
        if (i >= Port_InputCount()) {
            for (k = 0; k < Port_InputCount(); k++) {
                Port_InputResetBinding(dev, k);
            }
        } else if (dir < 0) {
            Port_InputResetBinding(dev, i);
        } else {
            sCaptureRow = i;
            Port_InputBeginCapture(dev, i);
            return;
        }
        SDL_SetAtomicInt(&sSave, 1);
        return;
    }
    const Option *o = &kPages[page].opts[i];
    int c = choice(o) + dir;

    if (c < 0) {
        c = o->count - 1;
    } else if (c >= o->count) {
        c = 0;
    }
    *o->field = o->values[c];
    storeDerived();
    if (o->video) {
        SDL_SetAtomicInt(&sApplyVideo, 1);
    }
    SDL_SetAtomicInt(&sSave, 1);
}

void Port_OptSetOpen(int open) {
    if (open) {
        loadDerived();
    }
    sOpen = open;
}

int Port_OptIsOpen(void) {
    return sOpen;
}

/* Main thread, each time round its loop */
void Port_OptionsApplyPending(void) {
    if (SDL_CompareAndSwapAtomicInt(&sApplyVideo, 1, 0)) {
        Port_VideoApplySettings();
    }
    if (SDL_CompareAndSwapAtomicInt(&sSave, 1, 0)) {
        Port_ConfigSave();
    }
}

/* Player 1's pad, as the game last read it (the game thread, osContGetReadData) */
static u16 sMenuButtons;
static s8 sMenuX, sMenuY;

void Port_InputNoteMenuPad(const OSContPad *pad) {
    sMenuButtons = pad->button;
    sMenuX = pad->stick_x;
    sMenuY = pad->stick_y;
}

void Port_OptMenuPad(u16 *buttons, s8 *x, s8 *y) {
    *buttons = sMenuButtons;
    *x = sMenuX;
    *y = sMenuY;
}
