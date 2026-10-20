/*
 * SDL3 window and OpenGL context for Fast3D (GfxWindowManagerAPI).
 *
 * The window is created on the host main thread, which also owns the GL
 * context and pumps SDL events (port/src/events.c). With the SDL_gpu renderer
 * (gfx_sdl_use_gpu) the window has no GL context: gfx_sdlgpu.cpp claims it,
 * and does the swap and the vsync. Fullscreen is either
 * borderless desktop or an exclusive mode at the closest resolution.
 */
#include <SDL3/SDL.h>
#include <string.h>

#include "gfx_window_manager_api.h"
#include "gfx_screen_config.h"
#include "system.h"

static SDL_Window *wnd;
static SDL_GLContext ctx;
static int swap_interval = 1;
static bool fullscreen_state;
static bool fullscreen_exclusive;

/* Set before init: the SDL_gpu renderer draws (port/src/video.c) */
extern "C" bool gfx_sdl_use_gpu;
bool gfx_sdl_use_gpu;
extern "C" void gfx_sdlgpu_swap(void);
extern "C" bool gfx_sdlgpu_set_swap_interval(int interval);

extern "C" SDL_Window *gfx_sdl_window(void) {
    return wnd;
}

static void apply_fullscreen(bool enable) {
    if (enable) {
        if (fullscreen_exclusive) {
            SDL_DisplayID d = SDL_GetDisplayForWindow(wnd);
            const SDL_DisplayMode *desk = SDL_GetDesktopDisplayMode(d);
            SDL_DisplayMode mode;
            if (desk != NULL &&
                SDL_GetClosestFullscreenDisplayMode(d, desk->w, desk->h, desk->refresh_rate, true, &mode)) {
                SDL_SetWindowFullscreenMode(wnd, &mode);
            }
        } else {
            SDL_SetWindowFullscreenMode(wnd, NULL); /* borderless desktop */
        }
    }
    SDL_SetWindowFullscreen(wnd, enable);
    SDL_SyncWindow(wnd);
    fullscreen_state = enable;
}

static void gfx_sdl_init(const struct GfxWindowInitSettings *set) {
    static const int glver[][3] = {
        { 3, 0, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY }, /* has everything gfx_opengl uses */
        { 4, 1, SDL_GL_CONTEXT_PROFILE_CORE },          /* macOS */
        { 3, 2, SDL_GL_CONTEXT_PROFILE_CORE },
        { 3, 0, SDL_GL_CONTEXT_PROFILE_ES },
        { 2, 1, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY },
    };
    SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    int w = set->width ? (int)set->width : DESIRED_SCREEN_WIDTH;
    int h = set->height ? (int)set->height : DESIRED_SCREEN_HEIGHT;

    if (set->allow_hidpi) {
        flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
    }
    fullscreen_exclusive = set->fullscreen_is_exclusive;

    if (gfx_sdl_use_gpu) {
        wnd = SDL_CreateWindow(set->title, w, h, (flags & ~SDL_WINDOW_OPENGL) | SDL_WINDOW_VULKAN);
        if (wnd == NULL) {
            sysFatalError("Could not open a window:\n%s", SDL_GetError());
        }
        sysLogPrintf(LOG_NOTE, "SDL: window for SDL_gpu, video driver %s", SDL_GetCurrentVideoDriver());
        SDL_SetWindowPosition(wnd, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(wnd);
        if (set->fullscreen) {
            apply_fullscreen(true);
        }
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    for (size_t i = 0; i < sizeof(glver) / sizeof(glver[0]) && ctx == NULL; i++) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glver[i][0]);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glver[i][1]);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, glver[i][2]);
        wnd = SDL_CreateWindow(set->title, w, h, flags);
        if (wnd == NULL) {
            sysLogPrintf(LOG_WARNING, "SDL: no window for GL %d.%d: %s", glver[i][0], glver[i][1], SDL_GetError());
            continue;
        }
        ctx = SDL_GL_CreateContext(wnd);
        if (ctx == NULL) {
            sysLogPrintf(LOG_WARNING, "SDL: no GL %d.%d context: %s", glver[i][0], glver[i][1], SDL_GetError());
            SDL_DestroyWindow(wnd);
            wnd = NULL;
        } else {
            sysLogPrintf(LOG_NOTE, "SDL: GL %d.%d context, video driver %s", glver[i][0], glver[i][1],
                         SDL_GetCurrentVideoDriver());
        }
    }
    if (wnd == NULL || ctx == NULL) {
        sysFatalError("Could not open a window with an OpenGL context:\n%s", SDL_GetError());
    }

    SDL_SetWindowPosition(wnd, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_GL_MakeCurrent(wnd, ctx);
    SDL_GL_SetSwapInterval(swap_interval);
    SDL_ShowWindow(wnd);
    if (set->fullscreen) {
        apply_fullscreen(true);
    }
}

