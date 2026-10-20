/*
 * SDL_gpu backend for the display-list interpreter (gfx_pc.cpp): the second
 * renderer next to OpenGL (gfx_opengl.cpp), Vulkan through SDL3's GPU API on
 * Linux and Windows ([video] renderer = sdlgpu). It draws what the OpenGL
 * backend draws, and should match it pixel for pixel where the drivers do:
 *
 * - One shader for every combiner (shaders/combiner.*.glsl, which the OpenGL
 *   backend runs too with DD64_UBERSHADER=1), compiled to SPIR-V ahead of
 *   time (shaders/spirv.h); a draw's combiner is in its uniforms.
 * - Every image is kept bottom row first, as GL keeps it, so the
 *   interpreter's viewports and scissors, gl_FragCoord (the noise) and the
 *   rasteriser's rules mean what they mean in GL. The window is a texture too
 *   (framebuffer 0), flipped onto the swapchain when the frame is shown.
 * - SDL_gpu cannot upload or clear inside a render pass, so a pass's draws
 *   are recorded and run when the pass ends (another framebuffer, a clear, a
 *   blit, a screenshot, the end of the frame): first one copy pass with every
 *   vertex and texture they use, then the render pass.
 * - Pipelines are made as draws need them, by vertex layout, depth mode,
 *   blending and the target's sample count.
 *
 * What it cannot do as the OpenGL backend does: SDL_gpu has no
 * mirror-clamp-to-edge, so mirrored clamping repeats the mirror (as GL does
 * without ARB_texture_mirror_clamp_to_edge).
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <map>
#include <vector>

#include <SDL3/SDL.h>

#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif
#include <PR/gbi.h>

#include "gfx_cc.h"
#include "gfx_rendering_api.h"
#include "gfx_pc.h"
#include "shaders/spirv.h"

extern "C" SDL_Window* gfx_sdl_window(void);
extern int g_GfxPresentX, g_GfxPresentW; /* gfx_opengl.cpp: where a present into the window lands */

#define COLOR_FORMAT SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM
#define DEPTH_FAR 0.9999995f /* gl_set_depth_range: clamped far geometry must pass GL_LESS */

/* The types here are this file's own: gfx_opengl.cpp has its own Framebuffer,
   and the interpreter's struct ShaderProgram is gfx_opengl.cpp's; here a
   ShaderProgram pointer is a GpuProgram */
namespace {

/* The combiner shader's uniforms (std140, CombinerUniforms in combiner.frag.glsl) */
struct CombinerUniforms {
    int32_t cc[4][4];
    int32_t form[4];
    int32_t opt[4];
    float noise[4];
};

struct GpuProgram {
    uint8_t num_floats;
    uint8_t layout;       /* the vertex layout: bits 0-1 textures, 2 fog, 3-5 inputs, 6 inputs have alpha */
    int32_t cc[4][4];     /* uCC */
    int32_t form[4];      /* uForm */
    int32_t opt[2];       /* uOpt.x and .y, without the three-point bits */
};

struct Texture {
    SDL_GPUTexture* tex;
    uint32_t width, height;
    bool mips;
    bool used;            /* a live texture id */
    SDL_GPUSampler* sampler;
};

struct Framebuffer {
    uint32_t width, height, msaa;
    bool has_depth;
    SDL_GPUTexture* color;    /* the render target (multisampled with MSAA) */
    SDL_GPUTexture* resolved; /* with MSAA: the single-sample copy blits read */
    SDL_GPUTexture* depth;
    bool msaa_sampled;        /* color can be sampled per sample (N64 AA) */
};

/* A draw recorded for the pass, with all the state it needs */
struct Draw {
    uint32_t pipeline;  /* pipeline key */
    uint32_t uniforms;  /* index into pass_uniforms */
    SDL_GPUTexture* tex[2];
    SDL_GPUSampler* smp[2];
    int32_t vp[4], sc[4];
    uint32_t vb_offset; /* bytes */
    uint32_t num_verts;
    bool uploaded;      /* from the replay's buffer (upload_vertices), not the pass's stream */
};

/* An upload waiting for the next copy pass: bytes of upload_bytes into a texture */
struct TexUpload {
    SDL_GPUTexture* tex;
    uint32_t offset, width, height;
    bool mips;
};

} // namespace

static SDL_GPUDevice* dev;
static SDL_Window* wnd;
static SDL_GPUCommandBuffer* cmd; /* the frame's, acquired when first needed */
static SDL_GPUTextureFormat depth_format;
static SDL_GPUShader *combiner_vs, *combiner_fs;
static std::map<uint32_t, SDL_GPUGraphicsPipeline*> pipelines;
static std::map<uint32_t, SDL_GPUSampler*> samplers;
static std::map<std::pair<uint64_t, uint32_t>, GpuProgram> shader_program_pool;
static std::vector<Texture> textures;     /* by id; 0 is never handed out */
static std::vector<uint32_t> free_texture_ids;
static std::vector<Framebuffer> framebuffers;
static SDL_GPUTexture* dummy_texture;     /* bound where a draw reads no texture */
static SDL_GPUSampler* dummy_sampler;

static uint32_t frame_count;
static float current_noise_scale = 1.0f;
static FilteringMode current_filter_mode = FILTER_LINEAR;
static int current_framebuffer;

/* The state the next draw takes */
static GpuProgram* cur_prg;
static uint32_t cur_texture_ids[2];
static bool cur_linear[2];
static int last_selected_tile;
static bool depth_test, depth_mask;
static int depth_func;          /* 0 LEQUAL, 1 LESS, 2 ALWAYS */
static bool depth_decal;        /* ZMODE_DEC's polygon offset */
static bool use_alpha, blend_modulate;
static int32_t cur_vp[4], cur_sc[4];

/* The pass being recorded on current_framebuffer */
static std::vector<Draw> pass_draws;
static std::vector<uint8_t> pass_vertices;
static std::vector<CombinerUniforms> pass_uniforms;
static bool pending_clear_color, pending_clear_depth;

/* The interpolated replay's vertices (upload_vertices): kept apart from the
   pass's stream, as the replay's draws may span passes */
static std::vector<uint8_t> uploaded_vertices;
static bool uploaded_dirty;
static SDL_GPUBuffer *vb_stream, *vb_uploaded;
static uint32_t vb_stream_size, vb_uploaded_size;

/* Uploads for the next copy pass */
static std::vector<uint8_t> upload_bytes;
static std::vector<TexUpload> tex_uploads;
static SDL_GPUTransferBuffer* upload_tb;
static uint32_t upload_tb_size;
/* Textures replaced or deleted while a recorded draw may still use them */
static std::vector<SDL_GPUTexture*> release_later;

static SDL_GPUCommandBuffer* get_cmd(void) {
    if (cmd == NULL) {
        cmd = SDL_AcquireGPUCommandBuffer(dev);
        if (cmd == NULL) {
            sysFatalError("SDL_gpu: no command buffer: %s", SDL_GetError());
        }
    }
    return cmd;
}

