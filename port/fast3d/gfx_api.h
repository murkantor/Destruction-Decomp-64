#ifndef GFX_API_H
#define GFX_API_H

#ifndef __cplusplus
#include <stdint.h>
#include <stdbool.h>
#endif

#include "gfx_rendering_api.h"
#include "gfx_window_manager_api.h"

struct XYWidthHeight {
    int16_t x, y;
    uint32_t width, height;
};

struct GfxInitSettings {
    struct GfxWindowManagerAPI *wapi;
    struct GfxRenderingAPI *rapi;
    struct GfxWindowInitSettings window_settings;
};

/* Settings, read by gfx_pc.cpp and gfx_opengl.cpp */
extern uint32_t gfx_msaa_level;  /* 1 = off */
extern uint32_t gfx_n64_aa;      /* resolve the MSAA frame like the N64's VI (gfx_opengl resolve_n64_aa) */
extern bool gfx_framebuffers_enabled;
extern uint32_t gfx_anisotropy;  /* 1 = off; 2..16 samples */
extern uint32_t gfx_internal_height; /* lines the game is drawn at; 0 = the window's */
extern uint32_t gfx_edge_fill;   /* fill see-through texels' colour from their neighbours */
extern uint32_t gfx_seamless_sky; /* the sky dome's pieces filtered across their joins */
extern uint32_t gfx_draw_distance; /* scenery and fog this many times further out; 1 = the N64's */
extern uint32_t gfx_aspect_mode; /* 0 = 4:3 with side bars, 1 = stretch to the window, 2 = widescreen */
#ifdef __cplusplus
extern "C" {
#endif
/* Widescreen: the 3D scene keeps to the 4:3 area, as on the N64, while a menu
   that hides it there is up (port/game/pcmenu.c: the options screen over a race) */
extern uint32_t gfx_scene_4x3;
extern uint32_t gfx_scene_hidden; /* the 3D is not drawn (a menu covers it), the colour cleared */
#ifdef __cplusplus
}
#endif

void gfx_init(const struct GfxInitSettings *settings);

/* One RSP graphics task: a display list started under the given microcode
 * (the N64 address of its text, so F3DEX2 or S2DEX2 is known). */
void gfx_start_frame(void);
void gfx_run(const void *commands, uint32_t ucode_text);
void gfx_end_frame(void);
/* Show the last frame drawn (the VI swapped to its framebuffer), or black */
void gfx_present(bool black);

void gfx_set_vsync(int interval);
void gfx_set_texture_filter(enum FilteringMode mode);
void gfx_texture_cache_clear(void);

#endif
