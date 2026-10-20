/*
 * Video: graphics tasks go to Fast3D (port/fast3d), and the VI swap presents
 * the frame. Everything here runs on the host main thread, which owns the
 * window and the GL context.
 *
 * When frames are shown at another rate than the game's 60 Hz (a faster
 * display, or max_fps), and [video] interpolation is on, the VI swap does not
 * present: the main loop does, as often as the display and the cap allow, each
 * time drawing the game's newest frame blended from the one before
 * (gfx_interp_present). The game itself still runs at 60 Hz.
 */
#include <SDL3/SDL.h>

#include "port.h"
#include "port_config.h"
#include "gfx_api.h"
#include "gfx_opengl.h"

extern struct GfxWindowManagerAPI gfx_sdl;
extern bool gfx_show_fps;

static int sBlack;
static int sInterp;       /* frames in between are drawn (the mode, 0 = off) */
static u64 sNextPresentNs; /* the cap's next frame, while interpolating */

extern uint32_t gfx_interp_mode;
void gfx_interp_set_mode(uint32_t mode);
bool gfx_interp_present(bool black);
float gfx_sdl_refresh_rate(void);

#define GAME_RATE 60

/* How often frames can be shown: the cap, and the display with vsync on */
static float presentRate(void) {
    float refresh = gfx_sdl_refresh_rate(), cap = (float)gPortConfig.maxFps;

    if (gPortConfig.vsync != 0 && refresh > 0.0f) {
        return cap > 0.0f && cap < refresh ? cap : refresh;
    }
    return cap > 0.0f ? cap : 1000.0f;
}

/* Interpolation is only worth its blend where frames are not shown at the game's rate */
static void updateInterp(void) {
    float rate = presentRate();
    int mode = gPortConfig.interpolation != 0 && SDL_fabsf(rate - GAME_RATE) > 1.5f ? gPortConfig.interpolation : 0;

    if (mode != sInterp) {
        sInterp = mode;
        gfx_interp_set_mode((uint32_t)mode);
        Port_Log("video: frames shown at up to %.0f Hz; interpolation %s", rate,
                 mode == 0 ? "off" : mode == 2 ? "on (extrapolating, no added lag)" : "on");
    }
}

void Port_VideoInit(void) {
    struct GfxInitSettings set;

    SDL_zero(set);
    set.wapi = &gfx_sdl;
    set.rapi = &gfx_opengl_api;
    set.window_settings.title = "Destruction Derby 64";
    set.window_settings.width = gPortConfig.windowWidth;
    set.window_settings.height = gPortConfig.windowHeight;
    set.window_settings.fullscreen = gPortConfig.fullscreen;
    set.window_settings.fullscreen_is_exclusive = gPortConfig.fullscreenExclusive;
    set.window_settings.allow_hidpi = true;
    gfx_msaa_level = gPortConfig.msaa;
    gfx_n64_aa = gPortConfig.n64AA;
    gfx_anisotropy = gPortConfig.anisotropy;
    gfx_internal_height = gPortConfig.internalHeight;
    gfx_aspect_mode = gPortConfig.stretch ? 1 : gPortConfig.widescreen ? 2 : 0;
    gfx_init(&set);
    gfx_set_vsync(gPortConfig.vsync);
    gfx_set_texture_filter(gPortConfig.textureFilter);
    gfx_show_fps = gPortConfig.showFps != 0;
    updateInterp();
}

void gfx_sdl_set_window(bool fullscreen, bool exclusive, int width, int height);

/* The PC options page changed [video] settings: apply them (main thread) */
void Port_VideoApplySettings(void) {
    gfx_msaa_level = gPortConfig.msaa;
    gfx_n64_aa = gPortConfig.n64AA;
    gfx_internal_height = gPortConfig.internalHeight;
    gfx_aspect_mode = gPortConfig.stretch ? 1 : gPortConfig.widescreen ? 2 : 0;
    gfx_show_fps = gPortConfig.showFps != 0;
    gfx_set_vsync(gPortConfig.vsync);
    gfx_set_texture_filter(gPortConfig.textureFilter);
    gfx_sdl_set_window(gPortConfig.fullscreen, gPortConfig.fullscreenExclusive, gPortConfig.windowWidth,
                       gPortConfig.windowHeight);
    sNextPresentNs = 0;
    updateInterp();
}

/* The N64 address of the microcode a task starts with (F3DEX2 or S2DEX2) */
static u32 ucodeAddr(const void *p) {
    u32 v = (u32)p;

    if (v >= 0x10000000 && v < 0x80000000) {
        return Port_HostToN64(p);
    }
    return v;
}

void Port_VideoRunTask(OSTask *task) {
    gfx_run(task->t.data_ptr, ucodeAddr(task->t.ucode));
    gfx_end_frame();
}

void Port_VideoSwap(void *framebuffer) {
    if (!sInterp) {
        gfx_present(sBlack);
    }
}

/* Main loop, each time round: while interpolating, show a frame when the cap allows */
void Port_VideoPresentInterp(void) {
    static unsigned checks;
    u64 now;

    if (++checks % 256 == 0) {
        updateInterp(); /* the window may have moved to another display */
    }
    if (!sInterp) {
        return;
    }
    now = SDL_GetTicksNS();
    if (gPortConfig.maxFps > 0) {
        u64 interval = 1000000000ULL / (u64)gPortConfig.maxFps;
        if (now < sNextPresentNs) {
            return;
        }
        sNextPresentNs = sNextPresentNs + interval > now ? sNextPresentNs + interval : now + interval;
    }
    {
        static int logTimes = -1;
        static u64 spent;
        static unsigned n;

        gfx_interp_present(sBlack);
        if (logTimes < 0) {
            logTimes = SDL_getenv("DD64_GFXLOG") != NULL;
        }
        if (logTimes) {
            spent += SDL_GetTicksNS() - now;
            if (++n == 120) {
                Port_Log("video: an interpolated frame took %.2f ms to draw and show", spent / 120 / 1e6);
                spent = 0;
                n = 0;
            }
        }
    }
}

/* The shortest time between frames, for Fps_Update: one field at the game's
   rate (the N64's fastest), or longer for a lower [video] max_fps without
   interpolation (30: two fields a frame, as the N64 often ran). With
   interpolation the game keeps its rate whatever the display does. */
u64 Port_FrameIntervalNs(s32 rate) {
    s32 fps = gPortConfig.maxFps;

    if (sInterp || fps <= 0 || fps > rate) {
        fps = rate;
    }
    return 1000000000ULL / (u64)fps;
}

/* Fps_Update, each game frame: the fields it took (DD64_GFXLOG: a histogram) */
void Port_NoteGameFrame(s32 fields) {
    static int logTimes = -1;
    static unsigned hist[4], n;

    if (logTimes < 0) {
        logTimes = SDL_getenv("DD64_GFXLOG") != NULL;
    }
    if (!logTimes) {
        return;
    }
    hist[fields < 1 ? 0 : fields > 3 ? 3 : fields]++;
    if (++n == 120) {
        Port_Log("video: last 120 game frames took 1 field: %u, 2: %u, 3 or more: %u (0: %u)", hist[1], hist[2],
                 hist[3], hist[0]);
        hist[0] = hist[1] = hist[2] = hist[3] = 0;
        n = 0;
    }
}

void Port_VideoSetBlack(int black) {
    sBlack = black;
}

void Port_VideoToggleFullscreen(void) {
    gPortConfig.fullscreen = !gfx_sdl.get_fullscreen_state();
    gfx_sdl.set_fullscreen(gPortConfig.fullscreen);
    Port_ConfigSave();
    updateInterp();
}