static SDL_GPUTexture* make_texture(uint32_t w, uint32_t h, uint32_t levels, SDL_GPUTextureFormat format,
                                    SDL_GPUTextureUsageFlags usage, uint32_t msaa) {
    SDL_GPUTextureCreateInfo ci;
    SDL_zero(ci);
    ci.type = SDL_GPU_TEXTURETYPE_2D;
    ci.format = format;
    ci.usage = usage;
    ci.width = w;
    ci.height = h;
    ci.layer_count_or_depth = 1;
    ci.num_levels = levels;
    ci.sample_count = msaa >= 8 ? SDL_GPU_SAMPLECOUNT_8 : msaa >= 4 ? SDL_GPU_SAMPLECOUNT_4
                    : msaa >= 2 ? SDL_GPU_SAMPLECOUNT_2 : SDL_GPU_SAMPLECOUNT_1;
    return SDL_CreateGPUTexture(dev, &ci);
}

static SDL_GPUBuffer* grow_buffer(SDL_GPUBuffer* buf, uint32_t* size, uint32_t need) {
    if (buf != NULL && *size >= need) {
        return buf;
    }
    if (buf != NULL) {
        SDL_ReleaseGPUBuffer(dev, buf);
    }
    uint32_t n = 1u << 20;
    while (n < need) {
        n *= 2;
    }
    SDL_GPUBufferCreateInfo ci;
    SDL_zero(ci);
    ci.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    ci.size = n;
    *size = n;
    buf = SDL_CreateGPUBuffer(dev, &ci);
    if (buf == NULL) {
        sysFatalError("SDL_gpu: no %u-byte vertex buffer: %s", n, SDL_GetError());
    }
    return buf;
}

/* Bytes for the next copy pass; returns where they start in upload_bytes */
static uint32_t stage(const void* data, size_t len) {
    uint32_t off = (uint32_t)((upload_bytes.size() + 15) & ~(size_t)15);
    upload_bytes.resize(off + len);
    memcpy(upload_bytes.data() + off, data, len);
    return off;
}

/* The copy pass: textures, the pass's vertices and the replay's, then mip chains */
static void run_uploads(void) {
    uint32_t stream_off = 0, uploaded_off = 0;
    bool stream = !pass_vertices.empty(), uploaded = uploaded_dirty && !uploaded_vertices.empty();

    if (stream) {
        stream_off = stage(pass_vertices.data(), pass_vertices.size());
        vb_stream = grow_buffer(vb_stream, &vb_stream_size, (uint32_t)pass_vertices.size());
    }
    if (uploaded) {
        uploaded_off = stage(uploaded_vertices.data(), uploaded_vertices.size());
        vb_uploaded = grow_buffer(vb_uploaded, &vb_uploaded_size, (uint32_t)uploaded_vertices.size());
        uploaded_dirty = false;
    }
    if (upload_bytes.empty()) {
        return;
    }
    if (upload_tb == NULL || upload_tb_size < upload_bytes.size()) {
        if (upload_tb != NULL) {
            SDL_ReleaseGPUTransferBuffer(dev, upload_tb);
        }
        upload_tb_size = 1u << 20;
        while (upload_tb_size < upload_bytes.size()) {
            upload_tb_size *= 2;
        }
        SDL_GPUTransferBufferCreateInfo ci;
        SDL_zero(ci);
        ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        ci.size = upload_tb_size;
        upload_tb = SDL_CreateGPUTransferBuffer(dev, &ci);
        if (upload_tb == NULL) {
            sysFatalError("SDL_gpu: no %u-byte transfer buffer: %s", upload_tb_size, SDL_GetError());
        }
    }
    /* cycled: an earlier pass of this frame may still read the last contents */
    void* map = SDL_MapGPUTransferBuffer(dev, upload_tb, true);
    memcpy(map, upload_bytes.data(), upload_bytes.size());
    SDL_UnmapGPUTransferBuffer(dev, upload_tb);

    SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(get_cmd());
    for (const TexUpload& u : tex_uploads) {
        SDL_GPUTextureTransferInfo src;
        SDL_GPUTextureRegion dst;
        SDL_zero(src);
        SDL_zero(dst);
        src.transfer_buffer = upload_tb;
        src.offset = u.offset;
        src.pixels_per_row = u.width;
        src.rows_per_layer = u.height;
        dst.texture = u.tex;
        dst.w = u.width;
        dst.h = u.height;
        dst.d = 1;
        SDL_UploadToGPUTexture(copy, &src, &dst, false);
    }
    SDL_GPUTransferBufferLocation loc;
    SDL_GPUBufferRegion reg;
    if (stream) {
        loc.transfer_buffer = upload_tb;
        loc.offset = stream_off;
        reg.buffer = vb_stream;
        reg.offset = 0;
        reg.size = (uint32_t)pass_vertices.size();
        SDL_UploadToGPUBuffer(copy, &loc, &reg, true);
    }
    if (uploaded) {
        loc.transfer_buffer = upload_tb;
        loc.offset = uploaded_off;
        reg.buffer = vb_uploaded;
        reg.offset = 0;
        reg.size = (uint32_t)uploaded_vertices.size();
        SDL_UploadToGPUBuffer(copy, &loc, &reg, true);
    }
    SDL_EndGPUCopyPass(copy);
    for (const TexUpload& u : tex_uploads) {
        if (u.mips) {
            SDL_GenerateMipmapsForGPUTexture(cmd, u.tex);
        }
    }
    tex_uploads.clear();
    upload_bytes.clear();
}

/* ---- Pipelines ---- */

#define PK_LAYOUT(k) ((k) & 0x7F)
#define PK_DEPTH_TEST (1u << 7)
#define PK_DEPTH_MASK (1u << 8)
#define PK_DEPTH_FUNC(k) (((k) >> 9) & 3)
#define PK_DECAL (1u << 11)
#define PK_BLEND(k) (((k) >> 12) & 3) /* 0 none, 1 alpha, 2 modulate */
#define PK_MSAA(k) (((k) >> 14) & 3)  /* log2 of the target's samples */

/* ZMODE_DEC: gfx_opengl.cpp's polygon offset, DD64_DEC_ZBIAS=factor,units as there */
static float dec_bias_f = -4.0f, dec_bias_u = -8.0f;

static uint32_t msaa_log2(uint32_t msaa) {
    return msaa >= 8 ? 3 : msaa >= 4 ? 2 : msaa >= 2 ? 1 : 0;
}

/* The vertex layout a combiner's vertices have (gfx_pc.cpp emit_vertex) */
static uint32_t layout_stride(uint32_t layout) {
    uint32_t n = 4 + ((layout & 1) ? 2 : 0) + ((layout & 2) ? 2 : 0) + ((layout & 4) ? 4 : 0);
    return (n + ((layout >> 3) & 7) * ((layout & 64) ? 4 : 3)) * sizeof(float);
}