static int32_t gfx_sdl_get_fullscreen_state(void) {
    return fullscreen_state;
}

static void gfx_sdl_set_fullscreen(bool enable) {
    if (enable != fullscreen_state) {
        apply_fullscreen(enable);
    }
}

static void gfx_sdl_get_dimensions(uint32_t *width, uint32_t *height, int32_t *posX, int32_t *posY) {
    int w = 0, h = 0, x = 0, y = 0;

    SDL_GetWindowSizeInPixels(wnd, &w, &h);
    SDL_GetWindowPosition(wnd, &x, &y);
    *width = w;
    *height = h;
    if (posX != NULL) {
        *posX = x;
    }
    if (posY != NULL) {
        *posY = y;
    }
}

static void gfx_sdl_swap_buffers_begin(void) {
    if (gfx_sdl_use_gpu) {
        gfx_sdlgpu_swap();
    } else {
        SDL_GL_SwapWindow(wnd);
    }
}

static void gfx_sdl_swap_buffers_end(void) {
}

static bool gfx_sdl_set_swap_interval(int interval) {
    if (gfx_sdl_use_gpu) {
        swap_interval = interval;
        return gfx_sdlgpu_set_swap_interval(interval);
    }
    if (interval == 2) {
        interval = 1; /* VRR: vsync, under a cap (port/src/video.c) */
    }
    /* -1 is adaptive vsync; fall back to plain vsync where the driver lacks it */
    if (!SDL_GL_SetSwapInterval(interval)) {
        if (interval != -1 || !SDL_GL_SetSwapInterval(1)) {
            return false;
        }
        interval = 1;
    }
    swap_interval = interval;
    return true;
}

struct GfxWindowManagerAPI gfx_sdl = {
    gfx_sdl_init,
    gfx_sdl_get_fullscreen_state,
    gfx_sdl_set_fullscreen,
    gfx_sdl_get_dimensions,
    gfx_sdl_swap_buffers_begin,
    gfx_sdl_swap_buffers_end,
    gfx_sdl_set_swap_interval,
};

/* The refresh rate of the display the window is on, Hz (0 if unknown) */
extern "C" float gfx_sdl_refresh_rate(void) {
    SDL_DisplayID d = wnd != NULL ? SDL_GetDisplayForWindow(wnd) : 0;
    const SDL_DisplayMode *m = d != 0 ? SDL_GetCurrentDisplayMode(d) : NULL;
    return m != NULL ? m->refresh_rate : 0.0f;
}

/* The PC options: windowed (at a size) or fullscreen, borderless or exclusive */
extern "C" void gfx_sdl_set_window(bool fullscreen, bool exclusive, int width, int height) {
    if (wnd == NULL) {
        return;
    }
    if (!fullscreen) {
        if (fullscreen_state) {
            apply_fullscreen(false);
        }
        SDL_SetWindowSize(wnd, width, height);
        SDL_SyncWindow(wnd);
        return;
    }
    if (!fullscreen_state || exclusive != fullscreen_exclusive) {
        fullscreen_exclusive = exclusive;
        if (fullscreen_state) {
            apply_fullscreen(false);
        }
        apply_fullscreen(true);
    }
}
