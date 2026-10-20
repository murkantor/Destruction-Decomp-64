/*
 * Video: graphics tasks go to Fast3D (port/fast3d), and the VI swap presents
 * the frame. Everything here runs on the host main thread, which owns the
 * window and the GL context.
 */
#include <SDL3/SDL.h>

#include "port.h"
#include "port_config.h"
#include "gfx_api.h"
#include "gfx_opengl.h"

extern struct GfxWindowManagerAPI gfx_sdl;

static int sBlack;
static int sFramesDrawn;

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
    gfx_aspect_mode = gPortConfig.stretch;
    gfx_init(&set);
    gfx_set_vsync(gPortConfig.vsync);
    gfx_set_texture_filter(gPortConfig.textureFilter);
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
    sFramesDrawn++;
}

void Port_VideoSwap(void *framebuffer) {
    gfx_present(sBlack);
}

void Port_VideoSetBlack(int black) {
    sBlack = black;
}

void Port_VideoPresentIdle(void) {
    gfx_present(true);
}