static SDL_GPUGraphicsPipeline* get_pipeline(uint32_t key) {
    auto it = pipelines.find(key);
    if (it != pipelines.end()) {
        return it->second;
    }

    const uint32_t layout = PK_LAYOUT(key);
    SDL_GPUVertexBufferDescription vbd;
    SDL_GPUVertexAttribute attrs[11];
    SDL_zero(vbd);
    vbd.slot = 0;
    vbd.pitch = layout_stride(layout);
    vbd.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    /* Every location the shader declares has an attribute; the ones the layout
       lacks read the position (the shader never uses them) */
    uint32_t off = 16, num_inputs = (layout >> 3) & 7;
    for (uint32_t i = 0; i < 11; i++) {
        attrs[i].location = i;
        attrs[i].buffer_slot = 0;
        attrs[i].offset = 0;
        attrs[i].format = (i == 1 || i == 2) ? SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2 : SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    }
    for (uint32_t t = 0; t < 2; t++) {
        if (layout & (1 << t)) {
            attrs[1 + t].offset = off;
            off += 8;
        }
    }
    if (layout & 4) {
        attrs[3].offset = off;
        off += 16;
    }
    for (uint32_t i = 0; i < num_inputs; i++) {
        attrs[4 + i].offset = off;
        attrs[4 + i].format = (layout & 64) ? SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4 : SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
        off += (layout & 64) ? 16 : 12;
    }

    SDL_GPUColorTargetDescription ctd;
    SDL_zero(ctd);
    ctd.format = COLOR_FORMAT;
    if (PK_BLEND(key) != 0) {
        SDL_GPUBlendFactor src = PK_BLEND(key) == 2 ? SDL_GPU_BLENDFACTOR_DST_COLOR : SDL_GPU_BLENDFACTOR_SRC_ALPHA;
        SDL_GPUBlendFactor dst = PK_BLEND(key) == 2 ? SDL_GPU_BLENDFACTOR_ZERO : SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        ctd.blend_state.enable_blend = true;
        ctd.blend_state.src_color_blendfactor = ctd.blend_state.src_alpha_blendfactor = src;
        ctd.blend_state.dst_color_blendfactor = ctd.blend_state.dst_alpha_blendfactor = dst;
        ctd.blend_state.color_blend_op = ctd.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    }

    SDL_GPUGraphicsPipelineCreateInfo ci;
    SDL_zero(ci);
    ci.vertex_shader = combiner_vs;
    ci.fragment_shader = combiner_fs;
    ci.vertex_input_state.vertex_buffer_descriptions = &vbd;
    ci.vertex_input_state.num_vertex_buffers = 1;
    ci.vertex_input_state.vertex_attributes = attrs;
    ci.vertex_input_state.num_vertex_attributes = 11;
    ci.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    ci.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    ci.rasterizer_state.enable_depth_clip = false; /* clamped, as GL_DEPTH_CLAMP: no far clip, as the RSP */
    if (key & PK_DECAL) {
        ci.rasterizer_state.enable_depth_bias = true;
        ci.rasterizer_state.depth_bias_slope_factor = dec_bias_f;
        ci.rasterizer_state.depth_bias_constant_factor = dec_bias_u;
    }
    ci.multisample_state.sample_count = (SDL_GPUSampleCount)PK_MSAA(key);
    if (key & PK_DEPTH_TEST) {
        static const SDL_GPUCompareOp ops[3] = { SDL_GPU_COMPAREOP_LESS_OR_EQUAL, SDL_GPU_COMPAREOP_LESS,
                                                 SDL_GPU_COMPAREOP_ALWAYS };
        ci.depth_stencil_state.enable_depth_test = true;
        ci.depth_stencil_state.enable_depth_write = (key & PK_DEPTH_MASK) != 0;
        ci.depth_stencil_state.compare_op = ops[PK_DEPTH_FUNC(key)];
    }
    ci.target_info.color_target_descriptions = &ctd;
    ci.target_info.num_color_targets = 1;
    ci.target_info.depth_stencil_format = depth_format;
    ci.target_info.has_depth_stencil_target = true;

    SDL_GPUGraphicsPipeline* p = SDL_CreateGPUGraphicsPipeline(dev, &ci);
    if (p == NULL) {
        sysFatalError("SDL_gpu: pipeline %x failed: %s", key, SDL_GetError());
    }
    pipelines[key] = p;
    return p;
}

/* ---- Render passes ---- */

/* Run the recorded pass: the uploads it needs, then its clears and draws */
static void flush_pass(void) {
    if (pass_draws.empty() && !pending_clear_color && !pending_clear_depth) {
        if (!tex_uploads.empty() || uploaded_dirty) {
            run_uploads();
        }
        return;
    }
    run_uploads();

    Framebuffer& fb = framebuffers[current_framebuffer];
    SDL_GPUColorTargetInfo ct;
    SDL_GPUDepthStencilTargetInfo dt;
    SDL_zero(ct);
    SDL_zero(dt);
    ct.texture = fb.color;
    ct.load_op = pending_clear_color ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
    ct.store_op = SDL_GPU_STOREOP_STORE;
    ct.clear_color.a = 1.0f;
    dt.texture = fb.depth;
    dt.clear_depth = 1.0f;
    dt.load_op = pending_clear_depth ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
    dt.store_op = SDL_GPU_STOREOP_STORE;
    dt.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    dt.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
    pending_clear_color = pending_clear_depth = false;

    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(get_cmd(), &ct, 1, &dt);
    uint32_t bound_pipeline = UINT32_MAX, pushed_uniforms = UINT32_MAX;
    int32_t vp[4] = { -1, -1, -1, -1 }, sc[4] = { -1, -1, -1, -1 };
    SDL_GPUTexture* tex[2] = { NULL, NULL };
    SDL_GPUSampler* smp[2] = { NULL, NULL };
    SDL_GPUBuffer* vb = NULL;
    uint32_t vb_off = UINT32_MAX;

    for (const Draw& d : pass_draws) {
        if (d.pipeline != bound_pipeline) {
            SDL_BindGPUGraphicsPipeline(pass, get_pipeline(d.pipeline));
            bound_pipeline = d.pipeline;
        }
        if (memcmp(vp, d.vp, sizeof(vp)) != 0) {
            SDL_GPUViewport v = { (float)d.vp[0], (float)d.vp[1], (float)d.vp[2], (float)d.vp[3], 0.0f, DEPTH_FAR };
            SDL_SetGPUViewport(pass, &v);
            memcpy(vp, d.vp, sizeof(vp));
        }
        if (memcmp(sc, d.sc, sizeof(sc)) != 0) {
            SDL_Rect r = { d.sc[0], d.sc[1], d.sc[2], d.sc[3] };
            SDL_SetGPUScissor(pass, &r);
            memcpy(sc, d.sc, sizeof(sc));
        }
        if (d.uniforms != pushed_uniforms) {
            SDL_PushGPUFragmentUniformData(cmd, 0, &pass_uniforms[d.uniforms], sizeof(CombinerUniforms));
            pushed_uniforms = d.uniforms;
        }
        if (d.tex[0] != tex[0] || d.tex[1] != tex[1] || d.smp[0] != smp[0] || d.smp[1] != smp[1]) {
            SDL_GPUTextureSamplerBinding b[2] = { { d.tex[0], d.smp[0] }, { d.tex[1], d.smp[1] } };
            SDL_BindGPUFragmentSamplers(pass, 0, b, 2);
            memcpy(tex, d.tex, sizeof(tex));
            memcpy(smp, d.smp, sizeof(smp));
        }
        SDL_GPUBuffer* want = d.uploaded ? vb_uploaded : vb_stream;
        if (want != vb || d.vb_offset != vb_off) {
            SDL_GPUBufferBinding b = { want, d.vb_offset };
            SDL_BindGPUVertexBuffers(pass, 0, &b, 1);
            vb = want;
            vb_off = d.vb_offset;
        }
        SDL_DrawGPUPrimitives(pass, d.num_verts, 1, 0, 0);
    }
    SDL_EndGPURenderPass(pass);

    pass_draws.clear();
    pass_vertices.clear();
    pass_uniforms.clear();
    for (SDL_GPUTexture* t : release_later) {
        SDL_ReleaseGPUTexture(dev, t);
    }
    release_later.clear();
}

/* ---- Shaders ---- */

static void gfx_sdlgpu_unload_shader(struct ShaderProgram* old_prg) {
}

