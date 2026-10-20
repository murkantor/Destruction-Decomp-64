#ifndef GFX_RENDERING_API_H
#define GFX_RENDERING_API_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct ShaderProgram;

enum FilteringMode { FILTER_NONE, FILTER_LINEAR, FILTER_THREE_POINT };

/* What the display-list interpreter (gfx_pc.cpp) asks of a renderer: gfx_opengl.cpp */
struct GfxRenderingAPI {
    void (*unload_shader)(struct ShaderProgram* old_prg);
    void (*load_shader)(struct ShaderProgram* new_prg);
    struct ShaderProgram* (*create_and_load_new_shader)(uint64_t shader_id0, uint32_t shader_id1);
    struct ShaderProgram* (*lookup_shader)(uint64_t shader_id0, uint32_t shader_id1);
    uint32_t (*new_texture)(void);
    void (*select_texture)(int tile, uint32_t texture_id, bool linear_filter);
    void (*upload_texture)(const uint8_t* rgba32_buf, uint32_t width, uint32_t height);
    void (*set_sampler_parameters)(int sampler, bool linear_filter, uint32_t cms, uint32_t cmt);
    void (*set_depth_mode)(bool depth_test, bool depth_update, bool depth_compare, bool depth_source_prim, uint16_t zmode);
    void (*set_viewport)(int x, int y, int width, int height);
    void (*set_scissor)(int x, int y, int width, int height);
    void (*set_use_alpha)(bool use_alpha, bool modulate);
    void (*draw_triangles)(float buf_vbo[], size_t buf_vbo_len, size_t buf_vbo_num_tris);
    void (*init)(void);
    void (*start_frame)(void);
    void (*end_frame)(void);
    int (*create_framebuffer)(void);
    /* framebuffer 0 is the window; the others are render targets */
    void (*update_framebuffer_parameters)(int fb_id, uint32_t width, uint32_t height, uint32_t msaa_level,
                                          bool has_depth_buffer);
    bool (*start_draw_to_framebuffer)(int fb_id, float noise_scale);
    void (*clear_framebuffer)(bool clear_color, bool clear_depth);
    void (*resolve_msaa_color_buffer)(int fb_id_target, int fb_id_source);
    void (*delete_texture)(uint32_t texID);
    void (*set_texture_filter)(enum FilteringMode mode);
    /* Resolve the multisampled framebuffer fb_src into fb_dst the way the N64's
     * Video Interface filtered its frame (see gfx_opengl.cpp) */
    void (*resolve_n64_aa)(int fb_dst, int fb_src);
    /* A whole frame's vertices in one upload (the interpolated replay), then draws
       from it: first_vertex counts in the current program's stride */
    void (*upload_vertices)(const float *data, size_t num_floats);
    void (*draw_uploaded)(size_t first_vertex, size_t num_tris);
};

#endif
