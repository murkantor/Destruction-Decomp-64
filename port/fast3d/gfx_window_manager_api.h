#ifndef GFX_WINDOW_MANAGER_API_H
#define GFX_WINDOW_MANAGER_API_H

#include <stdint.h>
#include <stdbool.h>

struct GfxWindowInitSettings {
    const char *title;
    uint32_t width;
    uint32_t height;
    bool fullscreen;
    bool fullscreen_is_exclusive;
    bool allow_hidpi;
};

/* The window and GL context: gfx_sdl3.cpp */
struct GfxWindowManagerAPI {
    void (*init)(const struct GfxWindowInitSettings *settings);
    int32_t (*get_fullscreen_state)(void);
    void (*set_fullscreen)(bool enable);
    /* the window's drawable size in pixels (and position, either may be NULL) */
    void (*get_dimensions)(uint32_t* width, uint32_t* height, int32_t* posX, int32_t* posY);
    void (*swap_buffers_begin)(void);
    void (*swap_buffers_end)(void);
    bool (*set_swap_interval)(int);
};

#endif