static void gfx_sdlgpu_load_shader(struct ShaderProgram* new_prg) {
    cur_prg = (GpuProgram*)new_prg;
}

static struct ShaderProgram* gfx_sdlgpu_create_and_load_new_shader(uint64_t shader_id0, uint32_t shader_id1) {
    struct CCFeatures cc_features = { 0 };
    gfx_cc_get_features(shader_id0, shader_id1, &cc_features);
    GpuProgram* prg = &shader_program_pool[std::make_pair(shader_id0, shader_id1)];

    memset(prg, 0, sizeof(*prg));
    prg->layout = (cc_features.used_textures[0] ? 1 : 0) | (cc_features.used_textures[1] ? 2 : 0) |
                  (cc_features.opt_fog ? 4 : 0) | (cc_features.num_inputs << 3) | (cc_features.opt_alpha ? 64 : 0);
    prg->num_floats = (uint8_t)(layout_stride(prg->layout) / sizeof(float));
    for (int c = 0; c < 2; c++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 4; k++) {
                prg->cc[c * 2 + j][k] = cc_features.c[c][j][k];
            }
            /* the generator's choice, in its order (gfx_opengl.cpp append_formula) */
            prg->form[c * 2 + j] = cc_features.do_single[c][j] ? 1 : cc_features.do_multiply[c][j] ? 2
                                 : cc_features.do_mix[c][j]    ? 3 : 0;
        }
    }
    prg->opt[0] = (int32_t)shader_id1;
    prg->opt[1] = (cc_features.used_textures[0] ? 1 : 0) | (cc_features.used_textures[1] ? 2 : 0) |
                  (current_filter_mode == FILTER_THREE_POINT ? 4 : 0);
    cur_prg = prg;
    return (struct ShaderProgram*)prg;
}

static struct ShaderProgram* gfx_sdlgpu_lookup_shader(uint64_t shader_id0, uint32_t shader_id1) {
    auto it = shader_program_pool.find(std::make_pair(shader_id0, shader_id1));
    return it == shader_program_pool.end() ? nullptr : (struct ShaderProgram*)&it->second;
}

/* ---- Textures ---- */

static uint32_t gfx_sdlgpu_new_texture(void) {
    uint32_t id;
    if (!free_texture_ids.empty()) {
        id = free_texture_ids.back();
        free_texture_ids.pop_back();
    } else {
        if (textures.empty()) {
            textures.resize(1); /* id 0: none */
        }
        id = (uint32_t)textures.size();
        textures.resize(id + 1);
    }
    textures[id] = Texture();
    textures[id].used = true;
    return id;
}

static void gfx_sdlgpu_delete_texture(uint32_t id) {
    if (id < textures.size() && textures[id].used) {
        if (textures[id].tex != NULL) {
            release_later.push_back(textures[id].tex);
        }
        textures[id] = Texture();
        free_texture_ids.push_back(id);
    }
}

static void gfx_sdlgpu_select_texture(int tile, uint32_t texture_id, bool linear_filter) {
    cur_texture_ids[tile] = texture_id;
    cur_linear[tile] = linear_filter;
    last_selected_tile = tile;
}

static void gfx_sdlgpu_upload_texture(const uint8_t* rgba32_buf, uint32_t width, uint32_t height) {
    uint32_t id = cur_texture_ids[last_selected_tile];
    if (id == 0 || id >= textures.size()) {
        return;
    }
    Texture& t = textures[id];
    /* a fresh texture each upload: draws recorded before this one keep the old */
    if (t.tex != NULL) {
        release_later.push_back(t.tex);
    }
    t.mips = gfx_anisotropy > 1;
    uint32_t levels = 1;
    if (t.mips) {
        for (uint32_t s = width > height ? width : height; s > 1; s >>= 1) {
            levels++;
        }
    }
    t.tex = make_texture(width, height, levels, COLOR_FORMAT,
                         SDL_GPU_TEXTUREUSAGE_SAMPLER | (t.mips ? SDL_GPU_TEXTUREUSAGE_COLOR_TARGET : 0), 1);
    if (t.tex == NULL) {
        sysFatalError("SDL_gpu: no %ux%u texture: %s", width, height, SDL_GetError());
    }
    t.width = width;
    t.height = height;
    TexUpload u = { t.tex, stage(rgba32_buf, (size_t)width * height * 4), width, height, t.mips && levels > 1 };
    tex_uploads.push_back(u);
}

static SDL_GPUSamplerAddressMode address_mode(uint32_t cm) {
    switch (cm) {
        case G_TX_NOMIRROR | G_TX_CLAMP:
            return SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        case G_TX_MIRROR | G_TX_WRAP:
        case G_TX_MIRROR | G_TX_CLAMP: /* no mirror-clamp in SDL_gpu */
            return SDL_GPU_SAMPLERADDRESSMODE_MIRRORED_REPEAT;
    }
    return SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
}

static SDL_GPUSampler* get_sampler(bool linear, bool mips, uint32_t aniso, uint32_t cms, uint32_t cmt) {
    uint32_t key = (linear ? 1 : 0) | (mips ? 2 : 0) | (aniso << 2) | (cms << 8) | (cmt << 12);
    auto it = samplers.find(key);
    if (it != samplers.end()) {
        return it->second;
    }
    SDL_GPUSamplerCreateInfo ci;
    SDL_zero(ci);
    ci.min_filter = ci.mag_filter = linear ? SDL_GPU_FILTER_LINEAR : SDL_GPU_FILTER_NEAREST;
    ci.mipmap_mode = mips ? SDL_GPU_SAMPLERMIPMAPMODE_LINEAR : SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    ci.address_mode_u = address_mode(cms);
    ci.address_mode_v = address_mode(cmt);
    ci.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    ci.max_lod = mips ? 1000.0f : 0.0f;
    if (aniso > 1) {
        ci.enable_anisotropy = true;
        ci.max_anisotropy = (float)aniso;
    }
    SDL_GPUSampler* s = SDL_CreateGPUSampler(dev, &ci);
    if (s == NULL) {
        sysFatalError("SDL_gpu: sampler failed: %s", SDL_GetError());
    }
    samplers[key] = s;
    return s;
}

/* The filter and wrap of the texture on this tile: they belong to the texture,
   as in GL, until set again */
static void gfx_sdlgpu_set_sampler_parameters(int tile, bool linear_filter, uint32_t cms, uint32_t cmt) {
    if (tile < 0 || tile >= 2) {
        return;
    }
    cur_linear[tile] = linear_filter;
    uint32_t id = cur_texture_ids[tile];
    if (id == 0 || id >= textures.size()) {
        return;
    }
    Texture& t = textures[id];
    const bool linear = linear_filter && current_filter_mode == FILTER_LINEAR;
    const bool mips = linear && t.mips;
    t.sampler = get_sampler(linear, mips, mips ? gfx_anisotropy : 1, cms, cmt);
}

static void gfx_sdlgpu_set_texture_filter(FilteringMode mode) {
    current_filter_mode = mode;
}

/* ---- Depth, blend, viewport ---- */

static void gfx_sdlgpu_set_depth_mode(bool test, bool update, bool compare, bool source_prim, uint16_t zmode) {
    depth_test = test;
    depth_mask = update;
    depth_decal = false;
    if (!compare) {
        depth_func = 2;
    } else if (zmode == ZMODE_OPA || zmode == ZMODE_XLU) {
        depth_func = source_prim ? 0 : 1;
    } else {
        depth_func = 0; /* ZMODE_INTER, ZMODE_DEC */
        depth_decal = zmode == ZMODE_DEC;
    }
}

