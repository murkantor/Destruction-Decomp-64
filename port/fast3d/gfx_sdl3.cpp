/*
 * SDL3 window and OpenGL context for Fast3D (GfxWindowManagerAPI).
 *
 * The window is created on the host main thread, which also owns the GL
 * context and pumps SDL events (port/src/events.c). Fullscreen is either
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
static int target_fps = 60;
static bool fullscreen_state;
static bool fullscreen_exclusive;
static void (*on_fullscreen_changed_callback)(bool is_now_fullscreen);

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
    if (on_fullscreen_changed_callback != NULL) {
        on_fullscreen_changed_callback(enable);
    }
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
    if (set->maximized) {
        flags |= SDL_WINDOW_MAXIMIZED;
    }
    fullscreen_exclusive = set->fullscreen_is_exclusive;

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

    if (set->has_position) {
        SDL_SetWindowPosition(wnd, set->x, set->y);
    } else {
        SDL_SetWindowPosition(wnd, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    SDL_GL_MakeCurrent(wnd, ctx);
    SDL_GL_SetSwapInterval(swap_interval);
    SDL_ShowWindow(wnd);
    if (set->fullscreen) {
        apply_fullscreen(true);
    }
}

static void gfx_sdl_close(void) {
    if (ctx != NULL) {
        SDL_GL_DestroyContext(ctx);
        ctx = NULL;
    }
    if (wnd != NULL) {
        SDL_DestroyWindow(wnd);
        wnd = NULL;
    }
}

static int gfx_sdl_get_display_mode(int modenum, int *out_w, int *out_h) {
    int count = 0;
    SDL_DisplayMode **modes = SDL_GetFullscreenDisplayModes(SDL_GetDisplayForWindow(wnd), &count);
    int ok = 0;

    if (modes != NULL && modenum >= 0 && modenum < count) {
        *out_w = modes[modenum]->w;
        *out_h = modes[modenum]->h;
        ok = 1;
    }
    SDL_free(modes);
    return ok;
}

static int gfx_sdl_get_current_display_mode(int *out_w, int *out_h) {
    const SDL_DisplayMode *m = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(wnd));

    if (m == NULL) {
        return 0;
    }
    *out_w = m->w;
    *out_h = m->h;
    return 1;
}

static int gfx_sdl_get_num_display_modes(void) {
    int count = 0;
    SDL_DisplayMode **modes = SDL_GetFullscreenDisplayModes(SDL_GetDisplayForWindow(wnd), &count);

    SDL_free(modes);
    return count;
}

static int32_t gfx_sdl_get_fullscreen_state(void) {
    return fullscreen_state;
}

static void gfx_sdl_set_fullscreen_changed_callback(void (*cb)(bool is_now_fullscreen)) {
    on_fullscreen_changed_callback = cb;
}

static void gfx_sdl_set_fullscreen(bool enable) {
    if (enable != fullscreen_state) {
        apply_fullscreen(enable);
    }
}

static void gfx_sdl_set_fullscreen_exclusive(bool exc) {
    fullscreen_exclusive = exc;
    if (fullscreen_state) {
        apply_fullscreen(true);
    }
}

static void gfx_sdl_set_fullscreen_flag(int32_t mode) {
    gfx_sdl_set_fullscreen_exclusive(mode != 0);
}

static int32_t gfx_sdl_get_fullscreen_flag_mode(void) {
    return fullscreen_exclusive;
}

static int32_t gfx_sdl_get_maximized_state(void) {
    return (SDL_GetWindowFlags(wnd) & SDL_WINDOW_MAXIMIZED) != 0;
}

static void gfx_sdl_set_maximize(bool enable) {
    if (enable) {
        SDL_MaximizeWindow(wnd);
    } else {
        SDL_RestoreWindow(wnd);
    }
}

static void gfx_sdl_get_active_window_refresh_rate(uint32_t *refresh_rate) {
    const SDL_DisplayMode *m = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(wnd));

    *refresh_rate = (m != NULL && m->refresh_rate > 0.0f) ? (uint32_t)(m->refresh_rate + 0.5f) : 60;
}

static void gfx_sdl_set_cursor_visibility(bool visible) {
    if (visible) {
        SDL_ShowCursor();
    } else {
        SDL_HideCursor();
    }
}

static void gfx_sdl_set_closest_resolution(int32_t width, int32_t height, bool should_center) {
    SDL_SetWindowSize(wnd, width, height);
    if (should_center) {
        SDL_SetWindowPosition(wnd, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
}

static void gfx_sdl_set_dimensions(uint32_t width, uint32_t height, int32_t posX, int32_t posY) {
    SDL_SetWindowSize(wnd, width, height);
    SDL_SetWindowPosition(wnd, posX, posY);
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

static void gfx_sdl_get_centered_positions(int32_t width, int32_t height, int32_t *posX, int32_t *posY) {
    SDL_Rect r;

    if (SDL_GetDisplayUsableBounds(SDL_GetDisplayForWindow(wnd), &r)) {
        *posX = r.x + (r.w - width) / 2;
        *posY = r.y + (r.h - height) / 2;
    } else {
        *posX = *posY = 0;
    }
}

/* Events are pumped by port/src/events.c on the same thread. */
static void gfx_sdl_handle_events(void) {
}

static bool gfx_sdl_start_frame(void) {
    return true;
}

static void gfx_sdl_swap_buffers_begin(void) {
    SDL_GL_SwapWindow(wnd);
}

static void gfx_sdl_swap_buffers_end(void) {
}

static double gfx_sdl_get_time(void) {
    return SDL_GetTicksNS() / 1e9;
}

static int32_t gfx_sdl_get_target_fps(void) {
    return target_fps;
}

static void gfx_sdl_set_target_fps(int fps) {
    target_fps = fps;
}

static bool gfx_sdl_can_disable_vsync(void) {
    return true;
}

static void *gfx_sdl_get_window_handle(void) {
    return wnd;
}

static void gfx_sdl_set_window_title(const char *title) {
    SDL_SetWindowTitle(wnd, title);
}

static int gfx_sdl_get_swap_interval(void) {
    return swap_interval;
}

static bool gfx_sdl_set_swap_interval(int interval) {
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
    gfx_sdl_close,
    gfx_sdl_get_display_mode,
    gfx_sdl_get_current_display_mode,
    gfx_sdl_get_num_display_modes,
    gfx_sdl_get_fullscreen_state,
    gfx_sdl_set_fullscreen_changed_callback,
    gfx_sdl_set_fullscreen,
    gfx_sdl_set_fullscreen_exclusive,
    gfx_sdl_set_fullscreen_flag,
    gfx_sdl_get_fullscreen_flag_mode,
    gfx_sdl_get_maximized_state,
    gfx_sdl_set_maximize,
    gfx_sdl_get_active_window_refresh_rate,
    gfx_sdl_set_cursor_visibility,
    gfx_sdl_set_closest_resolution,
    gfx_sdl_set_dimensions,
    gfx_sdl_get_dimensions,
    gfx_sdl_get_centered_positions,
    gfx_sdl_handle_events,
    gfx_sdl_start_frame,
    gfx_sdl_swap_buffers_begin,
    gfx_sdl_swap_buffers_end,
    gfx_sdl_get_time,
    gfx_sdl_get_target_fps,
    gfx_sdl_set_target_fps,
    gfx_sdl_can_disable_vsync,
    gfx_sdl_get_window_handle,
    gfx_sdl_set_window_title,
    gfx_sdl_get_swap_interval,
    gfx_sdl_set_swap_interval,
};