static void gfx_sdlgpu_set_viewport(int x, int y, int width, int height) {
    cur_vp[0] = x, cur_vp[1] = y, cur_vp[2] = width, cur_vp[3] = height;
}

static void gfx_sdlgpu_set_scissor(int x, int y, int width, int height) {
    cur_sc[0] = x, cur_sc[1] = y, cur_sc[2] = width, cur_sc[3] = height;
}

static void gfx_sdlgpu_set_use_alpha(bool alpha, bool modulate) {
    use_alpha = alpha;
    blend_modulate = modulate;
}

/* ---- Draws ---- */

/* Record a draw of the current state; false if it would draw nothing */
static bool record_draw(Draw* d) {
    const Framebuffer& fb = framebuffers[current_framebuffer];
    GpuProgram* prg = cur_prg;

    if (prg == NULL || cur_vp[2] <= 0 || cur_vp[3] <= 0) {
        return false;
    }
    /* Vulkan's scissor may not start off the target (GL's may) */
    int32_t x0 = SDL_max(cur_sc[0], 0), y0 = SDL_max(cur_sc[1], 0);
    int32_t x1 = SDL_min(cur_sc[0] + cur_sc[2], (int32_t)fb.width), y1 = SDL_min(cur_sc[1] + cur_sc[3], (int32_t)fb.height);
    if (x1 <= x0 || y1 <= y0) {
        return false;
    }
    d->sc[0] = x0, d->sc[1] = y0, d->sc[2] = x1 - x0, d->sc[3] = y1 - y0;
    memcpy(d->vp, cur_vp, sizeof(cur_vp));

    uint32_t key = prg->layout | (msaa_log2(fb.msaa) << 14);
    if (depth_test) {
        key |= PK_DEPTH_TEST | (depth_mask ? PK_DEPTH_MASK : 0) | ((uint32_t)depth_func << 9) |
               (depth_decal ? PK_DECAL : 0);
    }
    if (use_alpha) {
        key |= (blend_modulate ? 2u : 1u) << 12;
    }
    d->pipeline = key;

    CombinerUniforms u;
    memcpy(u.cc, prg->cc, sizeof(u.cc));
    memcpy(u.form, prg->form, sizeof(u.form));
    u.opt[0] = prg->opt[0];
    u.opt[1] = prg->opt[1];
    if (u.opt[1] & 4) {
        u.opt[1] |= (cur_linear[0] ? 8 : 0) | (cur_linear[1] ? 16 : 0);
    }
    u.opt[2] = (int32_t)frame_count;
    u.opt[3] = 0;
    u.noise[0] = current_noise_scale;
    u.noise[1] = u.noise[2] = u.noise[3] = 0.0f;
    if (pass_uniforms.empty() || memcmp(&pass_uniforms.back(), &u, sizeof(u)) != 0) {
        pass_uniforms.push_back(u);
    }
    d->uniforms = (uint32_t)pass_uniforms.size() - 1;

    for (int t = 0; t < 2; t++) {
        uint32_t id = cur_texture_ids[t];
        d->tex[t] = dummy_texture;
        d->smp[t] = dummy_sampler;
        if ((prg->layout & (1 << t)) && id != 0 && id < textures.size() && textures[id].tex != NULL) {
            d->tex[t] = textures[id].tex;
            if (textures[id].sampler != NULL) {
                d->smp[t] = textures[id].sampler;
            }
        }
    }
    return true;
}

static void gfx_sdlgpu_draw_triangles(float buf_vbo[], size_t buf_vbo_len, size_t buf_vbo_num_tris) {
    Draw d;
    if (!record_draw(&d)) {
        return;
    }
    d.vb_offset = (uint32_t)pass_vertices.size();
    d.num_verts = (uint32_t)(3 * buf_vbo_num_tris);
    d.uploaded = false;
    const uint8_t* p = (const uint8_t*)buf_vbo;
    pass_vertices.insert(pass_vertices.end(), p, p + buf_vbo_len * sizeof(float));
    pass_draws.push_back(d);
}

/* The interpolated replay: the frame's vertices in one buffer, then its draws */
static void gfx_sdlgpu_upload_vertices(const float* data, size_t num_floats) {
    const uint8_t* p = (const uint8_t*)data;
    uploaded_vertices.assign(p, p + num_floats * sizeof(float));
    uploaded_dirty = true;
}

static void gfx_sdlgpu_draw_uploaded(size_t first_vertex, size_t num_tris) {
    Draw d;
    if (cur_prg == NULL || !record_draw(&d)) {
        return;
    }
    d.vb_offset = (uint32_t)(first_vertex * layout_stride(cur_prg->layout));
    d.num_verts = (uint32_t)(3 * num_tris);
    d.uploaded = true;
    pass_draws.push_back(d);
}

/* ---- Framebuffers ---- */

static void free_framebuffer(Framebuffer& fb) {
    SDL_GPUTexture* t[3] = { fb.color, fb.resolved, fb.depth };
    for (int i = 0; i < 3; i++) {
        if (t[i] != NULL) {
            SDL_ReleaseGPUTexture(dev, t[i]);
        }
    }
    fb.color = fb.resolved = fb.depth = NULL;
}

static int gfx_sdlgpu_create_framebuffer(void) {
    framebuffers.push_back(Framebuffer());
    return (int)framebuffers.size() - 1;
}

static void gfx_sdlgpu_update_framebuffer_parameters(int fb_id, uint32_t width, uint32_t height, uint32_t msaa_level,
                                                     bool has_depth_buffer) {
    static uint32_t msaa_cap = 0;
    static bool msaa_sampled_ok = true; /* cleared if N64 AA's sampled multisample target is refused */
    Framebuffer& fb = framebuffers[fb_id];

    width = SDL_max(width, 1u);
    height = SDL_max(height, 1u);
    if (msaa_cap == 0) {
        msaa_cap = 1;
        for (uint32_t s = 2; s <= 8; s *= 2) {
            if (SDL_GPUTextureSupportsSampleCount(dev, COLOR_FORMAT, (SDL_GPUSampleCount)msaa_log2(s)) &&
                SDL_GPUTextureSupportsSampleCount(dev, depth_format, (SDL_GPUSampleCount)msaa_log2(s))) {
                msaa_cap = s;
            }
        }
        sysLogPrintf(LOG_NOTE, "SDL_gpu: up to %ux MSAA", msaa_cap);
    }
    if (fb_id == 0) {
        msaa_level = 1;
    }
    while (msaa_level > 1 && msaa_level > msaa_cap) {
        msaa_level >>= 1;
    }
    const bool want_sampled = msaa_level > 1 && gfx_n64_aa != 0 && msaa_sampled_ok;
    if (fb.color != NULL && fb.width == width && fb.height == height && fb.msaa == msaa_level &&
        fb.msaa_sampled == want_sampled) {
        return;
    }
    if (fb_id == current_framebuffer) {
        flush_pass(); /* its recorded draws go to the old targets */
    }
    free_framebuffer(fb);
    const SDL_GPUTextureUsageFlags plain = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    if (msaa_level > 1) {
        fb.msaa_sampled = false;
        if (want_sampled) {
            fb.color = make_texture(width, height, 1, COLOR_FORMAT, plain, msaa_level);
            fb.msaa_sampled = fb.color != NULL;
            if (fb.color == NULL) {
                sysLogPrintf(LOG_WARNING, "SDL_gpu: no sampled %ux MSAA target (%s): N64 AA falls back to "
                             "the plain resolve", msaa_level, SDL_GetError());
                msaa_sampled_ok = false;
            }
        }
        if (fb.color == NULL) {
            fb.color = make_texture(width, height, 1, COLOR_FORMAT, SDL_GPU_TEXTUREUSAGE_COLOR_TARGET, msaa_level);
        }
        fb.resolved = make_texture(width, height, 1, COLOR_FORMAT, plain, 1);
    } else {
        fb.color = make_texture(width, height, 1, COLOR_FORMAT, plain, 1);
        fb.msaa_sampled = false;
    }
    fb.depth = make_texture(width, height, 1, depth_format, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET, msaa_level);
    if (fb.color == NULL || fb.depth == NULL || (msaa_level > 1 && fb.resolved == NULL)) {
        if (msaa_level > 1) {
            sysLogPrintf(LOG_WARNING, "SDL_gpu: %ux MSAA framebuffer refused (%s), trying %ux", msaa_level,
                         SDL_GetError(), msaa_level >> 1);
            free_framebuffer(fb);
            msaa_cap = msaa_level >> 1;
            gfx_sdlgpu_update_framebuffer_parameters(fb_id, width, height, msaa_level >> 1, has_depth_buffer);
            return;
        }
        sysFatalError("SDL_gpu: no %ux%u framebuffer: %s", width, height, SDL_GetError());
    }
    fb.width = width;
    fb.height = height;
    fb.msaa = msaa_level;
    fb.has_depth = true; /* always: one pipeline target layout for every framebuffer */
    if (fb_id == current_framebuffer) {
        pending_clear_color = pending_clear_depth = true; /* new contents are undefined */
    }
}

static bool gfx_sdlgpu_start_draw_to_framebuffer(int fb_id, float noise_scale) {
    if (fb_id >= (int)framebuffers.size()) {
        return false;
    }
    if (fb_id != current_framebuffer) {
        flush_pass();
        current_framebuffer = fb_id;
    }
    if (noise_scale != 0.0f) {
        current_noise_scale = 1.0f / noise_scale;
    }
    return true;
}

/* A clear is a load op of the next pass: the draws before it run first */
static void gfx_sdlgpu_clear_framebuffer(bool clear_color, bool clear_depth) {
    if (!pass_draws.empty()) {
        flush_pass();
    }
    pending_clear_color |= clear_color;
    pending_clear_depth |= clear_depth;
}

/* The single-sample image of a framebuffer, for a blit: with MSAA, resolved by
   an empty pass over it */
static SDL_GPUTexture* resolved_color(int fb_id) {
    Framebuffer& fb = framebuffers[fb_id];
    if (fb.msaa <= 1) {
        return fb.color;
    }
    SDL_GPUColorTargetInfo ct;
    SDL_zero(ct);
    ct.texture = fb.color;
    ct.load_op = SDL_GPU_LOADOP_LOAD;
    ct.store_op = SDL_GPU_STOREOP_RESOLVE_AND_STORE;
    ct.resolve_texture = fb.resolved;
    SDL_EndGPURenderPass(SDL_BeginGPURenderPass(get_cmd(), &ct, 1, NULL));
    return fb.resolved;
}

static void blit(SDL_GPUTexture* src, uint32_t sw, uint32_t sh, SDL_GPUTexture* dst, int dx, int dy, int dw, int dh,
                 SDL_GPUFilter filter, SDL_FlipMode flip) {
    SDL_GPUBlitInfo b;
    SDL_zero(b);
    b.source.texture = src;
    b.source.w = sw;
    b.source.h = sh;
    b.destination.texture = dst;
    b.destination.x = dx;
    b.destination.y = dy;
    b.destination.w = dw;
    b.destination.h = dh;
    b.load_op = SDL_GPU_LOADOP_LOAD;
    b.flip_mode = flip;
    b.filter = filter;
    SDL_BlitGPUTexture(get_cmd(), &b);
}

/* The window rect a present into framebuffer 0 lands in (gfx_opengl_present_x) */
static void present_x(int fb_id, uint32_t full_w, int* x0, int* x1) {
    if (fb_id == 0 && g_GfxPresentW > 0) {
        *x0 = g_GfxPresentX;
        *x1 = g_GfxPresentX + g_GfxPresentW;
    } else {
        *x0 = 0;
        *x1 = (int)full_w;
    }
}

static void gfx_sdlgpu_resolve_msaa_color_buffer(int fb_id_target, int fb_id_source) {
    Framebuffer& dst = framebuffers[fb_id_target];
    Framebuffer& src = framebuffers[fb_id_source];
    int x0, x1;

    /* the target's pending clear (and the source's draws, if it is current) first */
    flush_pass();
    present_x(fb_id_target, dst.width, &x0, &x1);
    blit(resolved_color(fb_id_source), src.width, src.height, dst.color, x0, 0, x1 - x0, (int)dst.height,
         SDL_GPU_FILTER_NEAREST, SDL_FLIP_NONE);
}

/* ---- N64 AA: gfx_opengl.cpp's VI-style resolve, the same two passes ---- */

static SDL_GPUShader *vi_vs, *vi_fs[2];
static SDL_GPUGraphicsPipeline* vi_pipeline[2];
static SDL_GPUTexture* vi_tex[2];
static SDL_GPUSampler* vi_sampler;
static uint32_t vi_w, vi_h;
static bool vi_failed;

static SDL_GPUShader* make_shader(const uint32_t* code, size_t size, SDL_GPUShaderStage stage, uint32_t samplers_n,
                                  uint32_t uniforms_n) {
    SDL_GPUShaderCreateInfo ci;
    SDL_zero(ci);
    ci.code = (const Uint8*)code;
    ci.code_size = size;
    ci.entrypoint = "main";
    ci.format = SDL_GPU_SHADERFORMAT_SPIRV;
    ci.stage = stage;
    ci.num_samplers = samplers_n;
    ci.num_uniform_buffers = uniforms_n;
    return SDL_CreateGPUShader(dev, &ci);
}

static bool vi_init(void) {
    if (vi_pipeline[0] != NULL) {
        return true;
    }
    if (vi_failed) {
        return false;
    }
    vi_vs = make_shader(vi_vert_spv, sizeof(vi_vert_spv), SDL_GPU_SHADERSTAGE_VERTEX, 0, 0);
    vi_fs[0] = make_shader(vi_coverage_frag_spv, sizeof(vi_coverage_frag_spv), SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 1);
    vi_fs[1] = make_shader(vi_blend_frag_spv, sizeof(vi_blend_frag_spv), SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 1);
    for (int i = 0; i < 2 && vi_vs != NULL && vi_fs[i] != NULL; i++) {
        SDL_GPUColorTargetDescription ctd;
        SDL_GPUGraphicsPipelineCreateInfo ci;
        SDL_zero(ctd);
        SDL_zero(ci);
        ctd.format = COLOR_FORMAT;
        ci.vertex_shader = vi_vs;
        ci.fragment_shader = vi_fs[i];
        ci.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
        ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
        ci.target_info.color_target_descriptions = &ctd;
        ci.target_info.num_color_targets = 1;
        vi_pipeline[i] = SDL_CreateGPUGraphicsPipeline(dev, &ci);
    }
    SDL_GPUSamplerCreateInfo si;
    SDL_zero(si);
    si.address_mode_u = si.address_mode_v = si.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    vi_sampler = SDL_CreateGPUSampler(dev, &si);
    if (vi_pipeline[0] == NULL || vi_pipeline[1] == NULL || vi_sampler == NULL) {
        sysLogPrintf(LOG_ERROR, "N64 AA: the VI resolve failed to set up (%s) - plain MSAA resolve instead",
                     SDL_GetError());
        vi_failed = true;
        return false;
    }
    sysLogPrintf(LOG_NOTE, "N64 AA: VI resolve ready");
    return true;
}

static float vi_strength(void) {
    static float s = -1.0f;
    if (s < 0.0f) {
        const char* e = getenv("DD64_N64AA_STRENGTH"); /* 1.0 = the VI's full blend by missing coverage */
        s = e ? (float)atof(e) : 1.0f;
        if (s < 0.0f) s = 0.0f;
    }
    return s;
}

/* One full-target pass of the VI resolve: src (sampled) into dst */
static void vi_pass(int which, SDL_GPUTexture* src, SDL_GPUTexture* dst, const void* uniforms, size_t len) {
    SDL_GPUColorTargetInfo ct;
    SDL_zero(ct);
    ct.texture = dst;
    ct.load_op = SDL_GPU_LOADOP_DONT_CARE;
    ct.store_op = SDL_GPU_STOREOP_STORE;
    SDL_PushGPUFragmentUniformData(get_cmd(), 0, uniforms, (Uint32)len);
    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &ct, 1, NULL);
    SDL_GPUViewport v = { 0.0f, 0.0f, (float)vi_w, (float)vi_h, 0.0f, 1.0f };
    SDL_GPUTextureSamplerBinding b = { src, vi_sampler };
    SDL_BindGPUGraphicsPipeline(pass, vi_pipeline[which]);
    SDL_SetGPUViewport(pass, &v);
    SDL_BindGPUFragmentSamplers(pass, 0, &b, 1);
    SDL_DrawGPUPrimitives(pass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(pass);
}

static void gfx_sdlgpu_resolve_n64_aa(int fb_dst, int fb_src) {
    if (fb_src <= 0 || fb_src >= (int)framebuffers.size() || fb_dst < 0 || fb_dst >= (int)framebuffers.size()) {
        return;
    }
    Framebuffer& src = framebuffers[fb_src];
    Framebuffer& dst = framebuffers[fb_dst];
    if (src.msaa <= 1 || !src.msaa_sampled || !vi_init()) {
        gfx_sdlgpu_resolve_msaa_color_buffer(fb_dst, fb_src); /* the plain resolve */
        return;
    }
    flush_pass();
    if (vi_w != src.width || vi_h != src.height) {
        for (int i = 0; i < 2; i++) {
            if (vi_tex[i] != NULL) {
                SDL_ReleaseGPUTexture(dev, vi_tex[i]);
            }
            vi_tex[i] = make_texture(src.width, src.height, 1, COLOR_FORMAT,
                                     SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER, 1);
        }
        vi_w = src.width;
        vi_h = src.height;
    }
    /* pass 1: samples -> mean colour + coverage; pass 2: the VI's edge blend */
    int32_t u1[4] = { (int32_t)src.msaa, 0, 0, 0 };
    vi_pass(0, src.color, vi_tex[0], u1, sizeof(u1));
    struct {
        int32_t size[4];
        float strength[4];
    } u2 = { { (int32_t)src.width, (int32_t)src.height, 0, 0 }, { vi_strength(), 0.0f, 0.0f, 0.0f } };
    vi_pass(1, vi_tex[0], vi_tex[1], &u2, sizeof(u2));
    /* present: the soft resample to the destination */
    static int soft = -1;
    int x0, x1;
    if (soft < 0) {
        const char* e = getenv("DD64_N64AA_SOFT");
        soft = (e && e[0] == '0') ? 0 : 1;
    }
    present_x(fb_dst, dst.width, &x0, &x1);
    blit(vi_tex[1], src.width, src.height, dst.color, x0, 0, x1 - x0, (int)dst.height,
         soft ? SDL_GPU_FILTER_LINEAR : SDL_GPU_FILTER_NEAREST, SDL_FLIP_NONE);
}

/* ---- The window: the FPS counter, screenshots, the swap ---- */

/*
 * The FPS counter: gfx_opengl_draw_counter's 3x5 digits on a dark box, here an
 * image of the box at one pixel a dot, blitted up to the scale (the same
 * pixels as GL's scissored clears).
 */
static void gfx_sdlgpu_draw_counter(int value) {
    static const uint16_t font[10] = {
        /* 3x5 digits, 15 bits each, row-major from the top */
        0x7B6F, 0x2C97, 0x73E7, 0x73CF, 0x5BC9, 0x79CF, 0x79EF, 0x7249, 0x7BEF, 0x7BCF,
    };
    static SDL_GPUTexture* tex;
    static uint32_t tex_w;
    char text[12];
    const int h = (int)framebuffers[0].height, scale = h / 240 > 1 ? h / 240 : 1;
    const int n = snprintf(text, sizeof(text), "%d", value < 0 ? 0 : value);
    const uint32_t w = 4 * n + 1;
    std::vector<uint32_t> px(w * 7, 0xFF000000u); /* RGBA bytes 0,0,0,255 */

    for (int i = 0; i < n; i++) {
        uint16_t bits = font[text[i] - '0'];
        for (int r = 0; r < 5; r++) {
            for (int c = 0; c < 3; c++) {
                if (bits & (1 << (14 - (r * 3 + c)))) {
                    /* bottom row first: the box's row 1 + r from the top */
                    px[(5 - r) * w + 1 + 4 * i + c] = 0xFF33FFFFu; /* 255, 255, 51 (GL's 1, 1, 0.2) */
                }
            }
        }
    }
    flush_pass();
    if (tex == NULL || tex_w != w) {
        if (tex != NULL) {
            SDL_ReleaseGPUTexture(dev, tex);
        }
        tex = make_texture(w, 7, 1, COLOR_FORMAT, SDL_GPU_TEXTUREUSAGE_SAMPLER, 1);
        tex_w = w;
    }
    TexUpload u = { tex, stage(px.data(), px.size() * 4), w, 7, false };
    tex_uploads.push_back(u);
    run_uploads();
    blit(tex, w, 7, framebuffers[0].color, 2 * scale, h - 9 * scale, (int)w * scale, 7 * scale,
         SDL_GPU_FILTER_NEAREST, SDL_FLIP_NONE);
}

/* Save the window's image as a BMP (before the swap) */
static void gfx_sdlgpu_screenshot(const char* path) {
    const Framebuffer& fb = framebuffers[0];
    const uint32_t w = fb.width, h = fb.height, bytes = w * h * 4;
    SDL_GPUTransferBufferCreateInfo ci;
    SDL_zero(ci);
    ci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
    ci.size = bytes;
    SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(dev, &ci);
    if (tb == NULL) {
        return;
    }
    flush_pass();
    SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(get_cmd());
    SDL_GPUTextureRegion src;
    SDL_GPUTextureTransferInfo dst;
    SDL_zero(src);
    SDL_zero(dst);
    src.texture = fb.color;
    src.w = w;
    src.h = h;
    src.d = 1;
    dst.transfer_buffer = tb;
    SDL_DownloadFromGPUTexture(copy, &src, &dst);
    SDL_EndGPUCopyPass(copy);
    SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);
    cmd = NULL;
    if (fence != NULL) {
        SDL_WaitForGPUFences(dev, true, &fence, 1);
        SDL_ReleaseGPUFence(dev, fence);
    }
    const uint8_t* px = (const uint8_t*)SDL_MapGPUTransferBuffer(dev, tb, false);
    if (px != NULL) {
        /* bottom row first, as glReadPixels gives it */
        std::vector<uint8_t> flipped(bytes);
        for (uint32_t y = 0; y < h; y++) {
            memcpy(&flipped[(size_t)y * w * 4], px + (size_t)(h - 1 - y) * w * 4, (size_t)w * 4);
        }
        SDL_UnmapGPUTransferBuffer(dev, tb);
        SDL_Surface* s = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_ABGR8888, flipped.data(), w * 4);
        if (s != NULL) {
            SDL_SaveBMP(s, path);
            SDL_DestroySurface(s);
        }
    }
    SDL_ReleaseGPUTransferBuffer(dev, tb);
}

/* gfx_sdl3.cpp's swap: the window's image onto the swapchain, upside up, and
   the frame's commands submitted */
extern "C" void gfx_sdlgpu_swap(void) {
    SDL_GPUTexture* sc = NULL;
    Uint32 w = 0, h = 0;

    flush_pass();
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(get_cmd(), wnd, &sc, &w, &h)) {
        sysLogPrintf(LOG_WARNING, "SDL_gpu: no swapchain image: %s", SDL_GetError());
    } else if (sc != NULL) { /* NULL: minimised */
        const Framebuffer& fb = framebuffers[0];
        blit(fb.color, fb.width, fb.height, sc, 0, 0, (int)w, (int)h, SDL_GPU_FILTER_NEAREST, SDL_FLIP_VERTICAL);
    }
    SDL_SubmitGPUCommandBuffer(cmd);
    cmd = NULL;
}

/* gfx_sdl3.cpp: 0 no vsync, 1 vsync, -1 adaptive (SDL_gpu has none: vsync) */
extern "C" bool gfx_sdlgpu_set_swap_interval(int interval) {
    SDL_GPUPresentMode mode = SDL_GPU_PRESENTMODE_VSYNC;
    if (interval == 0) {
        if (SDL_WindowSupportsGPUPresentMode(dev, wnd, SDL_GPU_PRESENTMODE_IMMEDIATE)) {
            mode = SDL_GPU_PRESENTMODE_IMMEDIATE;
        } else if (SDL_WindowSupportsGPUPresentMode(dev, wnd, SDL_GPU_PRESENTMODE_MAILBOX)) {
            mode = SDL_GPU_PRESENTMODE_MAILBOX;
        }
    }
    return SDL_SetGPUSwapchainParameters(dev, wnd, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, mode);
}

/* ---- Initialisation ---- */

static void gfx_sdlgpu_init(void) {
    const char* e = getenv("DD64_DEC_ZBIAS");
    if (e != NULL) {
        sscanf(e, "%f,%f", &dec_bias_f, &dec_bias_u);
    }

    wnd = gfx_sdl_window();
    dev = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, getenv("DD64_DEBUG_GPU") != NULL, NULL);
    if (dev == NULL) {
        sysFatalError("Could not start SDL_gpu (Vulkan).\nReported SDL error: %s\n"
                      "Set renderer = opengl in dd64.ini to use OpenGL.", SDL_GetError());
    }
    if (!SDL_ClaimWindowForGPUDevice(dev, wnd)) {
        sysFatalError("SDL_gpu could not use the window: %s", SDL_GetError());
    }
    /* (SDL 3.2 cannot name the device: SDL_GetGPUDeviceProperties is 3.4's) */
    sysLogPrintf(LOG_NOTE, "SDL_gpu: driver %s", SDL_GetGPUDeviceDriver(dev));

    static const SDL_GPUTextureFormat depths[] = { SDL_GPU_TEXTUREFORMAT_D24_UNORM, SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT,
                                                   SDL_GPU_TEXTUREFORMAT_D32_FLOAT };
    depth_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    for (size_t i = 0; i < SDL_arraysize(depths); i++) {
        if (SDL_GPUTextureSupportsFormat(dev, depths[i], SDL_GPU_TEXTURETYPE_2D,
                                         SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)) {
            depth_format = depths[i];
            break;
        }
    }
    sysLogPrintf(LOG_NOTE, "SDL_gpu: depth buffer %s", depth_format == SDL_GPU_TEXTUREFORMAT_D32_FLOAT ? "32-bit float"
                                                       : "24-bit");

    combiner_vs = make_shader(combiner_vert_spv, sizeof(combiner_vert_spv), SDL_GPU_SHADERSTAGE_VERTEX, 0, 0);
    combiner_fs = make_shader(combiner_frag_spv, sizeof(combiner_frag_spv), SDL_GPU_SHADERSTAGE_FRAGMENT, 2, 1);
    if (combiner_vs == NULL || combiner_fs == NULL) {
        sysFatalError("SDL_gpu: the combiner shader failed: %s", SDL_GetError());
    }

    /* the texture unused sampler slots get: one white texel */
    dummy_texture = make_texture(1, 1, 1, COLOR_FORMAT, SDL_GPU_TEXTUREUSAGE_SAMPLER, 1);
    dummy_sampler = get_sampler(false, false, 1, 0, 0);
    const uint32_t white = 0xFFFFFFFFu;
    TexUpload u = { dummy_texture, stage(&white, 4), 1, 1, false };
    tex_uploads.push_back(u);

    framebuffers.resize(1); /* the window's */
    gfx_sdlgpu_set_swap_interval(1);
}

static void gfx_sdlgpu_start_frame(void) {
    frame_count++;
}

static void gfx_sdlgpu_end_frame(void) {
    flush_pass();
}

struct GfxRenderingAPI gfx_sdlgpu_api = {
    gfx_sdlgpu_unload_shader,
    gfx_sdlgpu_load_shader,
    gfx_sdlgpu_create_and_load_new_shader,
    gfx_sdlgpu_lookup_shader,
    gfx_sdlgpu_new_texture,
    gfx_sdlgpu_select_texture,
    gfx_sdlgpu_upload_texture,
    gfx_sdlgpu_set_sampler_parameters,
    gfx_sdlgpu_set_depth_mode,
    gfx_sdlgpu_set_viewport,
    gfx_sdlgpu_set_scissor,
    gfx_sdlgpu_set_use_alpha,
    gfx_sdlgpu_draw_triangles,
    gfx_sdlgpu_init,
    gfx_sdlgpu_start_frame,
    gfx_sdlgpu_end_frame,
    gfx_sdlgpu_create_framebuffer,
    gfx_sdlgpu_update_framebuffer_parameters,
    gfx_sdlgpu_start_draw_to_framebuffer,
    gfx_sdlgpu_clear_framebuffer,
    gfx_sdlgpu_resolve_msaa_color_buffer,
    gfx_sdlgpu_delete_texture,
    gfx_sdlgpu_set_texture_filter,
    gfx_sdlgpu_resolve_n64_aa,
    gfx_sdlgpu_upload_vertices,
    gfx_sdlgpu_draw_uploaded,
    gfx_sdlgpu_draw_counter,
    gfx_sdlgpu_screenshot,
};
