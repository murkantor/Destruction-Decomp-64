/*
 * Fast3D display list interpreter for Destruction Derby 64: F3DEX2 2.07 and
 * S2DEX2 2.07, as the game uses them, drawn through a GfxRenderingAPI.
 *
 * Built on the Fast3D lineage Project Midas uses (Emill's n64-fast3d-engine);
 * the combiner, texture decoders and the OpenGL backend come from there.
 *
 * Game memory is read through rd16/rd32: data the game built is in host order,
 * data straight from the ROM is still big-endian until the game reads it
 * (port/src/shadow.c), so every multi-byte read asks which.
 *
 * Addresses in commands are host pointers, N64 segment addresses (0x0Sxxxxxx),
 * or N64 addresses still inside ROM data (0x80xxxxxx); seg_addr sorts them out.
 */
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

#define F3DEX_GBI_2
#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif
#include <PR/gbi.h>
#include <PR/gs2dex.h>

#include "gfx_pc.h"
#include "gfx_cc.h"
#include "gfx_rendering_api.h"
#include "gfx_window_manager_api.h"
#include "gfx_screen_config.h"
#include "port_shadow.h"

#include <SDL3/SDL.h>

/* ---- Settings ---- */

uint32_t gfx_msaa_level = 1;
uint32_t gfx_n64_aa = 0;
bool gfx_framebuffers_enabled = true;
uint32_t gfx_anisotropy = 1;
uint32_t gfx_internal_height = 0;
uint32_t gfx_aspect_mode = 0;
uint32_t gfx_edge_fill = 0;
uint32_t gfx_draw_distance = 1;

static void fill_clear_texels(uint8_t *rgba, uint32_t w, uint32_t h);

extern int g_GfxPresentX, g_GfxPresentW;

/* ---- Constants ---- */

#define NATIVE_WIDTH 320
#define NATIVE_HEIGHT 240

#define MAX_BUFFERED 256
#define MAX_LIGHTS 7
#define MAX_VERTICES 64
#define MATRIX_STACK 18

#define SCALE_5_8(v) (((v) * 0xFF) / 0x1F)
#define SCALE_4_8(v) ((v) * 0x11)
#define SCALE_3_8(v) ((v) * 0x24)

/* Microcode text addresses (N64 physical, as gSPLoadUcodeL passes them) */
#define UCODE_S2DEX2 0x0007D050

/* S2DEX2 opcodes that differ from F3DEX2's */
#define S2_OBJ_RECTANGLE 0x01
#define S2_OBJ_SPRITE 0x02
#define S2_SELECT_DL 0x04
#define S2_OBJ_LOADTXTR 0x05
#define S2_OBJ_LDTX_SPRITE 0x06
#define S2_OBJ_LDTX_RECT 0x07
#define S2_OBJ_LDTX_RECT_R 0x08
#define S2_BG_1CYC 0x09
#define S2_BG_COPY 0x0A
#define S2_OBJ_RENDERMODE 0x0B
#define S2_OBJ_RECTANGLE_R 0xDA
#define S2_OBJ_MOVEMEM 0xDC

/* ---- Reading game memory ---- */

static inline uint32_t rd32(const void *p) {
    return Port_RdN64(p, 4);
}

static inline uint16_t rd16(const void *p) {
    return (uint16_t)Port_RdN64(p, 2);
}

static inline int16_t rds16(const void *p) {
    return (int16_t)rd16(p);
}

static inline uint8_t rd8(const void *p) {
    uint32_t off = (uint32_t)(uintptr_t)p - (uint32_t)(uintptr_t)gRam;
    uint8_t b;

    if (off >= PORT_RAM_SIZE || (gPortShadow[off] >> 4) < 2) {
        return *(const uint8_t *)p;
    }
    Port_ReadN64Bytes(&b, p, 1); /* a byte of a value written wider */
    return b;
}

/* DD64_DLDUMP's task: matrices and the first vertices are logged too */
static bool dl_dumping;
static int dl_dumped_vtx;

/* A 16-bit texel or palette entry. Texture data the game never touched is
 * still in the cart's byte order; data the game wrote as u16 is in host order. */
static inline uint16_t tex16(const uint8_t *base, uint32_t off) {
    return rd16(base + off);
}

static uintptr_t segment_base[16];
static bool s2dex; /* the current microcode is S2DEX2 */

static const uint8_t *n64_to_host(uint32_t addr) {
    return (const uint8_t *)Port_N64ToHost(addr);
}

static const uint8_t *seg_addr(uint32_t w1) {
    if (w1 >= 0x80000000u) {
        return n64_to_host(0x80000000u | (w1 & 0x1FFFFFFF));
    }
    if (w1 < 0x10000000u) {
        uint32_t seg = (w1 >> 24) & 0xF;
        uint32_t off = w1 & 0x00FFFFFF;
        if (segment_base[seg] != 0) {
            return (const uint8_t *)(segment_base[seg] + off);
        }
        return n64_to_host(0x80000000u | off); /* physical */
    }
    return (const uint8_t *)(uintptr_t)w1;
}

/* ---- Fast3D state ---- */

struct RGBA {
    uint8_t r, g, b, a;
};

struct LoadedVertex {
    float x, y, z, w;
    float u, v;
    struct RGBA color;
    uint8_t clip_rej;
    /* for frame interpolation (below): the model-space position, the matrix slot
       (-1: drawn as it is, a rectangle say), the widescreen x scale and z offset */
    uint8_t wide;
    int16_t slot;
    float ox, oy, oz, zoff;
};

struct TextureCacheKey {
    const uint8_t *addr;
    uint32_t palette_sum;
    uint32_t size_bytes;
    uint32_t line_size_bytes;
    uint8_t fmt, siz;
    bool operator==(const TextureCacheKey &o) const {
        return addr == o.addr && palette_sum == o.palette_sum && size_bytes == o.size_bytes &&
               line_size_bytes == o.line_size_bytes && fmt == o.fmt && siz == o.siz;
    }
};

struct TextureCacheKeyHash {
    size_t operator()(const TextureCacheKey &k) const {
        uintptr_t a = (uintptr_t)k.addr;
        return (size_t)(a ^ (a >> 5) ^ k.palette_sum ^ (k.size_bytes << 3) ^ (k.fmt << 8 | k.siz));
    }
};

struct TextureCacheValue {
    uint32_t texture_id;
    uint32_t width, height;
    uint32_t checksum;
};

static std::unordered_map<TextureCacheKey, TextureCacheValue, TextureCacheKeyHash> texture_cache;

struct ColorCombiner {
    uint64_t shader_id0;
    uint32_t shader_id1;
    bool used_textures[2];
    struct ShaderProgram *prg;
    uint8_t shader_input_mapping[2][7];
};

static std::map<ColorCombinerKey, ColorCombiner> color_combiner_pool;
static enum FilteringMode texture_filter_mode = FILTER_LINEAR; /* gfx_set_texture_filter */

static struct RSP {
    float modelview_matrix_stack[MATRIX_STACK][4][4];
    uint8_t modelview_matrix_stack_size;
    float MP_matrix[4][4];
    float P_matrix[4][4];

    struct {
        uint8_t col[3];
        int8_t dir[3];
    } lights[MAX_LIGHTS + 1]; /* [num_lights] is ambient */
    uint8_t num_lights;
    float lights_coeffs[MAX_LIGHTS][3];
    int8_t lookat[2][3];
    float lookat_coeffs[2][3];
    bool lights_changed;

    uint32_t geometry_mode;
    int16_t fog_mul, fog_offset;

    struct {
        uint16_t s, t;
    } texture_scaling_factor;
    uint8_t texture_tile;

    struct LoadedVertex loaded_vertices[MAX_VERTICES + 4];

    /* S2DEX object matrix */
    float obj_a, obj_d, obj_x, obj_y, obj_base_x, obj_base_y; /* B and C (rotation) are not drawn */
} rsp;

static struct RDP {
    uint16_t palette[256];
    uint32_t palette_sum;
    struct {
        const uint8_t *addr;
        uint8_t fmt, siz;
        uint32_t width;
    } texture_to_load;
    struct {
        uint8_t fmt, siz;
        uint8_t cms, cmt;
        uint8_t shifts, shiftt;
        uint16_t uls, ult, lrs, lrt; /* U10.2 */
        uint16_t tmem;
        uint32_t line_size_bytes;
        uint8_t palette;
    } texture_tile[8];
    struct {
        const uint8_t *addr;
        uint32_t size_bytes;
        uint32_t full_line_size_bytes;
        uint32_t line_size_bytes;
    } loaded_texture[512];
    bool textures_changed[2];

    uint32_t other_mode_l, other_mode_h;
    uint64_t combine_mode;

    struct RGBA env_color, prim_color, fog_color, fill_color, blend_color;
    uint8_t prim_lod_fraction;
    uint16_t prim_depth;
    struct XYWidthHeight viewport, scissor;
    bool viewport_or_scissor_changed;
    const uint8_t *z_buf_address;
    const uint8_t *color_image_address;
    uint32_t rdphalf_1;
} rdp;

static struct RenderingState {
    bool depth_test, depth_update, depth_compare, depth_source_prim;
    uint16_t zmode;
    bool alpha_blend;
    struct XYWidthHeight viewport, scissor;
    struct ShaderProgram *shader_program;
    uint32_t textures[2];
    bool linear[2];
    uint8_t cms[2], cmt[2];
} rendering_state;

static struct GfxWindowManagerAPI *gfx_wapi;

/* Frames presented so far (DD64_SHOT and DD64_PRESS count these) */
extern "C" unsigned gfx_presents;
unsigned gfx_presents;

/* The FPS counter in the top-left corner (set by port/src/video.c) */
extern "C" bool gfx_show_fps;
bool gfx_show_fps;
static struct GfxRenderingAPI *gfx_rapi;

static uint32_t window_width, window_height;
static uint32_t fb_width, fb_height;   /* the game framebuffer (internal resolution) */
/* Widescreen (gfx_aspect_mode 2): the framebuffer has the window's shape. Perspective
 * (3D) draws use the whole width, with clip x scaled by wide_k so nothing is stretched
 * and more of the scene shows at the sides; everything else (the HUD, menus, screens
 * drawn in 2D) keeps its 4:3 area in the middle. */
static float wide_k = 1.0f;            /* 4:3 width / framebuffer width */
/* The viewport's z translate over its z scale: the RSP adds it to every vertex's
 * screen z, which the game uses as a depth bias (vtrans z -8 around particles and
 * skid marks, so they sit on the road instead of fighting it) */
static float vp_z_offset;
static float vp_z_scale = 1023.0f; /* the viewport's z scale (G_MAXZ, usually) */
/* G_ZS_PRIM: the depth the triangle being emitted has at every pixel, on the scale
   vertices' z/w are on (NAN: its own) */
static float emit_zprim = NAN;
static bool vp_wide;                   /* the current draw uses the whole width */
static int game_fb;                    /* the game draws here, presented to framebuffer 0 */
static bool frame_started;

static float buf_vbo[MAX_BUFFERED * (32 * 3)];
static size_t buf_vbo_len;
static size_t buf_vbo_num_tris;

static uint8_t *tex_upload_buffer;

/* DD64_GFXLOG=1: per-frame command counts in the log, once a second */
static struct {
    unsigned tris, fills, texrects, s2, dls, tasks, cmds;
} stats;

/* DD64_GFXLOG: what ended each batch (a draw call), counted per frame */
static const char *flush_why = "other";
static const char *flush_whys[24];
static unsigned flush_counts[24], flush_frames;
#define FLUSH(why) (flush_why = (why), gfx_flush())

static void flush_log_frame(void) {
    static const char *gfxlog = getenv("DD64_GFXLOG");
    char line[512];
    int n = 0;

    if (gfxlog == NULL || ++flush_frames < 300) {
        return;
    }
    for (int i = 0; i < 24 && flush_whys[i] != NULL; i++) {
        n += snprintf(line + n, sizeof(line) - n, " %s %.0f,", flush_whys[i], flush_counts[i] / (double)flush_frames);
        flush_counts[i] = 0;
    }
    sysLogPrintf(LOG_NOTE, "gfx: draws a frame by what ended them:%s", line);
    flush_frames = 0;
}

static void gfx_flush(void) {
    if (buf_vbo_len > 0) {
        for (int i = 0; i < 24; i++) {
            if (flush_whys[i] == NULL || flush_whys[i] == flush_why) {
                flush_whys[i] = flush_why;
                flush_counts[i]++;
                break;
            }
        }
        flush_why = "other";
        gfx_rapi->draw_triangles(buf_vbo, buf_vbo_len, buf_vbo_num_tris);
        buf_vbo_len = 0;
        buf_vbo_num_tris = 0;
    }
}

/* ---- Combiners ---- */

static void gfx_generate_cc(struct ColorCombiner *comb, const ColorCombinerKey &key) {
    bool is_2cyc = (key.options & (uint64_t)SHADER_OPT_2CYC) != 0;
    uint8_t c[2][2][4] = { { { 0 } } };
    uint64_t shader_id0 = 0;
    uint32_t shader_id1 = (uint32_t)key.options;
    uint8_t shader_input_mapping[2][7] = { { 0 } };
    bool used_textures[2] = { false, false };

    for (int i = 0; i < 2 && (i == 0 || is_2cyc); i++) {
        uint32_t rgb_a = (key.combine_mode >> (i * 28)) & 0xf;
        uint32_t rgb_b = (key.combine_mode >> (i * 28 + 4)) & 0xf;
        uint32_t rgb_c = (key.combine_mode >> (i * 28 + 8)) & 0x1f;
        uint32_t rgb_d = (key.combine_mode >> (i * 28 + 13)) & 7;
        uint32_t alpha_a = (key.combine_mode >> (i * 28 + 16)) & 7;
        uint32_t alpha_b = (key.combine_mode >> (i * 28 + 16 + 3)) & 7;
        uint32_t alpha_c = (key.combine_mode >> (i * 28 + 16 + 6)) & 7;
        uint32_t alpha_d = (key.combine_mode >> (i * 28 + 16 + 9)) & 7;

        if (rgb_a >= 8) rgb_a = G_CCMUX_0;
        if (rgb_b >= 8) rgb_b = G_CCMUX_0;
        if (rgb_c >= 16) rgb_c = G_CCMUX_0;
        if (rgb_d == 7) rgb_d = G_CCMUX_0;
        if (rgb_a == rgb_b || rgb_c == G_CCMUX_0) {
            rgb_a = rgb_b = rgb_c = G_CCMUX_0;
        }
        if (alpha_a == alpha_b || alpha_c == G_ACMUX_0) {
            alpha_a = alpha_b = alpha_c = G_ACMUX_0;
        }
        if (i == 1) {
            if (rgb_a != G_CCMUX_COMBINED && rgb_b != G_CCMUX_COMBINED && rgb_c != G_CCMUX_COMBINED &&
                rgb_d != G_CCMUX_COMBINED) {
                c[0][0][0] = c[0][0][1] = c[0][0][2] = c[0][0][3] = G_CCMUX_0;
            }
            if (rgb_c != G_CCMUX_COMBINED_ALPHA && alpha_a != G_ACMUX_COMBINED && alpha_b != G_ACMUX_COMBINED &&
                alpha_d != G_ACMUX_COMBINED) {
                c[0][1][0] = c[0][1][1] = c[0][1][2] = c[0][1][3] = G_ACMUX_0;
            }
        }
        c[i][0][0] = rgb_a;
        c[i][0][1] = rgb_b;
        c[i][0][2] = rgb_c;
        c[i][0][3] = rgb_d;
        c[i][1][0] = alpha_a;
        c[i][1][1] = alpha_b;
        c[i][1][2] = alpha_c;
        c[i][1][3] = alpha_d;
    }
    if (!is_2cyc) {
        for (int i = 0; i < 2; i++) {
            for (int k = 0; k < 4; k++) {
                c[1][i][k] = i == 0 ? G_CCMUX_0 : G_ACMUX_0;
            }
        }
    }
    {
        uint8_t input_number[32] = { 0 };
        int next_input_number = SHADER_INPUT_1;
        for (int i = 0; i < 2 && (i == 0 || is_2cyc); i++) {
            for (int j = 0; j < 4; j++) {
                uint32_t val = 0;
                switch (c[i][0][j]) {
                    case G_CCMUX_0: val = SHADER_0; break;
                    case G_CCMUX_1: val = SHADER_1; break;
                    case G_CCMUX_TEXEL0: val = SHADER_TEXEL0; used_textures[0] = true; break;
                    case G_CCMUX_TEXEL1: val = SHADER_TEXEL1; used_textures[1] = true; break;
                    case G_CCMUX_TEXEL0_ALPHA: val = SHADER_TEXEL0A; used_textures[0] = true; break;
                    case G_CCMUX_TEXEL1_ALPHA: val = SHADER_TEXEL1A; used_textures[1] = true; break;
                    case G_CCMUX_NOISE: /* == G_CCMUX_COMBINED_ALPHA in C, G_CCMUX_K4 in B */
                        if (j != 0) {
                            static bool reported;
                            if (!reported) {
                                reported = true;
                                sysLogPrintf(LOG_WARNING, "gfx: combiner input 7 in slot %c (%016llX) is not supported",
                                             "ABCD"[j], (unsigned long long)key.combine_mode);
                            }
                            break;
                        }
                        val = SHADER_NOISE;
                        break;
                    case G_CCMUX_PRIMITIVE:
                    case G_CCMUX_PRIMITIVE_ALPHA:
                    case G_CCMUX_PRIM_LOD_FRAC:
                    case G_CCMUX_SHADE:
                    case G_CCMUX_SHADE_ALPHA:
                    case G_CCMUX_ENVIRONMENT:
                    case G_CCMUX_ENV_ALPHA:
                    case G_CCMUX_LOD_FRACTION:
                        if (input_number[c[i][0][j]] == 0) {
                            shader_input_mapping[0][next_input_number - 1] = c[i][0][j];
                            input_number[c[i][0][j]] = next_input_number++;
                        }
                        val = input_number[c[i][0][j]];
                        break;
                    case G_CCMUX_COMBINED: val = SHADER_COMBINED; break;
                    default: break;
                }
                shader_id0 |= (uint64_t)val << (i * 32 + j * 4);
            }
        }
    }
    {
        uint8_t input_number[16] = { 0 };
        int next_input_number = SHADER_INPUT_1;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 4; j++) {
                uint32_t val = 0;
                switch (c[i][1][j]) {
                    case G_ACMUX_0: val = SHADER_0; break;
                    case G_ACMUX_TEXEL0: val = SHADER_TEXEL0; used_textures[0] = true; break;
                    case G_ACMUX_TEXEL1: val = SHADER_TEXEL1; used_textures[1] = true; break;
                    case G_ACMUX_LOD_FRACTION: /* == G_ACMUX_COMBINED */
                        if (j != 2) {
                            val = SHADER_COMBINED;
                            break;
                        }
                        c[i][1][j] = G_CCMUX_LOD_FRACTION;
                        [[fallthrough]];
                    case G_ACMUX_1: /* == G_ACMUX_PRIM_LOD_FRAC */
                        if (j != 2) {
                            val = SHADER_1;
                            break;
                        }
                        c[i][1][j] = G_CCMUX_PRIM_LOD_FRAC;
                        [[fallthrough]];
                    case G_ACMUX_PRIMITIVE:
                    case G_ACMUX_SHADE:
                    case G_ACMUX_ENVIRONMENT:
                        if (input_number[c[i][1][j]] == 0) {
                            shader_input_mapping[1][next_input_number - 1] = c[i][1][j];
                            input_number[c[i][1][j]] = next_input_number++;
                        }
                        val = input_number[c[i][1][j]];
                        break;
                }
                shader_id0 |= (uint64_t)val << (i * 32 + 16 + j * 4);
            }
        }
    }
    comb->shader_id0 = shader_id0;
    comb->shader_id1 = shader_id1;
    comb->used_textures[0] = used_textures[0];
    comb->used_textures[1] = used_textures[1];
    comb->prg = NULL;
    memcpy(comb->shader_input_mapping, shader_input_mapping, sizeof(shader_input_mapping));
}

static struct ColorCombiner *gfx_lookup_or_create_color_combiner(const ColorCombinerKey &key) {
    static std::map<ColorCombinerKey, ColorCombiner>::iterator prev = color_combiner_pool.end();

    if (prev != color_combiner_pool.end() && prev->first == key) {
        return &prev->second;
    }
    prev = color_combiner_pool.find(key);
    if (prev != color_combiner_pool.end()) {
        return &prev->second;
    }
    gfx_flush();
    prev = color_combiner_pool.insert(std::make_pair(key, ColorCombiner())).first;
    gfx_generate_cc(&prev->second, key);
    return &prev->second;
}

static struct ShaderProgram *gfx_lookup_or_create_shader_program(uint64_t id0, uint32_t id1) {
    struct ShaderProgram *prg = gfx_rapi->lookup_shader(id0, id1);

    if (prg == NULL) {
        gfx_rapi->unload_shader(rendering_state.shader_program);
        prg = gfx_rapi->create_and_load_new_shader(id0, id1);
        rendering_state.shader_program = prg;
    }
    return prg;
}

/* ---- Textures ---- */

static void import_texture_rgba16(const uint8_t *addr, uint32_t size_bytes, uint32_t line_bytes,
                                  uint32_t full_line_bytes, uint32_t *w, uint32_t *h) {
    uint32_t width = line_bytes / 2, height = size_bytes / line_bytes;
    uint8_t *d = tex_upload_buffer;

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++, d += 4) {
            uint16_t c = tex16(addr, y * full_line_bytes + x * 2);
            d[0] = SCALE_5_8(c >> 11);
            d[1] = SCALE_5_8((c >> 6) & 0x1F);
            d[2] = SCALE_5_8((c >> 1) & 0x1F);
            d[3] = (c & 1) ? 255 : 0;
        }
    }
    *w = width;
    *h = height;
}

static void import_texture_rgba32(const uint8_t *addr, uint32_t size_bytes, uint32_t line_bytes,
                                  uint32_t full_line_bytes, uint32_t *w, uint32_t *h) {
    /* line_bytes counts the TMEM split halves: 2 bytes per texel per half */
    uint32_t width = line_bytes / 2, height = size_bytes / 2 / line_bytes;
    uint8_t *d = tex_upload_buffer;

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++, d += 4) {
            uint32_t c = rd32(addr + y * full_line_bytes * 2 + x * 4);
            d[0] = c >> 24;
            d[1] = c >> 16;
            d[2] = c >> 8;
            d[3] = c;
        }
    }
    *w = width;
    *h = height;
}

/* 4- and 8-bit formats: bytes, the same in either byte order. */
static void import_texture_8(const uint8_t *addr, uint8_t fmt, uint8_t siz, uint8_t palette, uint32_t size_bytes,
                             uint32_t line_bytes, uint32_t full_line_bytes, uint32_t *w, uint32_t *h) {
    uint32_t texels_per_line = siz == G_IM_SIZ_4b ? line_bytes * 2 : line_bytes;
    uint32_t height = size_bytes / line_bytes;
    uint8_t *d = tex_upload_buffer;

    for (uint32_t y = 0; y < height; y++) {
        const uint8_t *row = addr + y * full_line_bytes;
        for (uint32_t x = 0; x < texels_per_line; x++, d += 4) {
            uint8_t v;
            if (siz == G_IM_SIZ_4b) {
                v = (row[x / 2] >> ((x & 1) ? 0 : 4)) & 0xF;
            } else {
                v = row[x];
            }
            if (fmt == G_IM_FMT_CI) {
                uint16_t c = rdp.palette[siz == G_IM_SIZ_4b ? (palette * 16 + v) & 0xFF : v];
                if ((rdp.other_mode_h & (3 << G_MDSFT_TEXTLUT)) == G_TT_IA16) {
                    d[0] = d[1] = d[2] = c >> 8;
                    d[3] = c & 0xFF;
                } else {
                    d[0] = SCALE_5_8(c >> 11);
                    d[1] = SCALE_5_8((c >> 6) & 0x1F);
                    d[2] = SCALE_5_8((c >> 1) & 0x1F);
                    d[3] = (c & 1) ? 255 : 0;
                }
            } else if (fmt == G_IM_FMT_IA) {
                if (siz == G_IM_SIZ_4b) {
                    uint8_t i = SCALE_3_8(v >> 1);
                    d[0] = d[1] = d[2] = i;
                    d[3] = (v & 1) ? 255 : 0;
                } else {
                    d[0] = d[1] = d[2] = SCALE_4_8(v >> 4);
                    d[3] = SCALE_4_8(v & 0xF);
                }
            } else { /* I */
                uint8_t i = siz == G_IM_SIZ_4b ? SCALE_4_8(v) : v;
                d[0] = d[1] = d[2] = d[3] = i;
            }
        }
    }
    *w = texels_per_line;
    *h = height;
}

static void import_texture_ia16(const uint8_t *addr, uint32_t size_bytes, uint32_t line_bytes,
                                uint32_t full_line_bytes, uint32_t *w, uint32_t *h) {
    uint32_t width = line_bytes / 2, height = size_bytes / line_bytes;
    uint8_t *d = tex_upload_buffer;

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++, d += 4) {
            uint16_t c = tex16(addr, y * full_line_bytes + x * 2);
            d[0] = d[1] = d[2] = c >> 8;
            d[3] = c & 0xFF;
        }
    }
    *w = width;
    *h = height;
}

static uint32_t texture_checksum(const uint8_t *addr, uint32_t size) {
    /* Content check so textures the game rewrites in place are re-imported. It
       covers every byte: a sampled check misses partial rewrites (the THQ logo
       reveal changes every 11th texel a step). */
    uint32_t h = 2166136261u, i = 0, w;
    for (; i + 4 <= size; i += 4) {
        memcpy(&w, addr + i, 4);
        h = (h ^ w) * 16777619u;
        h ^= h >> 15;
    }
    for (; i < size; i++) {
        h = (h ^ addr[i]) * 16777619u;
    }
    return h;
}

/* The size of the texture bound in each slot, in texels (0: none) */
static uint32_t bound_w[2], bound_h[2];

/*
 * Bind the texture the tile's load gave slot i, importing it if new or changed.
 * The batch so far is drawn first (gfx_flush), unless the load turns out to be
 * the texture already bound: the game loads a texture again for every few
 * triangles of the same material, and each flush is a draw call.
 */
static void import_texture(int i, int tile) {
    uint8_t fmt = rdp.texture_tile[tile].fmt;
    uint8_t siz = rdp.texture_tile[tile].siz;
    uint32_t tmem = rdp.texture_tile[tile].tmem;
    const uint8_t *addr = rdp.loaded_texture[tmem].addr;
    uint32_t size_bytes = rdp.loaded_texture[tmem].size_bytes;
    uint32_t line_bytes = rdp.texture_tile[tile].line_size_bytes;
    uint32_t full_line_bytes = rdp.loaded_texture[tmem].full_line_size_bytes;

    if (addr == NULL || line_bytes == 0 || size_bytes == 0) {
        FLUSH("texture");
        rendering_state.cms[i] = rendering_state.cmt[i] = 0xFF;
        gfx_rapi->select_texture(i, 0, false);
        rendering_state.textures[i] = 0;
        bound_w[i] = bound_h[i] = 0;
        return;
    }
    if (full_line_bytes == 0) {
        full_line_bytes = line_bytes;
    }
    /* A tile line is padded to 8 bytes: a LoadTile row (33 texels, say) can be
       shorter, and the texture is the rows loaded, not the padding */
    uint32_t loaded_line = rdp.loaded_texture[tmem].line_size_bytes;
    if (siz != G_IM_SIZ_32b && loaded_line != 0 && loaded_line < line_bytes) {
        line_bytes = loaded_line;
    }
    if (siz == G_IM_SIZ_32b) {
        full_line_bytes = rdp.loaded_texture[tmem].full_line_size_bytes / 2;
        if (full_line_bytes == 0) {
            full_line_bytes = line_bytes;
        }
    }

    /* With TLUT mode on, the RDP looks every 4- and 8-bit texel up in the palette,
       whatever the tile's format says (DD64's car bodies are "RGBA 8-bit") */
    uint32_t tlut = rdp.other_mode_h & (3U << G_MDSFT_TEXTLUT);
    if ((siz == G_IM_SIZ_4b || siz == G_IM_SIZ_8b) && tlut != G_TT_NONE) {
        fmt = G_IM_FMT_CI;
    }

    /* CI4 texels index one 16-colour bank of the palette, chosen per tile (the font
       draws each glyph in its own colour this way) */
    uint32_t pal_key = 0;
    if (fmt == G_IM_FMT_CI) {
        pal_key = (rdp.palette_sum ^ tlut) + (siz == G_IM_SIZ_4b ? rdp.texture_tile[tile].palette * 0x9E3779B9u : 0);
    }
    TextureCacheKey key = { addr, pal_key, size_bytes, line_bytes, fmt, siz };
    uint32_t sum = texture_checksum(addr, size_bytes);
    auto it = texture_cache.find(key);
    if (it != texture_cache.end() && it->second.checksum == sum && it->second.texture_id == rendering_state.textures[i] &&
        it->second.width == bound_w[i] && it->second.height == bound_h[i]) {
        return; /* the texture already bound: the batch goes on */
    }
    FLUSH("texture");
    /* GL filter and wrap modes belong to the texture object: whatever gets bound
       here needs them set again (a new one is incomplete, so black, without) */
    rendering_state.cms[i] = rendering_state.cmt[i] = 0xFF;
    if (it != texture_cache.end() && it->second.checksum == sum) {
        gfx_rapi->select_texture(i, it->second.texture_id, false);
        rendering_state.textures[i] = it->second.texture_id;
        bound_w[i] = it->second.width;
        bound_h[i] = it->second.height;
        return;
    }

    uint32_t id;
    if (it != texture_cache.end()) {
        id = it->second.texture_id;
    } else {
        id = gfx_rapi->new_texture();
    }
    gfx_rapi->select_texture(i, id, false);

    uint32_t w = 0, h = 0;
    if (fmt == G_IM_FMT_RGBA && siz == G_IM_SIZ_16b) {
        import_texture_rgba16(addr, size_bytes, line_bytes, full_line_bytes, &w, &h);
    } else if (fmt == G_IM_FMT_RGBA && siz == G_IM_SIZ_32b) {
        import_texture_rgba32(addr, size_bytes, line_bytes, full_line_bytes, &w, &h);
    } else if (fmt == G_IM_FMT_IA && siz == G_IM_SIZ_16b) {
        import_texture_ia16(addr, size_bytes, line_bytes, full_line_bytes, &w, &h);
    } else if (siz == G_IM_SIZ_4b || siz == G_IM_SIZ_8b) {
        /* CI4 picks its palette per tile */
        import_texture_8(addr, fmt, siz, rdp.texture_tile[tile].palette, size_bytes, line_bytes, full_line_bytes, &w,
                         &h);
    } else if (siz == G_IM_SIZ_16b) { /* CI16 / I16: treat as IA16 */
        import_texture_ia16(addr, size_bytes, line_bytes, full_line_bytes, &w, &h);
    } else {
        w = h = 1;
        memset(tex_upload_buffer, 0xFF, 4);
    }
    if (w == 0 || h == 0) {
        w = h = 1;
        memset(tex_upload_buffer, 0, 4);
    }
    if (gfx_edge_fill) {
        fill_clear_texels(tex_upload_buffer, w, h);
    }
    gfx_rapi->upload_texture(tex_upload_buffer, w, h);
    {
        /* DD64_TEXDUMP=1: save each upload as tex_<n>_<w>x<h>.bmp (the first 200; with
         * DD64_DLDUMP set, only the uploads of the dumped task) */
        static const char *dump = getenv("DD64_TEXDUMP");
        static const char *dldump = getenv("DD64_DLDUMP");
        static int n;
        if (dump != NULL && n < 200 && (dldump == NULL || dl_dumping)) {
            char path[64];
            snprintf(path, sizeof(path), "tex_%03d_%ux%u_f%us%u.bmp", n++, w, h, fmt, siz);
            SDL_Surface *s = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_ABGR8888, tex_upload_buffer, w * 4);
            if (s != NULL) {
                SDL_SaveBMP(s, path);
                SDL_DestroySurface(s);
            }
        }
    }
    texture_cache[key] = { id, w, h, sum };
    rendering_state.textures[i] = id;
    bound_w[i] = w;
    bound_h[i] = h;
}

/*
 * [video] edge_fill: a cut-out texture's see-through texels (alpha 0) are mostly
 * black, and filtering mixes them into the edge texels, which then draw dark
 * (the N64 shows this too). This gives each see-through texel the average colour
 * of the visible ones round it, in two passes (the filters reach a texel; the
 * mipmaps further), wrapping at the edges as the tiles do. Alpha stays as it is.
 */
static void fill_clear_texels(uint8_t *rgba, uint32_t w, uint32_t h) {
    static std::vector<uint8_t> known;
    static std::vector<uint32_t> fills; /* index, then the colour */
    const uint32_t n = w * h;
    uint32_t i, clear = 0;

    known.resize(n);
    for (i = 0; i < n; i++) {
        known[i] = rgba[i * 4 + 3] != 0;
        clear += !known[i];
    }
    if (clear == 0 || clear == n) {
        return;
    }
    for (int pass = 0; pass < 2; pass++) {
        fills.clear();
        for (uint32_t y = 0; y < h; y++) {
            for (uint32_t x = 0; x < w; x++) {
                if (known[y * w + x]) {
                    continue;
                }
                uint32_t r = 0, g = 0, b = 0, c = 0;
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        const uint32_t j = ((y + h + dy) % h) * w + (x + w + dx) % w;
                        if (known[j]) {
                            r += rgba[j * 4], g += rgba[j * 4 + 1], b += rgba[j * 4 + 2], c++;
                        }
                    }
                }
                if (c != 0) {
                    fills.push_back(y * w + x);
                    fills.push_back((r / c) | (g / c) << 8 | (b / c) << 16);
                }
            }
        }
        for (i = 0; i < fills.size(); i += 2) {
            uint8_t *p = rgba + fills[i] * 4;
            p[0] = (uint8_t)fills[i + 1], p[1] = (uint8_t)(fills[i + 1] >> 8), p[2] = (uint8_t)(fills[i + 1] >> 16);
            known[fills[i]] = 1;
        }
    }
}

void gfx_texture_cache_clear(void) {
    gfx_flush();
    for (auto &e : texture_cache) {
        gfx_rapi->delete_texture(e.second.texture_id);
    }
    texture_cache.clear();
    /* nothing bound (a recycled name must not pass for the texture bound) */
    rendering_state.textures[0] = rendering_state.textures[1] = UINT32_MAX;
}

/* ---- Matrices and vertices ---- */

static void gfx_matrix_mul(float res[4][4], const float a[4][4], const float b[4][4]) {
    float tmp[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            tmp[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j] + a[i][3] * b[3][j];
        }
    }
    memcpy(res, tmp, sizeof(tmp));
}

static void read_mtx(float out[4][4], const uint8_t *addr) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j += 2) {
            uint32_t ints = rd32(addr + i * 8 + j * 2);
            uint32_t fracs = rd32(addr + 32 + i * 8 + j * 2);
            int32_t a = (int32_t)((ints & 0xFFFF0000) | (fracs >> 16));
            int32_t b = (int32_t)((ints << 16) | (fracs & 0xFFFF));
            out[i][j] = a / 65536.0f;
            out[i][j + 1] = b / 65536.0f;
        }
    }
}

static void calculate_normal_dir(const int8_t dir[3], const float mv[4][4], float coeffs[3]) {
    float x = dir[0] / 127.0f, y = dir[1] / 127.0f, z = dir[2] / 127.0f;
    /* to model space: transposed modelview */
    coeffs[0] = x * mv[0][0] + y * mv[0][1] + z * mv[0][2];
    coeffs[1] = x * mv[1][0] + y * mv[1][1] + z * mv[1][2];
    coeffs[2] = x * mv[2][0] + y * mv[2][1] + z * mv[2][2];
    float len = sqrtf(coeffs[0] * coeffs[0] + coeffs[1] * coeffs[1] + coeffs[2] * coeffs[2]);
    if (len > 0.0f) {
        coeffs[0] /= len;
        coeffs[1] /= len;
        coeffs[2] /= len;
    }
}

static void interp_matrix_changed(bool projection);

static void gfx_sp_matrix(uint8_t parameters, const uint8_t *addr) {
    float matrix[4][4];

    read_mtx(matrix, addr);
    if (dl_dumping) {
        sysLogPrintf(LOG_NOTE, "mtx %02X %p: %g %g %g %g | %g %g %g %g | %g %g %g %g | %g %g %g %g", parameters,
                     (const void *)addr, matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3], matrix[1][0],
                     matrix[1][1], matrix[1][2], matrix[1][3], matrix[2][0], matrix[2][1], matrix[2][2],
                     matrix[2][3], matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
    }
    if (parameters & G_MTX_PROJECTION) {
        if (parameters & G_MTX_LOAD) {
            memcpy(rsp.P_matrix, matrix, sizeof(matrix));
        } else {
            gfx_matrix_mul(rsp.P_matrix, matrix, rsp.P_matrix);
        }
    } else {
        if ((parameters & G_MTX_PUSH) && rsp.modelview_matrix_stack_size < MATRIX_STACK) {
            rsp.modelview_matrix_stack_size++;
            memcpy(rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1],
                   rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 2], sizeof(matrix));
        }
        if (parameters & G_MTX_LOAD) {
            memcpy(rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1], matrix, sizeof(matrix));
        } else {
            gfx_matrix_mul(rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1], matrix,
                           rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1]);
        }
        rsp.lights_changed = true;
    }
    gfx_matrix_mul(rsp.MP_matrix, rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1], rsp.P_matrix);
    interp_matrix_changed(parameters & G_MTX_PROJECTION);
}

static void gfx_sp_pop_matrix(uint32_t count) {
    while (count-- > 0 && rsp.modelview_matrix_stack_size > 1) {
        rsp.modelview_matrix_stack_size--;
    }
    gfx_matrix_mul(rsp.MP_matrix, rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1], rsp.P_matrix);
    rsp.lights_changed = true;
    interp_matrix_changed(false);
}

static int16_t interp_slot(const uint8_t *vtx);
static float interp_guard_band(void);

/*
 * [video] draw_distance k. The game sends the whole track every frame, and the
 * RSP does not clip at the far plane, but the sky is drawn at the far plane's
 * depth (gDPSetPrimDepth 0x7FFE): geometry past the far plane (4096 units)
 * clamps to that depth and loses to it, so the scenery ends there. With k > 1
 * the depth each vertex is drawn at is that of a far plane k times further out,
 * and the fog some tracks set (Swamp, Glacier) is that of a vertex k times
 * nearer; positions, the sky and the game's own z are as they were.
 *
 * Screen z of a perspective projection is a * w + b (the game's projection is a
 * look-at times guPerspective: column 2 is a times column 3 but for the
 * translation row), so z / w = a + b / w, -1 at the near plane w = b / (-1 - a)
 * and 1 at the far plane w = b / (1 - a).
 */
struct FarRemap {
    float fog;    /* added to z for the fog: a vertex k times nearer, b (k - 1) */
    float da, db; /* added to z for the depth: a' w + b' - (a w + b), far plane k times further */
};

static FarRemap far_remap(const float p[4][4]) {
    FarRemap f = { 0.0f, 0.0f, 0.0f };
    const float k = (float)gfx_draw_distance;
    int r = 0;

    if (gfx_draw_distance <= 1) {
        return f; /* the N64's */
    }
    for (int i = 1; i < 3; i++) {
        if (fabsf(p[i][3]) > fabsf(p[r][3])) {
            r = i;
        }
    }
    if (p[r][3] == 0.0f) {
        return f; /* not a perspective */
    }
    const float a = p[r][2] / p[r][3], b = p[3][2] - a * p[3][3];
    f.fog = b * (k - 1.0f);
    if (fabsf(1.0f - a) < 1e-7f) {
        return f;
    }
    const float wn = b / (-1.0f - a), wf = b / (1.0f - a);
    if (!(wn > 0.0f && wf > wn)) {
        return f;
    }
    const float wf2 = wf * k;
    f.da = (wf2 + wn) / (wf2 - wn) - a;
    f.db = -2.0f * wn * wf2 / (wf2 - wn) - b;
    return f;
}

static void gfx_sp_vertex(size_t n_vertices, size_t dest_index, const uint8_t *vtx) {
    const float(*mv)[4] = rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1];
    const FarRemap far = far_remap(rsp.P_matrix);

    if (rsp.lights_changed && (rsp.geometry_mode & G_LIGHTING)) {
        for (int i = 0; i < rsp.num_lights; i++) {
            calculate_normal_dir(rsp.lights[i].dir, mv, rsp.lights_coeffs[i]);
        }
        calculate_normal_dir(rsp.lookat[0], mv, rsp.lookat_coeffs[0]);
        calculate_normal_dir(rsp.lookat[1], mv, rsp.lookat_coeffs[1]);
        rsp.lights_changed = false;
    }

    for (size_t i = 0; i < n_vertices && dest_index + i < MAX_VERTICES; i++, vtx += 16) {
        struct LoadedVertex *d = &rsp.loaded_vertices[dest_index + i];
        float ob[3] = { (float)rds16(vtx), (float)rds16(vtx + 2), (float)rds16(vtx + 4) };
        int16_t tc0 = rds16(vtx + 8), tc1 = rds16(vtx + 10);
        uint8_t cn[4] = { rd8(vtx + 12), rd8(vtx + 13), rd8(vtx + 14), rd8(vtx + 15) };

        float x = ob[0] * rsp.MP_matrix[0][0] + ob[1] * rsp.MP_matrix[1][0] + ob[2] * rsp.MP_matrix[2][0] +
                  rsp.MP_matrix[3][0];
        float y = ob[0] * rsp.MP_matrix[0][1] + ob[1] * rsp.MP_matrix[1][1] + ob[2] * rsp.MP_matrix[2][1] +
                  rsp.MP_matrix[3][1];
        float z = ob[0] * rsp.MP_matrix[0][2] + ob[1] * rsp.MP_matrix[1][2] + ob[2] * rsp.MP_matrix[2][2] +
                  rsp.MP_matrix[3][2];
        float w = ob[0] * rsp.MP_matrix[0][3] + ob[1] * rsp.MP_matrix[1][3] + ob[2] * rsp.MP_matrix[2][3] +
                  rsp.MP_matrix[3][3];

        if (gfx_aspect_mode == 2 && rsp.P_matrix[2][3] != 0.0f) {
            x *= wide_k; /* perspective: widen the view (see vp_wide) */
        }
        z += vp_z_offset * w; /* screen z = z/w * scale + translate */
        d->ox = ob[0];
        d->oy = ob[1];
        d->oz = ob[2];
        d->zoff = vp_z_offset;
        d->wide = gfx_aspect_mode == 2 && rsp.P_matrix[2][3] != 0.0f;
        d->slot = interp_slot(vtx - i * 16);

        short U = tc0 * rsp.texture_scaling_factor.s >> 16;
        short V = tc1 * rsp.texture_scaling_factor.t >> 16;

        if (rsp.geometry_mode & G_LIGHTING) {
            int r = rsp.lights[rsp.num_lights].col[0];
            int g = rsp.lights[rsp.num_lights].col[1];
            int b = rsp.lights[rsp.num_lights].col[2];
            float nx = (int8_t)cn[0], ny = (int8_t)cn[1], nz = (int8_t)cn[2];

            for (int l = 0; l < rsp.num_lights; l++) {
                float intensity = nx * rsp.lights_coeffs[l][0] + ny * rsp.lights_coeffs[l][1] +
                                  nz * rsp.lights_coeffs[l][2];
                intensity /= 127.0f;
                if (intensity > 0.0f) {
                    r += intensity * rsp.lights[l].col[0];
                    g += intensity * rsp.lights[l].col[1];
                    b += intensity * rsp.lights[l].col[2];
                }
            }
            d->color.r = r > 255 ? 255 : r;
            d->color.g = g > 255 ? 255 : g;
            d->color.b = b > 255 ? 255 : b;

            if (rsp.geometry_mode & G_TEXTURE_GEN) {
                float dotx = nx * rsp.lookat_coeffs[0][0] + ny * rsp.lookat_coeffs[0][1] + nz * rsp.lookat_coeffs[0][2];
                float doty = nx * rsp.lookat_coeffs[1][0] + ny * rsp.lookat_coeffs[1][1] + nz * rsp.lookat_coeffs[1][2];
                dotx /= 127.0f;
                doty /= 127.0f;
                if (rsp.geometry_mode & G_TEXTURE_GEN_LINEAR) {
                    dotx = acosf(-dotx) / (float)M_PI - 0.5f;
                    doty = acosf(-doty) / (float)M_PI - 0.5f;
                }
                U = (int32_t)((dotx * 0.5f + 0.5f) * rsp.texture_scaling_factor.s);
                V = (int32_t)((doty * 0.5f + 0.5f) * rsp.texture_scaling_factor.t);
            }
        } else {
            d->color.r = cn[0];
            d->color.g = cn[1];
            d->color.b = cn[2];
        }
        d->u = U;
        d->v = V;

        /* trivial clip rejection (past a guard band while a frame is recorded for
           interpolation: see interp_guard_band) */
        const float gw = w * interp_guard_band();
        d->clip_rej = 0;
        if (x < -gw) d->clip_rej |= 1;
        if (x > gw) d->clip_rej |= 2;
        if (y < -gw) d->clip_rej |= 4;
        if (y > gw) d->clip_rej |= 8;
        if (z < -w) d->clip_rej |= 16;

        d->x = x;
        d->y = y;
        d->z = z + far.da * w + far.db; /* the depth drawn at ([video] draw_distance) */
        d->w = w;
        if (dl_dumping && dl_dumped_vtx < 3000) {
            dl_dumped_vtx++;
            sysLogPrintf(LOG_NOTE, "vtx %p: ob %g %g %g col %02X%02X%02X%02X -> clip %g %g %g %g rej %X", (const void *)vtx, ob[0], ob[1],
                         ob[2], cn[0], cn[1], cn[2], cn[3], x, y, z, w, d->clip_rej);
        }

        if (rsp.geometry_mode & G_FOG) {
            float winv = fabsf(w) < 0.001f ? 1000.0f : 1.0f / w;
            if (winv < 0.0f) {
                winv = 32767.0f;
            }
            float fog_z = (z + far.fog) * winv * rsp.fog_mul + rsp.fog_offset;
            if (fog_z < 0) fog_z = 0;
            if (fog_z > 255) fog_z = 255;
            d->color.a = (uint8_t)fog_z; /* F3DEX2 puts fog in shade alpha */
        } else {
            d->color.a = cn[3];
        }
    }
}

static void gfx_sp_modify_vertex(uint16_t vtx_idx, uint8_t where, uint32_t val) {
    struct LoadedVertex *v = &rsp.loaded_vertices[vtx_idx % MAX_VERTICES];

    switch (where) {
        case G_MWO_POINT_RGBA:
            v->color.r = val >> 24;
            v->color.g = val >> 16;
            v->color.b = val >> 8;
            v->color.a = val;
            break;
        case G_MWO_POINT_ST:
            v->u = (int16_t)(val >> 16);
            v->v = (int16_t)val;
            break;
        default:
            break;
    }
}

/* ---- Viewport, scissor and render state ---- */

static void gfx_adjust_rect(struct XYWidthHeight *area) {
    /* N64 pixels (top-left origin) to game framebuffer pixels (bottom-left origin) */
    float sx = (float)fb_width / NATIVE_WIDTH, sy = (float)fb_height / NATIVE_HEIGHT;
    float x0 = 0.0f;
    if (gfx_aspect_mode == 2 && !vp_wide) {
        sx = sy; /* the 4:3 area in the middle */
        x0 = (fb_width - NATIVE_WIDTH * sx) / 2.0f;
    }
    float x = x0 + area->x * sx, w = area->width * sx;
    float y = (NATIVE_HEIGHT - area->y - (int)area->height) * sy, h = area->height * sy;
    area->x = (int16_t)x;
    area->y = (int16_t)y;
    area->width = (uint32_t)(w + 0.5f);
    area->height = (uint32_t)(h + 0.5f);
}

static void gfx_calc_and_set_viewport(const uint8_t *vp) {
    float width = 2.0f * rds16(vp + 0) / 4.0f;
    float height = 2.0f * rds16(vp + 2) / 4.0f;
    float x = rds16(vp + 8) / 4.0f - width / 2.0f;
    float y = rds16(vp + 10) / 4.0f - height / 2.0f;

    rdp.viewport.x = (int16_t)x;
    rdp.viewport.y = (int16_t)y;
    rdp.viewport.width = (uint32_t)fabsf(width);
    rdp.viewport.height = (uint32_t)fabsf(height);
    rdp.viewport_or_scissor_changed = true;
    int16_t zscale = rds16(vp + 4), ztrans = rds16(vp + 12);
    vp_z_offset = zscale != 0 ? (float)ztrans / zscale : 0.0f;
    vp_z_scale = zscale != 0 ? (float)zscale : 1023.0f;
}

static void gfx_dp_set_scissor(uint32_t mode, uint32_t ulx, uint32_t uly, uint32_t lrx, uint32_t lry) {
    rdp.scissor.x = ulx / 4;
    rdp.scissor.y = uly / 4;
    rdp.scissor.width = lrx > ulx ? (lrx - ulx) / 4 : 0;
    rdp.scissor.height = lry > uly ? (lry - uly) / 4 : 0;
    rdp.viewport_or_scissor_changed = true;
}

static void apply_viewport_and_scissor(void) {
    if (!rdp.viewport_or_scissor_changed) {
        return;
    }
    struct XYWidthHeight vp = rdp.viewport, sc = rdp.scissor;
    gfx_adjust_rect(&vp);
    gfx_adjust_rect(&sc);
    if (memcmp(&vp, &rendering_state.viewport, sizeof(vp)) != 0) {
        FLUSH("viewport");
        gfx_rapi->set_viewport(vp.x, vp.y, vp.width, vp.height);
        rendering_state.viewport = vp;
    }
    if (memcmp(&sc, &rendering_state.scissor, sizeof(sc)) != 0) {
        FLUSH("scissor");
        gfx_rapi->set_scissor(sc.x, sc.y, sc.width, sc.height);
        rendering_state.scissor = sc;
    }
    rdp.viewport_or_scissor_changed = false;
}

/* A texture rectangle drawn at one texel a pixel: the RDP samples it exactly on
   texels, so bilinear filtering changes nothing at 320x240, but upscaled it would
   blend in the neighbouring texels (the next glyph of a font, say). */
static bool rect_unscaled;

/* Depth, blending and the combiner for the next draw. Returns the combiner. */
static struct ColorCombiner *prepare_draw(bool use_fog_ok, bool *use_alpha_out, bool *use_fog_out,
                                          bool *texture_edge_out) {
    uint32_t cycle_type = rdp.other_mode_h & (3U << G_MDSFT_CYCLETYPE);
    bool depth_test = (rsp.geometry_mode & G_ZBUFFER) != 0 && cycle_type != G_CYC_COPY && cycle_type != G_CYC_FILL;
    bool depth_compare = (rdp.other_mode_l & Z_CMP) != 0;
    bool depth_update = (rdp.other_mode_l & Z_UPD) != 0;
    bool depth_source_prim = (rdp.other_mode_l & G_ZS_PRIM) != 0;
    uint16_t zmode = rdp.other_mode_l & ZMODE_DEC;

    if (!depth_compare && !depth_update) {
        depth_test = false;
    }
    if (depth_test != rendering_state.depth_test || depth_update != rendering_state.depth_update ||
        depth_compare != rendering_state.depth_compare || depth_source_prim != rendering_state.depth_source_prim ||
        zmode != rendering_state.zmode) {
        FLUSH("depth");
        gfx_rapi->set_depth_mode(depth_test, depth_update, depth_compare, depth_source_prim, zmode);
        rendering_state.depth_test = depth_test;
        rendering_state.depth_update = depth_update;
        rendering_state.depth_compare = depth_compare;
        rendering_state.depth_source_prim = depth_source_prim;
        rendering_state.zmode = zmode;
    }

    bool use_alpha = (rdp.other_mode_l & (G_BL_A_MEM << 18)) == 0;
    bool use_fog = use_fog_ok && (rdp.other_mode_l >> 30) == G_BL_CLR_FOG;
    bool texture_edge = (rdp.other_mode_l & CVG_X_ALPHA) == CVG_X_ALPHA;
    bool use_noise = (rdp.other_mode_l & G_AC_DITHER) == G_AC_DITHER;
    bool alpha_threshold = (rdp.other_mode_l & (3 << G_MDSFT_ALPHACOMPARE)) == G_AC_THRESHOLD;

    if (cycle_type == G_CYC_COPY) {
        use_alpha = (rdp.other_mode_l & (3 << G_MDSFT_ALPHACOMPARE)) == G_AC_THRESHOLD;
        alpha_threshold = use_alpha;
        use_fog = false;
        texture_edge = false;
    } else if ((rdp.other_mode_l & (ALPHA_CVG_SEL | CVG_X_ALPHA)) == ALPHA_CVG_SEL && cycle_type != G_CYC_FILL) {
        /* ALPHA_CVG_SEL without CVG_X_ALPHA: the RDP compares the pixel's coverage, not the
         * combined alpha, so the alpha compare passes everywhere inside a primitive. The menu
         * backgrounds are RGBA16 images whose alpha bits are all 0, drawn with G_AC_THRESHOLD
         * in this mode; a texel alpha test would discard them whole. */
        alpha_threshold = false;
        use_noise = false;
    } else if (cycle_type == G_CYC_FILL) {
        /* fill mode writes the fill colour straight to memory: no blending */
        use_alpha = false;
        alpha_threshold = false;
        use_noise = false;
        use_fog = false;
        texture_edge = false;
    }
    if (texture_edge || alpha_threshold) {
        use_alpha = true;
    }
    if (use_alpha != rendering_state.alpha_blend) {
        FLUSH("blend");
        gfx_rapi->set_use_alpha(use_alpha, false);
        rendering_state.alpha_blend = use_alpha;
    }

    uint64_t options = 0;
    if (use_alpha) options |= SHADER_OPT_ALPHA;
    if (use_fog) options |= SHADER_OPT_FOG;
    if (texture_edge) options |= SHADER_OPT_TEXTURE_EDGE;
    if (use_noise) options |= SHADER_OPT_NOISE;
    if (alpha_threshold) options |= SHADER_OPT_ALPHA_THRESHOLD;
    if (cycle_type == G_CYC_2CYCLE) options |= SHADER_OPT_2CYC;
    if (texture_filter_mode == FILTER_THREE_POINT) options |= SHADER_OPT_THREE_POINT;

    uint64_t combine_mode = rdp.combine_mode;
    if (cycle_type == G_CYC_COPY) {
        /* copy mode: texel0 straight through */
        combine_mode = (uint64_t)(G_CCMUX_TEXEL0 << 13) | ((uint64_t)G_ACMUX_TEXEL0 << 25);
        options &= ~(uint64_t)SHADER_OPT_2CYC;
    } else if (cycle_type == G_CYC_FILL) {
        combine_mode = (uint64_t)(G_CCMUX_SHADE << 13) | ((uint64_t)G_ACMUX_SHADE << 25);
        options &= ~(uint64_t)SHADER_OPT_2CYC;
    }

    ColorCombinerKey key = { combine_mode, options };
    struct ColorCombiner *comb = gfx_lookup_or_create_color_combiner(key);
    struct ShaderProgram *prg = comb->prg;
    if (prg == NULL) {
        comb->prg = prg = gfx_lookup_or_create_shader_program(comb->shader_id0, comb->shader_id1);
    }
    if (prg != rendering_state.shader_program) {
        FLUSH("shader");
        gfx_rapi->unload_shader(rendering_state.shader_program);
        gfx_rapi->load_shader(prg);
        rendering_state.shader_program = prg;
    }

    for (int i = 0; i < 2; i++) {
        if (!comb->used_textures[i]) {
            continue;
        }
        int tile = (rsp.texture_tile + i) & 7;
        if (s2dex || cycle_type == G_CYC_COPY) {
            tile = i;
        }
        if (rdp.textures_changed[i]) {
            import_texture(i, tile); /* flushes unless it is the texture bound */
            rdp.textures_changed[i] = false;
        }
        bool linear = (rdp.other_mode_h & (3U << G_MDSFT_TEXTFILT)) != G_TF_POINT && cycle_type != G_CYC_COPY &&
                      !rect_unscaled;
        uint8_t cms = rdp.texture_tile[tile].cms, cmt = rdp.texture_tile[tile].cmt;
        if (linear != rendering_state.linear[i] || cms != rendering_state.cms[i] || cmt != rendering_state.cmt[i]) {
            FLUSH("sampler");
            gfx_rapi->set_sampler_parameters(i, linear, cms, cmt);
            rendering_state.linear[i] = linear;
            rendering_state.cms[i] = cms;
            rendering_state.cmt[i] = cmt;
        }
    }
    *use_alpha_out = use_alpha;
    *use_fog_out = use_fog;
    *texture_edge_out = texture_edge;
    return comb;
}

static int tile_for(int i) {
    return s2dex ? i : (rsp.texture_tile + i) & 7;
}

static void texture_size(int i, float *w, float *h) {
    int tile = tile_for(i);

    /* the tile's own size (SetTileSize), as the RDP wraps on it */
    uint32_t lrs = rdp.texture_tile[tile].lrs, uls = rdp.texture_tile[tile].uls;
    uint32_t lrt = rdp.texture_tile[tile].lrt, ult = rdp.texture_tile[tile].ult;
    float tw = (lrs - uls + 4) / 4.0f, th = (lrt - ult + 4) / 4.0f;
    uint32_t line = rdp.texture_tile[tile].line_size_bytes;
    switch (rdp.texture_tile[tile].siz) {
        case G_IM_SIZ_4b: line *= 2; break;
        case G_IM_SIZ_8b: break;
        case G_IM_SIZ_16b: line /= 2; break;
        case G_IM_SIZ_32b: line /= 2; break;
    }
    if (line > 0 && (tw <= 0.0f || tw > line)) {
        tw = (float)line;
    }
    if (th <= 0.0f) {
        th = 1.0f;
    }
    /* A clamped (or unmasked) axis does not wrap at the tile size: its texels are
       the ones uploaded, which can be more than the tile shows (the font loads
       10 texels a row for a 9-texel tile). Only wrapping axes repeat every tile. */
    if ((rdp.texture_tile[tile].cms & G_TX_CLAMP) && bound_w[i] != 0) {
        tw = (float)bound_w[i];
    }
    if ((rdp.texture_tile[tile].cmt & G_TX_CLAMP) && bound_h[i] != 0) {
        th = (float)bound_h[i];
    }
    *w = tw;
    *h = th;
}

/* Rectangles compute their texel coordinates themselves (gfx_draw_rectangle) */
static bool emitting_rect;

static float tile_shift(float c, uint8_t shift) {
    if (shift == 0) {
        return c;
    }
    if (shift <= 10) {
        return c / (float)(1 << shift);
    }
    return c * (float)(1 << (16 - shift));
}

/* Append one vertex in the layout the shader expects */
static void interp_note_vertex(const struct LoadedVertex *v);

static void emit_vertex(const struct LoadedVertex *v, struct ColorCombiner *comb, bool use_alpha, bool use_fog,
                        float tex_w[2], float tex_h[2], bool linear) {
    interp_note_vertex(v);
    buf_vbo[buf_vbo_len++] = v->x;
    buf_vbo[buf_vbo_len++] = v->y;
    buf_vbo[buf_vbo_len++] = std::isnan(emit_zprim) ? v->z : emit_zprim * v->w;
    buf_vbo[buf_vbo_len++] = v->w;

    for (int t = 0; t < 2; t++) {
        if (!comb->used_textures[t]) {
            continue;
        }
        int tile = tile_for(t);
        float su = v->u, sv = v->v;
        if (!emitting_rect) {
            /* the tile's shift, as the RDP applies it before the tile origin:
               1-10 divide by 2^shift, 11-15 multiply by 2^(16-shift) */
            su = tile_shift(su, rdp.texture_tile[tile].shifts);
            sv = tile_shift(sv, rdp.texture_tile[tile].shiftt);
        }
        float u = (su - rdp.texture_tile[tile].uls * 8) / 32.0f;
        float vv = (sv - rdp.texture_tile[tile].ult * 8) / 32.0f;
        if (linear) {
            u += 0.5f;
            vv += 0.5f;
        }
        buf_vbo[buf_vbo_len++] = u / tex_w[t];
        buf_vbo[buf_vbo_len++] = vv / tex_h[t];
    }
    if (use_fog) {
        buf_vbo[buf_vbo_len++] = rdp.fog_color.r / 255.0f;
        buf_vbo[buf_vbo_len++] = rdp.fog_color.g / 255.0f;
        buf_vbo[buf_vbo_len++] = rdp.fog_color.b / 255.0f;
        buf_vbo[buf_vbo_len++] = v->color.a / 255.0f;
    }
    int num_inputs = 0;
    for (int i = 0; i < 7; i++) {
        if (comb->shader_input_mapping[0][i] || comb->shader_input_mapping[1][i]) {
            num_inputs = i + 1;
        }
    }
    for (int j = 0; j < num_inputs; j++) {
        struct RGBA color = { 0, 0, 0, 0 };
        for (int k = 0; k < 1 + (use_alpha ? 1 : 0); k++) {
            struct RGBA tmp;
            switch (comb->shader_input_mapping[k][j]) {
                case G_CCMUX_PRIMITIVE: tmp = rdp.prim_color; break;
                case G_CCMUX_SHADE: tmp = v->color; break;
                case G_CCMUX_ENVIRONMENT: tmp = rdp.env_color; break;
                case G_CCMUX_PRIMITIVE_ALPHA:
                    tmp = { rdp.prim_color.a, rdp.prim_color.a, rdp.prim_color.a, rdp.prim_color.a };
                    break;
                case G_CCMUX_ENV_ALPHA:
                    tmp = { rdp.env_color.a, rdp.env_color.a, rdp.env_color.a, rdp.env_color.a };
                    break;
                case G_CCMUX_SHADE_ALPHA:
                    tmp = { v->color.a, v->color.a, v->color.a, v->color.a };
                    break;
                case G_CCMUX_PRIM_LOD_FRAC:
                    tmp = { rdp.prim_lod_fraction, rdp.prim_lod_fraction, rdp.prim_lod_fraction,
                            rdp.prim_lod_fraction };
                    break;
                case G_CCMUX_LOD_FRACTION:
                    tmp = { 255, 255, 255, 255 };
                    break;
                default:
                    tmp = { 0, 0, 0, 0 };
                    break;
            }
            if (k == 0) {
                color.r = tmp.r;
                color.g = tmp.g;
                color.b = tmp.b;
            } else {
                /* with fog, shade alpha is the fog factor: use full alpha for the input */
                color.a = (use_fog && comb->shader_input_mapping[k][j] == G_CCMUX_SHADE) ? 255 : tmp.a;
            }
        }
        buf_vbo[buf_vbo_len++] = color.r / 255.0f;
        buf_vbo[buf_vbo_len++] = color.g / 255.0f;
        buf_vbo[buf_vbo_len++] = color.b / 255.0f;
        if (use_alpha) {
            buf_vbo[buf_vbo_len++] = color.a / 255.0f;
        }
    }
}

static void gfx_sp_tri1(uint8_t vtx1_idx, uint8_t vtx2_idx, uint8_t vtx3_idx) {
    struct LoadedVertex *v1 = &rsp.loaded_vertices[vtx1_idx % MAX_VERTICES];
    struct LoadedVertex *v2 = &rsp.loaded_vertices[vtx2_idx % MAX_VERTICES];
    struct LoadedVertex *v3 = &rsp.loaded_vertices[vtx3_idx % MAX_VERTICES];
    struct LoadedVertex *v_arr[3] = { v1, v2, v3 };

    stats.tris++;
    if (v1->clip_rej & v2->clip_rej & v3->clip_rej) {
        return;
    }
    if (rsp.geometry_mode & G_CULL_BOTH) {
        float dx1 = v1->x / v1->w - v2->x / v2->w;
        float dy1 = v1->y / v1->w - v2->y / v2->w;
        float dx2 = v3->x / v3->w - v2->x / v2->w;
        float dy2 = v3->y / v3->w - v2->y / v2->w;
        float cross = dx1 * dy2 - dy1 * dx2;
        if ((v1->w < 0) ^ (v2->w < 0) ^ (v3->w < 0)) {
            cross = -cross;
        }
        switch (rsp.geometry_mode & G_CULL_BOTH) {
            case G_CULL_FRONT:
                if (cross <= 0) return;
                break;
            case G_CULL_BACK:
                if (cross >= 0) return;
                break;
            case G_CULL_BOTH:
                return;
        }
    }

    bool wide = gfx_aspect_mode == 2 && rsp.P_matrix[2][3] != 0.0f;
    if (wide != vp_wide) {
        vp_wide = wide;
        rdp.viewport_or_scissor_changed = true;
    }
    apply_viewport_and_scissor();
    bool use_alpha, use_fog, texture_edge;
    struct ColorCombiner *comb = prepare_draw(true, &use_alpha, &use_fog, &texture_edge);
    float tex_w[2] = { 1, 1 }, tex_h[2] = { 1, 1 };
    for (int i = 0; i < 2; i++) {
        if (comb->used_textures[i]) {
            texture_size(i, &tex_w[i], &tex_h[i]);
        }
    }
    bool linear = (rdp.other_mode_h & (3U << G_MDSFT_TEXTFILT)) != G_TF_POINT;
    /* G_ZS_PRIM: the RDP tests and writes the primitive depth (gDPSetPrimDepth), not
       the triangle's own. A screen z (viewport units, z/w * scale + translate) is
       the depth buffer's value / 32. Terminal Impact's backdrop behind the start line
       is drawn so at the far plane (0x7FFE); with its own depth, the part near the
       camera came out in front of the road. */
    emit_zprim = (rdp.other_mode_l & G_ZS_PRIM) ? std::min(rdp.prim_depth / (32.0f * vp_z_scale), 1.0f) : NAN;
    for (int i = 0; i < 3; i++) {
        emit_vertex(v_arr[i], comb, use_alpha, use_fog, tex_w, tex_h, linear);
    }
    emit_zprim = NAN;
    if (++buf_vbo_num_tris == MAX_BUFFERED) {
        FLUSH("full");
    }
}

/* ---- Moves, modes and RDP state ---- */

static void gfx_sp_movemem(uint8_t index, uint16_t offset, const uint8_t *data) {
    switch (index) {
        case G_MV_VIEWPORT:
            gfx_calc_and_set_viewport(data);
            break;
        case G_MV_LIGHT: {
            int lightidx = offset / 24 - 2;
            if (lightidx >= 0 && lightidx <= MAX_LIGHTS) {
                for (int i = 0; i < 3; i++) {
                    rsp.lights[lightidx].col[i] = rd8(data + i);
                    rsp.lights[lightidx].dir[i] = (int8_t)rd8(data + 8 + i);
                }
            } else if (lightidx < 0) {
                /* lookat x (offset 0) and y (offset 24) */
                int which = offset / 24;
                for (int i = 0; i < 3; i++) {
                    rsp.lookat[which & 1][i] = (int8_t)rd8(data + 8 + i);
                }
            }
            rsp.lights_changed = true;
            break;
        }
        default:
            break;
    }
}

static void gfx_sp_moveword(uint8_t index, uint16_t offset, uint32_t data) {
    switch (index) {
        case G_MW_NUMLIGHT:
            rsp.num_lights = data / 24;
            if (rsp.num_lights > MAX_LIGHTS) {
                rsp.num_lights = MAX_LIGHTS;
            }
            rsp.lights_changed = true;
            break;
        case G_MW_FOG:
            rsp.fog_mul = (int16_t)(data >> 16);
            rsp.fog_offset = (int16_t)data;
            break;
        case G_MW_SEGMENT: {
            int seg = (offset / 4) & 0xF;
            if (data < 0x10000000u) {
                segment_base[seg] = data == 0 ? 0 : (uintptr_t)n64_to_host(0x80000000u | data);
            } else if (data >= 0x80000000u) {
                segment_base[seg] = (uintptr_t)n64_to_host(data);
            } else {
                segment_base[seg] = data;
            }
            break;
        }
        case G_MW_LIGHTCOL: {
            int light = offset / 24;
            if (light < MAX_LIGHTS + 1 && (offset % 24) < 4) {
                rsp.lights[light].col[0] = data >> 24;
                rsp.lights[light].col[1] = data >> 16;
                rsp.lights[light].col[2] = data >> 8;
            }
            break;
        }
        default:
            break;
    }
}

static void gfx_sp_texture(uint16_t sc, uint16_t tc, uint8_t level, uint8_t tile, uint8_t on) {
    rsp.texture_scaling_factor.s = sc;
    rsp.texture_scaling_factor.t = tc;
    if (rsp.texture_tile != tile) {
        rdp.textures_changed[0] = rdp.textures_changed[1] = true;
    }
    rsp.texture_tile = tile;
}

static void gfx_dp_set_other_mode_h(uint32_t sft, uint32_t len, uint32_t data) {
    uint32_t mask = ((1U << len) - 1) << sft;
    uint32_t old = rdp.other_mode_h;
    rdp.other_mode_h = (rdp.other_mode_h & ~mask) | data;
    if ((old ^ rdp.other_mode_h) & (3U << G_MDSFT_TEXTLUT)) {
        rdp.textures_changed[0] = rdp.textures_changed[1] = true;
    }
}

static void gfx_dp_set_other_mode_l(uint32_t sft, uint32_t len, uint32_t data) {
    uint32_t mask = ((1U << len) - 1) << sft;
    rdp.other_mode_l = (rdp.other_mode_l & ~mask) | data;
}

static void gfx_dp_set_texture_image(uint32_t format, uint32_t size, uint32_t width, const uint8_t *addr) {
    rdp.texture_to_load.addr = addr;
    rdp.texture_to_load.fmt = format;
    rdp.texture_to_load.siz = size;
    rdp.texture_to_load.width = width;
}

static void gfx_dp_set_tile(uint8_t fmt, uint32_t siz, uint32_t line, uint32_t tmem, uint8_t tile, uint32_t palette,
                            uint32_t cmt, uint32_t maskt, uint32_t shiftt, uint32_t cms, uint32_t masks,
                            uint32_t shifts) {
    if (cms == G_TX_WRAP && masks == G_TX_NOMASK) {
        cms = G_TX_CLAMP;
    }
    if (cmt == G_TX_WRAP && maskt == G_TX_NOMASK) {
        cmt = G_TX_CLAMP;
    }
    rdp.texture_tile[tile].fmt = fmt;
    rdp.texture_tile[tile].siz = siz;
    rdp.texture_tile[tile].cms = cms;
    rdp.texture_tile[tile].cmt = cmt;
    rdp.texture_tile[tile].shifts = shifts;
    rdp.texture_tile[tile].shiftt = shiftt;
    rdp.texture_tile[tile].line_size_bytes = line * 8;
    rdp.texture_tile[tile].tmem = tmem;
    rdp.texture_tile[tile].palette = palette;
    rdp.textures_changed[0] = rdp.textures_changed[1] = true;
}

static void gfx_dp_set_tile_size(uint8_t tile, uint16_t uls, uint16_t ult, uint16_t lrs, uint16_t lrt) {
    rdp.texture_tile[tile].uls = uls;
    rdp.texture_tile[tile].ult = ult;
    rdp.texture_tile[tile].lrs = lrs;
    rdp.texture_tile[tile].lrt = lrt;
    rdp.textures_changed[0] = rdp.textures_changed[1] = true;
}

static void gfx_dp_load_tlut(uint8_t tile, uint32_t high_index) {
    uint32_t tmem = rdp.texture_tile[tile].tmem;
    uint32_t start = tmem >= 256 ? (tmem - 256) : 0;
    uint32_t count = high_index + 1;
    const uint8_t *addr = rdp.texture_to_load.addr;

    if (addr == NULL) {
        return;
    }
    for (uint32_t i = 0; i < count && start + i < 256; i++) {
        rdp.palette[start + i] = tex16(addr, i * 2);
    }
    uint32_t h = 2166136261u;
    for (int i = 0; i < 256; i++) {
        h = (h ^ rdp.palette[i]) * 16777619u;
    }
    rdp.palette_sum = h;
    rdp.textures_changed[0] = rdp.textures_changed[1] = true;
}

static void gfx_dp_load_block(uint8_t tile, uint32_t uls, uint32_t ult, uint32_t lrs, uint32_t dxt) {
    uint32_t word_size_shift = 0;

    switch (rdp.texture_to_load.siz) {
        case G_IM_SIZ_4b: word_size_shift = 0; break; /* loaded as 8b/16b */
        case G_IM_SIZ_8b: word_size_shift = 0; break;
        case G_IM_SIZ_16b: word_size_shift = 1; break;
        case G_IM_SIZ_32b: word_size_shift = 2; break;
    }
    uint32_t size_bytes = (lrs + 1) << word_size_shift;
    uint32_t tmem = rdp.texture_tile[tile].tmem;
    rdp.loaded_texture[tmem].addr = rdp.texture_to_load.addr;
    rdp.loaded_texture[tmem].size_bytes = size_bytes;
    rdp.loaded_texture[tmem].full_line_size_bytes = 0;
    rdp.loaded_texture[tmem].line_size_bytes = 0;
    rdp.textures_changed[0] = rdp.textures_changed[1] = true;
}

static void gfx_dp_load_tile(uint8_t tile, uint32_t uls, uint32_t ult, uint32_t lrs, uint32_t lrt) {
    uint32_t word_size_shift = 0;

    switch (rdp.texture_to_load.siz) {
        case G_IM_SIZ_4b: word_size_shift = 0; break;
        case G_IM_SIZ_8b: word_size_shift = 0; break;
        case G_IM_SIZ_16b: word_size_shift = 1; break;
        case G_IM_SIZ_32b: word_size_shift = 2; break;
    }
    uint32_t offset_x = uls >> G_TEXTURE_IMAGE_FRAC;
    uint32_t offset_y = ult >> G_TEXTURE_IMAGE_FRAC;
    uint32_t tile_width = ((lrs - uls) >> G_TEXTURE_IMAGE_FRAC) + 1;
    uint32_t tile_height = ((lrt - ult) >> G_TEXTURE_IMAGE_FRAC) + 1;
    uint32_t full_line = rdp.texture_to_load.width << word_size_shift;
    uint32_t line = tile_width << word_size_shift;
    uint32_t start = offset_y * full_line + (offset_x << word_size_shift);
    if (rdp.texture_to_load.siz == G_IM_SIZ_4b) {
        full_line = (rdp.texture_to_load.width + 1) / 2;
        line = (tile_width + 1) / 2;
        start = offset_y * full_line + offset_x / 2;
    }
    uint32_t tmem = rdp.texture_tile[tile].tmem;
    rdp.loaded_texture[tmem].addr = rdp.texture_to_load.addr + start;
    rdp.loaded_texture[tmem].size_bytes = line * tile_height;
    rdp.loaded_texture[tmem].full_line_size_bytes = full_line;
    rdp.loaded_texture[tmem].line_size_bytes = line;
    rdp.textures_changed[0] = rdp.textures_changed[1] = true;
}

static struct RGBA unpack_color(uint32_t c) {
    struct RGBA r = { (uint8_t)(c >> 24), (uint8_t)(c >> 16), (uint8_t)(c >> 8), (uint8_t)c };
    return r;
}

static void gfx_dp_set_fill_color(uint32_t packed) {
    uint16_t col16 = (uint16_t)packed;
    rdp.fill_color.r = SCALE_5_8(col16 >> 11);
    rdp.fill_color.g = SCALE_5_8((col16 >> 6) & 0x1F);
    rdp.fill_color.b = SCALE_5_8((col16 >> 1) & 0x1F);
    rdp.fill_color.a = (col16 & 1) * 255;
}

/* ---- Rectangles ---- */

/*
 * With MSAA, a rectangle edge inside a pixel (the screen at a scale that is not a
 * whole number, 1920x1080's 4.5) covers it in part: the pixel is drawn, blended by
 * its coverage, its texture coordinate run on past the edge. Around the glyphs of
 * the HUD and menu fonts that is a faint extra row or column, or a texel from the
 * other side of the tile. Without MSAA the pixel is drawn only if its centre is
 * inside. This moves the edges to the pixel boundaries that rule gives (in the
 * framebuffer, through the viewport rectangles are drawn with), and the texture
 * coordinates with them, so each pixel centre samples what it did: the rectangle
 * draws the pixels it draws without MSAA, whole.
 */
static void snap_rectangle(float *ulx, float *uly, float *lrx, float *lry, float *s0, float *t0, float *s1, float *t1,
                           bool flip) {
    const struct XYWidthHeight &vp = rendering_state.viewport;
    const float sx = vp.width / (float)NATIVE_WIDTH, sy = vp.height / (float)NATIVE_HEIGHT;

    if (sx <= 0.0f || sy <= 0.0f || *lrx <= *ulx || *lry <= *uly) {
        return;
    }
    /* N64 x to framebuffer x and back; y counts up from the bottom there */
    auto snap_x = [&](float x) { return (ceilf(vp.x + x * sx - 0.5f) - vp.x) / sx; };
    auto snap_y = [&](float y) {
        return NATIVE_HEIGHT - (ceilf(vp.y + (NATIVE_HEIGHT - y) * sy - 0.5f) - vp.y) / sy;
    };
    const float nx0 = snap_x(*ulx), nx1 = snap_x(*lrx), ny0 = snap_y(*uly), ny1 = snap_y(*lry);
    /* s runs along x and t along y (flipped: the other way round), linearly */
    const float fx0 = (nx0 - *ulx) / (*lrx - *ulx), fx1 = (nx1 - *ulx) / (*lrx - *ulx);
    const float fy0 = (ny0 - *uly) / (*lry - *uly), fy1 = (ny1 - *uly) / (*lry - *uly);
    const float ds = *s1 - *s0, dt = *t1 - *t0;
    if (!flip) {
        *s1 = *s0 + fx1 * ds, *s0 = *s0 + fx0 * ds;
        *t1 = *t0 + fy1 * dt, *t0 = *t0 + fy0 * dt;
    } else {
        *s1 = *s0 + fy1 * ds, *s0 = *s0 + fy0 * ds;
        *t1 = *t0 + fx1 * dt, *t0 = *t0 + fx0 * dt;
    }
    *ulx = nx0, *lrx = nx1, *uly = ny0, *lry = ny1;
}

/* A screen-space rectangle in N64 pixels, drawn with the current state. */
static void gfx_draw_rectangle(float ulx, float uly, float lrx, float lry, float s0, float t0, float s1, float t1,
                               bool flip, bool textured) {
    uint32_t saved_geometry_mode = rsp.geometry_mode;
    struct XYWidthHeight saved_viewport = rdp.viewport;
    /* A rectangle has no z of its own: it is depth tested only with the primitive depth
     * (G_ZS_PRIM, gDPSetPrimDepth). The menus draw their backgrounds after the 3D scene at
     * prim depth 0xFFFF, the far plane, which the RDP passes wherever nothing was drawn. */
    bool prim_z = (rdp.other_mode_l & G_ZS_PRIM) != 0;
    float z = prim_z ? std::min(rdp.prim_depth / 32767.0f, 1.0f) * 2.0f - 1.0f : -1.0f;
    rsp.geometry_mode = prim_z ? G_ZBUFFER : 0;
    rdp.viewport.x = 0;
    rdp.viewport.y = 0;
    rdp.viewport.width = NATIVE_WIDTH;
    rdp.viewport.height = NATIVE_HEIGHT;
    /* widescreen: a fill across the whole screen (a clear, a fade) covers the sides too */
    vp_wide = gfx_aspect_mode == 2 && !textured && ulx <= 0.0f && lrx >= NATIVE_WIDTH;
    rdp.viewport_or_scissor_changed = true;
    apply_viewport_and_scissor();

    if (gfx_msaa_level > 1) {
        snap_rectangle(&ulx, &uly, &lrx, &lry, &s0, &t0, &s1, &t1, flip);
    }

    bool use_alpha, use_fog, texture_edge;
    struct ColorCombiner *comb = prepare_draw(false, &use_alpha, &use_fog, &texture_edge);

    float x0 = ulx / NATIVE_WIDTH * 2.0f - 1.0f, x1 = lrx / NATIVE_WIDTH * 2.0f - 1.0f;
    float y0 = 1.0f - uly / NATIVE_HEIGHT * 2.0f, y1 = 1.0f - lry / NATIVE_HEIGHT * 2.0f;
    struct LoadedVertex v[4];
    memset(v, 0, sizeof(v));
    v[0].x = x0, v[0].y = y0; /* ul */
    v[1].x = x0, v[1].y = y1; /* ll */
    v[2].x = x1, v[2].y = y0; /* ur */
    v[3].x = x1, v[3].y = y1; /* lr */
    for (int i = 0; i < 4; i++) {
        v[i].z = z;
        v[i].w = 1.0f;
        v[i].color = rdp.fill_color;
        v[i].slot = -1;
    }
    float tex_w[2] = { 1, 1 }, tex_h[2] = { 1, 1 };
    if (textured) {
        for (int i = 0; i < 2; i++) {
            if (comb->used_textures[i]) {
                texture_size(i, &tex_w[i], &tex_h[i]);
            }
        }
        /* s/t in texels, relative to the tile like vertex coordinates (S10.5 * 32) */
        int tile = tile_for(0);
        float ou = rdp.texture_tile[tile].uls * 8, ov = rdp.texture_tile[tile].ult * 8;
        float us[4], vs[4];
        if (!flip) {
            us[0] = s0, vs[0] = t0;
            us[1] = s0, vs[1] = t1;
            us[2] = s1, vs[2] = t0;
            us[3] = s1, vs[3] = t1;
        } else {
            us[0] = s0, vs[0] = t0;
            us[1] = s1, vs[1] = t0;
            us[2] = s0, vs[2] = t1;
            us[3] = s1, vs[3] = t1;
        }
        for (int i = 0; i < 4; i++) {
            v[i].u = us[i] * 32.0f + ou;
            v[i].v = vs[i] * 32.0f + ov;
        }
    }
    bool linear = false; /* rect texture coordinates already hit texel centres */
    const struct LoadedVertex *tri[6] = { &v[0], &v[1], &v[3], &v[0], &v[3], &v[2] };
    emitting_rect = true;
    for (int i = 0; i < 6; i++) {
        emit_vertex(tri[i], comb, use_alpha, false, tex_w, tex_h, linear);
    }
    emitting_rect = false;
    buf_vbo_num_tris += 2;
    if (buf_vbo_num_tris >= MAX_BUFFERED - 2) {
        FLUSH("rect full");
    }
    rsp.geometry_mode = saved_geometry_mode;
    rdp.viewport = saved_viewport;
    rdp.viewport_or_scissor_changed = true;
}

static void gfx_dp_texture_rectangle(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry, uint8_t tile, int16_t uls,
                                     int16_t ult, int16_t dsdx, int16_t dtdy, bool flip) {
    uint32_t cycle_type = rdp.other_mode_h & (3U << G_MDSFT_CYCLETYPE);
    float x0 = ulx / 4.0f, y0 = uly / 4.0f, x1 = lrx / 4.0f, y1 = lry / 4.0f;
    float fdsdx = dsdx / 1024.0f, fdtdy = dtdy / 1024.0f;

    stats.texrects++;
    if (cycle_type == G_CYC_COPY) {
        fdsdx /= 4.0f; /* four texels a cycle */
        x1 += 1.0f;   /* inclusive */
        y1 += 1.0f;
    }
    float s0 = uls / 32.0f, t0 = ult / 32.0f;
    float w = x1 - x0, h = y1 - y0;
    float s1 = s0 + (flip ? h : w) * fdsdx;
    float t1 = t0 + (flip ? w : h) * fdtdy;
    uint8_t saved_tile = rsp.texture_tile;
    rsp.texture_tile = tile;
    if (saved_tile != tile) {
        rdp.textures_changed[0] = rdp.textures_changed[1] = true;
    }
    /* tile-relative: undo the offset emit_vertex applies */
    s0 -= rdp.texture_tile[tile].uls / 4.0f;
    s1 -= rdp.texture_tile[tile].uls / 4.0f;
    t0 -= rdp.texture_tile[tile].ult / 4.0f;
    t1 -= rdp.texture_tile[tile].ult / 4.0f;
    rect_unscaled = (dsdx == 1024 || dsdx == -1024) && (dtdy == 1024 || dtdy == -1024);
    gfx_draw_rectangle(x0, y0, x1, y1, s0, t0, s1, t1, flip, true);
    rect_unscaled = false;
    if (saved_tile != tile) {
        rsp.texture_tile = saved_tile;
        rdp.textures_changed[0] = rdp.textures_changed[1] = true;
    }
}

static void gfx_dp_fill_rectangle(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry) {
    uint32_t cycle_type = rdp.other_mode_h & (3U << G_MDSFT_CYCLETYPE);

    stats.fills++;
    if (rdp.color_image_address != NULL && rdp.color_image_address == rdp.z_buf_address) {
        /* clearing the depth buffer */
        FLUSH("fill");
        gfx_rapi->clear_framebuffer(false, true);
        return;
    }
    float x0 = ulx / 4.0f, y0 = uly / 4.0f, x1 = lrx / 4.0f, y1 = lry / 4.0f;
    if (cycle_type == G_CYC_FILL || cycle_type == G_CYC_COPY) {
        x1 += 1.0f;
        y1 += 1.0f;
    }
    if (cycle_type == G_CYC_FILL) {
        uint32_t saved_l = rdp.other_mode_l;
        rdp.other_mode_l &= ~(Z_CMP | Z_UPD);
        gfx_draw_rectangle(x0, y0, x1, y1, 0, 0, 0, 0, false, false);
        rdp.other_mode_l = saved_l;
        return;
    }
    gfx_draw_rectangle(x0, y0, x1, y1, 0, 0, 0, 0, false, false);
}

/* ---- S2DEX2 ---- */

static void s2_load_txtr(const uint8_t *txtr) {
    uint32_t type = rd32(txtr);
    const uint8_t *image = seg_addr(rd32(txtr + 4));
    uint16_t tmem = rd16(txtr + 8);

    if (dl_dumping) {
        sysLogPrintf(LOG_NOTE, "s2 txtr %08X image %p tmem %u %u %u", type, (const void *)image, tmem, rd16(txtr + 10),
                     rd16(txtr + 12));
    }
    if (type == G_OBJLT_TLUT) {
        uint16_t phead = rd16(txtr + 8), pnum = rd16(txtr + 10);
        rdp.texture_to_load.addr = image;
        rdp.texture_tile[7].tmem = phead;
        gfx_dp_load_tlut(7, pnum);
    } else if (type == G_OBJLT_TXTRBLOCK) {
        uint16_t tsize = rd16(txtr + 10);
        rdp.loaded_texture[tmem & 511].addr = image;
        rdp.loaded_texture[tmem & 511].size_bytes = (tsize + 1) * 8;
        rdp.loaded_texture[tmem & 511].full_line_size_bytes = 0;
        rdp.loaded_texture[tmem & 511].line_size_bytes = 0;
    } else { /* G_OBJLT_TXTRTILE */
        uint16_t twidth = rd16(txtr + 10), theight = rd16(txtr + 12);
        uint32_t line = (twidth + 1) / 4 * 8;        /* GS_TT_TWIDTH: 64-bit words a line * 4, minus one */
        uint32_t lines = (theight + 1) / 4;          /* GS_TT_THEIGHT: lines * 4 - 1 */
        rdp.loaded_texture[tmem & 511].addr = image;
        rdp.loaded_texture[tmem & 511].size_bytes = line * lines;
        rdp.loaded_texture[tmem & 511].full_line_size_bytes = line;
        rdp.loaded_texture[tmem & 511].line_size_bytes = line;
    }
    rdp.textures_changed[0] = rdp.textures_changed[1] = true;
}

/* Draw a uObjSprite at (x, y) in N64 pixels with the given scale. */
static void s2_draw_sprite(const uint8_t *spr, float x, float y, float scale_x, float scale_y) {
    uint16_t imageW = rd16(spr + 4), imageH = rd16(spr + 12);
    uint16_t stride = rd16(spr + 16), adrs = rd16(spr + 18);
    uint8_t fmt = rd8(spr + 20), siz = rd8(spr + 21), pal = rd8(spr + 22), flags = rd8(spr + 23);
    float w = imageW / 32.0f, h = imageH / 32.0f;

    if (dl_dumping) {
        sysLogPrintf(LOG_NOTE, "s2 sprite at %g,%g scale %g,%g: %gx%g stride %u adrs %u fmt %u siz %u pal %u flags %X",
                     x, y, scale_x, scale_y, w, h, stride, adrs, fmt, siz, pal, flags);
    }
    rdp.texture_tile[0].fmt = fmt;
    rdp.texture_tile[0].siz = siz;
    rdp.texture_tile[0].tmem = adrs & 511;
    rdp.texture_tile[0].line_size_bytes = stride * 8;
    rdp.texture_tile[0].palette = pal;
    rdp.texture_tile[0].cms = G_TX_CLAMP;
    rdp.texture_tile[0].cmt = G_TX_CLAMP;
    rdp.texture_tile[0].uls = 0;
    rdp.texture_tile[0].ult = 0;
    rdp.texture_tile[0].lrs = (uint16_t)((w - 1) * 4);
    rdp.texture_tile[0].lrt = (uint16_t)((h - 1) * 4);
    rdp.textures_changed[0] = rdp.textures_changed[1] = true;

    float s0 = 0, t0 = 0, s1 = w, t1 = h;
    if (flags & G_OBJ_FLAG_FLIPS) {
        std::swap(s0, s1);
    }
    if (flags & G_OBJ_FLAG_FLIPT) {
        std::swap(t0, t1);
    }
    gfx_draw_rectangle(x, y, x + w * scale_x, y + h * scale_y, s0, t0, s1, t1, false, true);
}

static void s2_obj_rectangle(const uint8_t *spr) {
    float x = rds16(spr + 0) / 4.0f, y = rds16(spr + 8) / 4.0f;
    float sx = 1024.0f / (rd16(spr + 2) ? rd16(spr + 2) : 1024);
    float sy = 1024.0f / (rd16(spr + 10) ? rd16(spr + 10) : 1024);

    s2_draw_sprite(spr, x, y, sx, sy);
}

static void s2_obj_rectangle_r(const uint8_t *spr) {
    float x = rds16(spr + 0) / 4.0f, y = rds16(spr + 8) / 4.0f;
    float sx = 1024.0f / (rd16(spr + 2) ? rd16(spr + 2) : 1024);
    float sy = 1024.0f / (rd16(spr + 10) ? rd16(spr + 10) : 1024);
    float bx = rsp.obj_base_x ? rsp.obj_base_x : 1.0f, by = rsp.obj_base_y ? rsp.obj_base_y : 1.0f;

    s2_draw_sprite(spr, rsp.obj_x + x / bx, rsp.obj_y + y / by, sx / bx, sy / by);
}

static void s2_obj_sprite(const uint8_t *spr) {
    /* affine sprites: drawn axis-aligned with the matrix scale */
    float x = rds16(spr + 0) / 4.0f, y = rds16(spr + 8) / 4.0f;
    float sx = 1024.0f / (rd16(spr + 2) ? rd16(spr + 2) : 1024);
    float sy = 1024.0f / (rd16(spr + 10) ? rd16(spr + 10) : 1024);

    s2_draw_sprite(spr, rsp.obj_x + x * rsp.obj_a, rsp.obj_y + y * rsp.obj_d, sx * rsp.obj_a, sy * rsp.obj_d);
}

static void s2_bg(const uint8_t *bg, bool scaled) {
    uint16_t imageX = rd16(bg + 0), imageW = rd16(bg + 2);
    int16_t frameX = rds16(bg + 4);
    uint16_t frameW = rd16(bg + 6);
    uint16_t imageY = rd16(bg + 8), imageH = rd16(bg + 10);
    int16_t frameY = rds16(bg + 12);
    uint16_t frameH = rd16(bg + 14);
    const uint8_t *image = seg_addr(rd32(bg + 16));
    uint8_t fmt = rd8(bg + 22), siz = rd8(bg + 23);
    uint16_t pal = rd16(bg + 24), flip = rd16(bg + 26);
    float scaleW = 1.0f, scaleH = 1.0f;

    if (scaled) {
        scaleW = rd16(bg + 28) / 1024.0f;
        scaleH = rd16(bg + 30) / 1024.0f;
        if (scaleW == 0) scaleW = 1;
        if (scaleH == 0) scaleH = 1;
    }
    if (dl_dumping) {
        sysLogPrintf(LOG_NOTE, "s2 bg %s: image %u,%u %ux%u frame %d,%d %ux%u fmt %u siz %u pal %u flip %u scale %g,%g ptr %p",
                     scaled ? "1cyc" : "copy", imageX / 32, imageY / 32, imageW / 4, imageH / 4, frameX / 4, frameY / 4,
                     frameW / 4, frameH / 4, fmt, siz, pal, flip, scaleW, scaleH, (const void *)image);
    }
    uint32_t w = imageW / 4, h = imageH / 4;
    uint32_t bpp_bytes2 = siz == G_IM_SIZ_4b ? 1 : siz == G_IM_SIZ_8b ? 2 : siz == G_IM_SIZ_16b ? 4 : 8;
    uint32_t line = w * bpp_bytes2 / 2;
    if (line == 0 || h == 0 || image == NULL) {
        return;
    }
    /* the whole image as one texture at TMEM 0 */
    rdp.loaded_texture[0].addr = image;
    rdp.loaded_texture[0].size_bytes = line * h;
    rdp.loaded_texture[0].full_line_size_bytes = line;
    rdp.loaded_texture[0].line_size_bytes = line;
    rdp.texture_tile[0].fmt = fmt;
    rdp.texture_tile[0].siz = siz;
    rdp.texture_tile[0].tmem = 0;
    rdp.texture_tile[0].line_size_bytes = line;
    rdp.texture_tile[0].palette = pal;
    rdp.texture_tile[0].cms = G_TX_CLAMP;
    rdp.texture_tile[0].cmt = G_TX_CLAMP;
    rdp.texture_tile[0].uls = rdp.texture_tile[0].ult = 0;
    rdp.texture_tile[0].lrs = (uint16_t)((w - 1) * 4);
    rdp.texture_tile[0].lrt = (uint16_t)((h - 1) * 4);
    rdp.textures_changed[0] = rdp.textures_changed[1] = true;

    float x0 = frameX / 4.0f, y0 = frameY / 4.0f;
    float x1 = x0 + frameW / 4.0f, y1 = y0 + frameH / 4.0f;
    float s0 = imageX / 32.0f, t0 = imageY / 32.0f;
    float s1 = s0 + (frameW / 4.0f) / scaleW, t1 = t0 + (frameH / 4.0f) / scaleH;
    if (flip & G_BG_FLAG_FLIPS) {
        std::swap(s0, s1);
    }
    gfx_draw_rectangle(x0, y0, x1, y1, s0, t0, s1, t1, false, true);
}

/* ---- The display list interpreter ---- */

static int unknown_reported[256];

static void report_unknown(uint8_t op, uint32_t w0, uint32_t w1) {
    if (!unknown_reported[op]) {
        unknown_reported[op] = 1;
        sysLogPrintf(LOG_WARNING, "gfx: unhandled %s command %02X (%08X %08X)", s2dex ? "S2DEX2" : "F3DEX2", op, w0, w1);
    }
}

static void gfx_run_dl(const uint8_t *cmd) {
    const uint8_t *stack[18];
    int depth = 0;

    for (;;) {
        if (cmd == NULL) {
            return;
        }
        uint32_t w0 = rd32(cmd), w1 = rd32(cmd + 4);
        uint8_t opcode = w0 >> 24;

        stats.cmds++;
        {
            /* DD64_DLDUMP=n: log every command of the task drawn for presented frame n
               (as DD64_SHOT counts) */
            static const char *dump = getenv("DD64_DLDUMP");
            dl_dumping = dump != NULL && (unsigned)atoi(dump) == gfx_presents + 1;
            if (dl_dumping) {
                sysLogPrintf(LOG_NOTE, "dl %p d%d %s: %08X %08X", (const void *)cmd, depth, s2dex ? "s2" : "f3", w0, w1);
            }
        }
        if (s2dex) {
            bool handled = true;
            stats.s2++;
            switch (opcode) {
                case S2_OBJ_RECTANGLE:
                    s2_obj_rectangle(seg_addr(w1));
                    break;
                case S2_OBJ_SPRITE:
                    s2_obj_sprite(seg_addr(w1));
                    break;
                case S2_OBJ_LOADTXTR:
                    s2_load_txtr(seg_addr(w1));
                    break;
                case S2_OBJ_LDTX_SPRITE: {
                    const uint8_t *p = seg_addr(w1);
                    s2_load_txtr(p);
                    s2_obj_sprite(p + 24);
                    break;
                }
                case S2_OBJ_LDTX_RECT: {
                    const uint8_t *p = seg_addr(w1);
                    s2_load_txtr(p);
                    s2_obj_rectangle(p + 24);
                    break;
                }
                case S2_OBJ_LDTX_RECT_R: {
                    const uint8_t *p = seg_addr(w1);
                    s2_load_txtr(p);
                    s2_obj_rectangle_r(p + 24);
                    break;
                }
                case S2_BG_1CYC:
                    s2_bg(seg_addr(w1), true);
                    break;
                case S2_BG_COPY:
                    s2_bg(seg_addr(w1), false);
                    break;
                case S2_OBJ_RENDERMODE:
                    break;
                case S2_OBJ_RECTANGLE_R:
                    s2_obj_rectangle_r(seg_addr(w1));
                    break;
                case S2_OBJ_MOVEMEM: {
                    const uint8_t *p = seg_addr(w1);
                    uint16_t len = (w0 >> 16) & 0xFF;
                    if (len == 23) { /* uObjMtx */
                        rsp.obj_a = (int32_t)rd32(p) / 65536.0f;
                        rsp.obj_d = (int32_t)rd32(p + 12) / 65536.0f;
                        rsp.obj_x = rds16(p + 16) / 4.0f;
                        rsp.obj_y = rds16(p + 18) / 4.0f;
                        rsp.obj_base_x = rd16(p + 20) / 1024.0f;
                        rsp.obj_base_y = rd16(p + 22) / 1024.0f;
                    } else { /* uObjSubMtx */
                        rsp.obj_x = rds16(p + 0) / 4.0f;
                        rsp.obj_y = rds16(p + 2) / 4.0f;
                        rsp.obj_base_x = rd16(p + 4) / 1024.0f;
                        rsp.obj_base_y = rd16(p + 6) / 1024.0f;
                    }
                    break;
                }
                case S2_SELECT_DL:
                    report_unknown(opcode, w0, w1);
                    break;
                default:
                    handled = false;
                    break;
            }
            if (handled) {
                cmd += 8;
                continue;
            }
        }

        switch (opcode) {
            /* ---- RSP (F3DEX2) ---- */
            case G_NOOP:
                break;
            case G_MTX:
                gfx_sp_matrix((w0 & 0xFF) ^ G_MTX_PUSH, seg_addr(w1));
                break;
            case (uint8_t)G_POPMTX:
                gfx_sp_pop_matrix(w1 / 64);
                break;
            case G_MOVEMEM:
                gfx_sp_movemem(w0 & 0xFF, ((w0 >> 8) & 0xFF) * 8, seg_addr(w1));
                break;
            case (uint8_t)G_MOVEWORD:
                gfx_sp_moveword((w0 >> 16) & 0xFF, w0 & 0xFFFF, w1);
                break;
            case (uint8_t)G_TEXTURE:
                gfx_sp_texture(w1 >> 16, w1 & 0xFFFF, (w0 >> 11) & 7, (w0 >> 8) & 7, (w0 >> 1) & 0x7F);
                break;
            case G_VTX: {
                uint32_t n = (w0 >> 12) & 0xFF;
                uint32_t end = (w0 >> 1) & 0x7F;
                gfx_sp_vertex(n, end - n, seg_addr(w1));
                break;
            }
            case G_MODIFYVTX:
                gfx_sp_modify_vertex((w0 & 0xFFFF) / 2, (w0 >> 16) & 0xFF, w1);
                break;
            case G_CULLDL:
            case G_BRANCH_Z:
                if (opcode == G_BRANCH_Z) {
                    /* draw the nearer level of detail: always branch */
                    cmd = seg_addr(rdp.rdphalf_1) - 8;
                }
                break;
            case G_TRI1:
                gfx_sp_tri1(((w0 >> 16) & 0xFF) / 2, ((w0 >> 8) & 0xFF) / 2, (w0 & 0xFF) / 2);
                break;
            case G_TRI2:
                gfx_sp_tri1(((w0 >> 16) & 0xFF) / 2, ((w0 >> 8) & 0xFF) / 2, (w0 & 0xFF) / 2);
                gfx_sp_tri1(((w1 >> 16) & 0xFF) / 2, ((w1 >> 8) & 0xFF) / 2, (w1 & 0xFF) / 2);
                break;
            case G_QUAD: /* packed like G_TRI2: (v0 v1 v2) and (v0 v2 v3) */
                gfx_sp_tri1(((w0 >> 16) & 0xFF) / 2, ((w0 >> 8) & 0xFF) / 2, (w0 & 0xFF) / 2);
                gfx_sp_tri1(((w1 >> 16) & 0xFF) / 2, ((w1 >> 8) & 0xFF) / 2, (w1 & 0xFF) / 2);
                break;
            case (uint8_t)G_GEOMETRYMODE:
                rsp.geometry_mode = (rsp.geometry_mode & (w0 & 0xFFFFFF)) | w1;
                break;
            case G_DL:
                if (((w0 >> 16) & 0xFF) == G_DL_PUSH) {
                    if (depth < 18) {
                        stack[depth++] = cmd + 8;
                    }
                }
                cmd = seg_addr(w1);
                continue;
            case (uint8_t)G_ENDDL:
                if (depth == 0) {
                    return;
                }
                cmd = stack[--depth];
                continue;
            case (uint8_t)G_SETOTHERMODE_L: {
                uint32_t len = (w0 & 0xFF) + 1;
                uint32_t sft = 32 - ((w0 >> 8) & 0xFF) - len;
                gfx_dp_set_other_mode_l(sft, len, w1);
                break;
            }
            case (uint8_t)G_SETOTHERMODE_H: {
                uint32_t len = (w0 & 0xFF) + 1;
                uint32_t sft = 32 - ((w0 >> 8) & 0xFF) - len;
                gfx_dp_set_other_mode_h(sft, len, w1);
                break;
            }
            case (uint8_t)G_RDPHALF_1:
                rdp.rdphalf_1 = w1;
                break;
            case (uint8_t)G_RDPHALF_2:
                break;
            case (uint8_t)G_LOAD_UCODE: {
                uint32_t text = w1 & 0x1FFFFFFF;
                if (w1 >= 0x10000000u && w1 < 0x80000000u) {
                    text = Port_HostToN64((const void *)(uintptr_t)w1) & 0x1FFFFFFF;
                }
                FLUSH("ucode");
                s2dex = text == UCODE_S2DEX2;
                break;
            }
            case (uint8_t)G_SPNOOP:
            case (uint8_t)G_DMA_IO:
                break;

            /* ---- RDP ---- */
            case (uint8_t)G_SETCIMG:
                FLUSH("colour image");
                rdp.color_image_address = seg_addr(w1);
                break;
            case (uint8_t)G_SETZIMG:
                rdp.z_buf_address = seg_addr(w1);
                break;
            case (uint8_t)G_SETTIMG:
                gfx_dp_set_texture_image((w0 >> 21) & 7, (w0 >> 19) & 3, (w0 & 0xFFF) + 1, seg_addr(w1));
                break;
            case (uint8_t)G_SETCOMBINE:
                {
                    /* Fast3D's packing of the mux fields */
                    uint32_t a0 = (w0 >> 20) & 0xF, c0 = (w0 >> 15) & 0x1F, Aa0 = (w0 >> 12) & 0x7,
                             Ac0 = (w0 >> 9) & 0x7, a1 = (w0 >> 5) & 0xF, c1 = w0 & 0x1F;
                    uint32_t b0 = (w1 >> 28) & 0xF, b1 = (w1 >> 24) & 0xF, Aa1 = (w1 >> 21) & 0x7,
                             Ac1 = (w1 >> 18) & 0x7, d0 = (w1 >> 15) & 0x7, Ab0 = (w1 >> 12) & 0x7,
                             Ad0 = (w1 >> 9) & 0x7, d1 = (w1 >> 6) & 0x7, Ab1 = (w1 >> 3) & 0x7, Ad1 = w1 & 0x7;
                    uint64_t c0w = a0 | b0 << 4 | c0 << 8 | d0 << 13 | Aa0 << 16 | Ab0 << 19 | Ac0 << 22 | Ad0 << 25;
                    uint64_t c1w = a1 | b1 << 4 | c1 << 8 | d1 << 13 | Aa1 << 16 | Ab1 << 19 | Ac1 << 22 | Ad1 << 25;
                    rdp.combine_mode = c0w | c1w << 28;
                }
                break;
            case (uint8_t)G_SETENVCOLOR:
                rdp.env_color = unpack_color(w1);
                break;
            case (uint8_t)G_SETPRIMCOLOR:
                rdp.prim_color = unpack_color(w1);
                rdp.prim_lod_fraction = w0 & 0xFF;
                break;
            case (uint8_t)G_SETBLENDCOLOR:
                rdp.blend_color = unpack_color(w1);
                break;
            case (uint8_t)G_SETFOGCOLOR:
                rdp.fog_color = unpack_color(w1);
                break;
            case (uint8_t)G_SETFILLCOLOR:
                gfx_dp_set_fill_color(w1);
                break;
            case (uint8_t)G_SETPRIMDEPTH:
                rdp.prim_depth = w1 >> 16;
                break;
            case (uint8_t)G_SETTILE:
                gfx_dp_set_tile((w0 >> 21) & 7, (w0 >> 19) & 3, (w0 >> 9) & 0x1FF, w0 & 0x1FF, (w1 >> 24) & 7,
                                (w1 >> 20) & 0xF, (w1 >> 18) & 3, (w1 >> 14) & 0xF, (w1 >> 10) & 0xF, (w1 >> 8) & 3,
                                (w1 >> 4) & 0xF, w1 & 0xF);
                break;
            case (uint8_t)G_SETTILESIZE:
                gfx_dp_set_tile_size((w1 >> 24) & 7, (w0 >> 12) & 0xFFF, w0 & 0xFFF, (w1 >> 12) & 0xFFF, w1 & 0xFFF);
                break;
            case (uint8_t)G_LOADBLOCK:
                gfx_dp_load_block((w1 >> 24) & 7, (w0 >> 12) & 0xFFF, w0 & 0xFFF, (w1 >> 12) & 0xFFF, w1 & 0xFFF);
                break;
            case (uint8_t)G_LOADTILE:
                gfx_dp_load_tile((w1 >> 24) & 7, (w0 >> 12) & 0xFFF, w0 & 0xFFF, (w1 >> 12) & 0xFFF, w1 & 0xFFF);
                break;
            case (uint8_t)G_LOADTLUT:
                gfx_dp_load_tlut((w1 >> 24) & 7, (w1 >> 14) & 0x3FF);
                break;
            case (uint8_t)G_RDPSETOTHERMODE:
                rdp.other_mode_h = (w0 & 0x00FFFFFF) | (rdp.other_mode_h & 0xFF000000);
                rdp.other_mode_l = w1;
                break;
            case (uint8_t)G_SETSCISSOR:
                gfx_dp_set_scissor((w1 >> 24) & 0xFF, (w0 >> 12) & 0xFFF, w0 & 0xFFF, (w1 >> 12) & 0xFFF, w1 & 0xFFF);
                break;
            case (uint8_t)G_FILLRECT:
                gfx_dp_fill_rectangle((w1 >> 12) & 0xFFF, w1 & 0xFFF, (w0 >> 12) & 0xFFF, w0 & 0xFFF);
                break;
            case (uint8_t)G_TEXRECT:
            case (uint8_t)G_TEXRECTFLIP: {
                uint32_t h1, h2;
                /* the next two commands carry s, t and the steps */
                h1 = rd32(cmd + 12);
                h2 = rd32(cmd + 20);
                gfx_dp_texture_rectangle((w1 >> 12) & 0xFFF, w1 & 0xFFF, (w0 >> 12) & 0xFFF, w0 & 0xFFF,
                                         (w1 >> 24) & 7, (int16_t)(h1 >> 16), (int16_t)h1, (int16_t)(h2 >> 16),
                                         (int16_t)h2, opcode == (uint8_t)G_TEXRECTFLIP);
                cmd += 16;
                break;
            }
            case (uint8_t)G_SETCONVERT:
            case (uint8_t)G_SETKEYR:
            case (uint8_t)G_SETKEYGB:
            case (uint8_t)G_RDPFULLSYNC:
            case (uint8_t)G_RDPTILESYNC:
            case (uint8_t)G_RDPPIPESYNC:
            case (uint8_t)G_RDPLOADSYNC:
                break;
            default:
                report_unknown(opcode, w0, w1);
                break;
        }
        cmd += 8;
    }
}


/* ---- Frame interpolation ----
 *
 * With the display faster or slower than the game's 60 Hz ([video] interpolation),
 * the frames in between are drawn from the game's last two: each game frame's
 * backend calls are recorded, with every vertex's model-space position and the
 * matrices that placed it (a slot), and replayed with each slot's matrices
 * blended towards their match in the frame before (interp_match). Slots with no
 * match, and rectangles, are drawn as recorded; camera cuts and teleports snap
 * instead of blending. Only the presentation changes: the game runs at its own
 * rate whatever the display does.
 */

static void update_framebuffer_sizes(void);
static void present_to_window(bool black);

enum InterpOp {
    IOP_LOAD_SHADER, IOP_UNLOAD_SHADER, IOP_TEXTURE, IOP_SAMPLER, IOP_DEPTH, IOP_VIEWPORT, IOP_SCISSOR,
    IOP_ALPHA, IOP_DRAW, IOP_CLEAR
};

struct InterpCmd {
    uint8_t op;
    int32_t a[5];
    struct ShaderProgram *prg;
    uint32_t vbo_off, vbo_len, tris, vtx_off;
};

struct InterpVtx {
    float ox, oy, oz, zoff;
    int16_t slot;
    uint8_t wide;
    float zprim; /* G_ZS_PRIM's depth (emit_zprim), or NAN */
};

struct InterpSlot {
    float mv[4][4], p[4][4];
    uint64_t key;  /* vertex data address and occurrence */
    uint32_t pkey; /* which projection load */
};

/* A slot's match in the frame before (interp_match): -1 for none */
static std::vector<int> interp_matches;
static const void *interp_matched_for; /* the frame interp_matches is for */

struct InterpFrame {
    std::vector<InterpCmd> cmds;
    std::vector<float> vbo;
    std::vector<InterpVtx> vtx;
    std::vector<InterpSlot> slots;
    std::unordered_map<uint32_t, int> by_pkey;
    std::unordered_map<uintptr_t, uint16_t> uses; /* recording: vertex data seen so far */
    uint32_t proj_loads;
    uint64_t t_ns;
    bool valid;
};

extern "C" {
uint32_t gfx_interp_mode; /* 0 off, 1 interpolate, 2 extrapolate (no added lag) */
}
static InterpFrame interp_store[3];
static InterpFrame *interp_prev = &interp_store[0], *interp_cur = &interp_store[1], *interp_rec = &interp_store[2];
static bool interp_recording;

/* A triangle rejected as off screen is not recorded, so the frames drawn between
   game frames could not show it as the camera turns it into view: polygons went
   missing at the edges of the screen. While recording, only triangles past half a
   screen more on each side are rejected (the GPU clips the rest). */
static float interp_guard_band(void) {
    return interp_recording ? 1.5f : 1.0f;
}
static unsigned interp_game_frames; /* recorded so far, for DD64_GFXLOG */
static int16_t interp_cur_slot = -1;
static std::vector<InterpVtx> interp_pending; /* vertices of the batch not yet drawn */
static struct GfxRenderingAPI *interp_real;     /* the backend the recorder passes calls to */
static struct GfxRenderingAPI interp_recorder;

static void interp_push(uint8_t op, int32_t a0 = 0, int32_t a1 = 0, int32_t a2 = 0, int32_t a3 = 0, int32_t a4 = 0,
                        struct ShaderProgram *prg = NULL) {
    if (!interp_recording) {
        return;
    }
    InterpCmd c = {};
    c.op = op;
    c.a[0] = a0, c.a[1] = a1, c.a[2] = a2, c.a[3] = a3, c.a[4] = a4;
    c.prg = prg;
    interp_rec->cmds.push_back(c);
}

static void interp_matrix_changed(bool projection) {
    interp_cur_slot = -1;
    if (projection && interp_recording) {
        interp_rec->proj_loads++;
    }
}

static int16_t interp_slot(const uint8_t *vtx) {
    if (!interp_recording) {
        return -1;
    }
    if (interp_cur_slot < 0) {
        InterpFrame *f = interp_rec;
        if (f->slots.size() >= 32000) {
            return -1;
        }
        InterpSlot s;
        memcpy(s.mv, rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1], sizeof(s.mv));
        memcpy(s.p, rsp.P_matrix, sizeof(s.p));
        uint16_t n = f->uses[(uintptr_t)vtx]++;
        s.key = (uint64_t)(uintptr_t)vtx << 16 | n;
        s.pkey = f->proj_loads;
        interp_cur_slot = (int16_t)f->slots.size();
        if (f->by_pkey.find(s.pkey) == f->by_pkey.end()) {
            f->by_pkey[s.pkey] = interp_cur_slot;
        }
        f->slots.push_back(s);
    }
    return interp_cur_slot;
}

static void interp_note_vertex(const struct LoadedVertex *v) {
    if (interp_recording) {
        interp_pending.push_back({ v->ox, v->oy, v->oz, v->zoff, v->slot, v->wide, emit_zprim });
    }
}

/* The recorder: each call is noted for the replay, then passed on */
static void rec_unload_shader(struct ShaderProgram *p) {
    interp_push(IOP_UNLOAD_SHADER, 0, 0, 0, 0, 0, p);
    interp_real->unload_shader(p);
}
static void rec_load_shader(struct ShaderProgram *p) {
    interp_push(IOP_LOAD_SHADER, 0, 0, 0, 0, 0, p);
    interp_real->load_shader(p);
}
static struct ShaderProgram *rec_create_and_load_new_shader(uint64_t id0, uint32_t id1) {
    struct ShaderProgram *p = interp_real->create_and_load_new_shader(id0, id1);
    interp_push(IOP_LOAD_SHADER, 0, 0, 0, 0, 0, p);
    return p;
}
static void rec_select_texture(int tile, uint32_t id, bool linear) {
    interp_push(IOP_TEXTURE, tile, (int32_t)id, linear);
    interp_real->select_texture(tile, id, linear);
}
static void rec_set_sampler_parameters(int sampler, bool linear, uint32_t cms, uint32_t cmt) {
    interp_push(IOP_SAMPLER, sampler, linear, (int32_t)cms, (int32_t)cmt);
    interp_real->set_sampler_parameters(sampler, linear, cms, cmt);
}
static void rec_set_depth_mode(bool test, bool update, bool compare, bool prim, uint16_t zmode) {
    interp_push(IOP_DEPTH, test, update, compare, prim, zmode);
    interp_real->set_depth_mode(test, update, compare, prim, zmode);
}
static void rec_set_viewport(int x, int y, int w, int h) {
    interp_push(IOP_VIEWPORT, x, y, w, h);
    interp_real->set_viewport(x, y, w, h);
}
static void rec_set_scissor(int x, int y, int w, int h) {
    interp_push(IOP_SCISSOR, x, y, w, h);
    interp_real->set_scissor(x, y, w, h);
}
static void rec_set_use_alpha(bool use_alpha, bool modulate) {
    interp_push(IOP_ALPHA, use_alpha, modulate);
    interp_real->set_use_alpha(use_alpha, modulate);
}
static void rec_draw_triangles(float vbo[], size_t len, size_t tris) {
    if (interp_recording && tris != 0 && interp_pending.size() == tris * 3) {
        InterpFrame *f = interp_rec;
        InterpCmd c = {};
        c.op = IOP_DRAW;
        c.vbo_off = (uint32_t)f->vbo.size();
        c.vbo_len = (uint32_t)len;
        c.tris = (uint32_t)tris;
        c.vtx_off = (uint32_t)f->vtx.size();
        f->vbo.insert(f->vbo.end(), vbo, vbo + len);
        f->vtx.insert(f->vtx.end(), interp_pending.begin(), interp_pending.end());
        f->cmds.push_back(c);
    }
    interp_pending.clear();
    if (!interp_recording) {
        interp_real->draw_triangles(vbo, len, tris);
    } /* else only the replay draws: every frame shown is one */
}
static void rec_clear_framebuffer(bool color, bool depth) {
    interp_push(IOP_CLEAR, color, depth);
    if (!interp_recording) {
        interp_real->clear_framebuffer(color, depth);
    }
}

/* Video, when the mode changes: recording on or off */
extern "C" void gfx_interp_set_mode(uint32_t mode) {
    gfx_interp_mode = mode;
    if (interp_real == NULL) {
        interp_real = gfx_rapi;
        interp_recorder = *gfx_rapi;
        interp_recorder.unload_shader = rec_unload_shader;
        interp_recorder.load_shader = rec_load_shader;
        interp_recorder.create_and_load_new_shader = rec_create_and_load_new_shader;
        interp_recorder.select_texture = rec_select_texture;
        interp_recorder.set_sampler_parameters = rec_set_sampler_parameters;
        interp_recorder.set_depth_mode = rec_set_depth_mode;
        interp_recorder.set_viewport = rec_set_viewport;
        interp_recorder.set_scissor = rec_set_scissor;
        interp_recorder.set_use_alpha = rec_set_use_alpha;
        interp_recorder.draw_triangles = rec_draw_triangles;
        interp_recorder.clear_framebuffer = rec_clear_framebuffer;
    }
    gfx_rapi = mode != 0 ? &interp_recorder : interp_real;
    if (mode == 0) {
        interp_prev->valid = interp_cur->valid = false;
    }
}

static void interp_begin_frame(void) {
    InterpFrame *f = interp_rec;

    interp_recording = gfx_interp_mode != 0;
    interp_cur_slot = -1;
    interp_pending.clear();
    f->cmds.clear();
    f->vbo.clear();
    f->vtx.clear();
    f->slots.clear();
    f->by_pkey.clear();
    f->uses.clear();
    f->proj_loads = 0;
    f->valid = false;
}

static void interp_end_frame(void) {
    if (!interp_recording) {
        return;
    }
    interp_recording = false;
    interp_game_frames++;
    InterpFrame *done = interp_rec;
    done->valid = true;
    interp_matched_for = NULL;
    done->t_ns = SDL_GetTicksNS();
    interp_rec = interp_prev;
    interp_prev = interp_cur;
    interp_cur = done;
}

static void mtx_lerp(float out[4][4], const float a[4][4], const float b[4][4], float t) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            out[i][j] = a[i][j] + (b[i][j] - a[i][j]) * t;
        }
    }
}

/* A modelview's rotation and size: false unless it is a rotation (no mirror)
   times one scale, within a few percent */
static bool mv_rotation(const float m[4][4], float r[3][3], float &scale) {
    float len[3];
    for (int i = 0; i < 3; i++) {
        len[i] = sqrtf(m[i][0] * m[i][0] + m[i][1] * m[i][1] + m[i][2] * m[i][2]);
    }
    scale = (len[0] + len[1] + len[2]) / 3.0f;
    if (scale < 1e-12f) {
        return false;
    }
    for (int i = 0; i < 3; i++) {
        if (fabsf(len[i] - scale) > 0.05f * scale) {
            return false;
        }
        for (int j = 0; j < 3; j++) {
            r[i][j] = m[i][j] / len[i];
        }
    }
    for (int i = 0; i < 3; i++) {
        const float *u = r[i], *v = r[(i + 1) % 3];
        if (fabsf(u[0] * v[0] + u[1] * v[1] + u[2] * v[2]) > 0.05f) {
            return false;
        }
    }
    float det = r[0][0] * (r[1][1] * r[2][2] - r[1][2] * r[2][1]) - r[0][1] * (r[1][0] * r[2][2] - r[1][2] * r[2][0]) +
                r[0][2] * (r[1][0] * r[2][1] - r[1][1] * r[2][0]);
    return det > 0.0f;
}

static void rot_to_quat(const float r[3][3], float q[4]) {
    const float tr = r[0][0] + r[1][1] + r[2][2];
    if (tr > 0.0f) {
        float k = sqrtf(tr + 1.0f) * 2.0f;
        q[0] = 0.25f * k, q[1] = (r[2][1] - r[1][2]) / k, q[2] = (r[0][2] - r[2][0]) / k, q[3] = (r[1][0] - r[0][1]) / k;
    } else if (r[0][0] > r[1][1] && r[0][0] > r[2][2]) {
        float k = sqrtf(1.0f + r[0][0] - r[1][1] - r[2][2]) * 2.0f;
        q[0] = (r[2][1] - r[1][2]) / k, q[1] = 0.25f * k, q[2] = (r[0][1] + r[1][0]) / k, q[3] = (r[0][2] + r[2][0]) / k;
    } else if (r[1][1] > r[2][2]) {
        float k = sqrtf(1.0f + r[1][1] - r[0][0] - r[2][2]) * 2.0f;
        q[0] = (r[0][2] - r[2][0]) / k, q[1] = (r[0][1] + r[1][0]) / k, q[2] = 0.25f * k, q[3] = (r[1][2] + r[2][1]) / k;
    } else {
        float k = sqrtf(1.0f + r[2][2] - r[0][0] - r[1][1]) * 2.0f;
        q[0] = (r[1][0] - r[0][1]) / k, q[1] = (r[0][2] + r[2][0]) / k, q[2] = (r[1][2] + r[2][1]) / k, q[3] = 0.25f * k;
    }
}

static void quat_to_rot(const float q[4], float r[3][3]) {
    const float w = q[0], x = q[1], y = q[2], z = q[3];
    r[0][0] = 1 - 2 * (y * y + z * z), r[0][1] = 2 * (x * y - w * z), r[0][2] = 2 * (x * z + w * y);
    r[1][0] = 2 * (x * y + w * z), r[1][1] = 1 - 2 * (x * x + z * z), r[1][2] = 2 * (y * z - w * x);
    r[2][0] = 2 * (x * z - w * y), r[2][1] = 2 * (y * z + w * x), r[2][2] = 1 - 2 * (x * x + y * y);
}

/*
 * A modelview between a and b: the rotation turned (slerp) and the size and
 * position moved in a straight line. A plain blend of the matrices shrinks a
 * model that turns far in a frame (a spinning wheel: to nothing at half a
 * turn). Matrices that are not a rotation and a size are blended plainly.
 */
static void mv_blend(float out[4][4], const float a[4][4], const float b[4][4], float t) {
    float ra[3][3], rb[3][3], sa, sb, qa[4], qb[4], q[4];

    mtx_lerp(out, a, b, t);
    if (!mv_rotation(a, ra, sa) || !mv_rotation(b, rb, sb)) {
        return;
    }
    rot_to_quat(ra, qa);
    rot_to_quat(rb, qb);
    float d = qa[0] * qb[0] + qa[1] * qb[1] + qa[2] * qb[2] + qa[3] * qb[3];
    if (d < 0.0f) {
        d = -d;
        for (int i = 0; i < 4; i++) {
            qb[i] = -qb[i];
        }
    }
    float wa = 1.0f - t, wb = t;
    if (d < 0.9995f) {
        const float th = acosf(std::min(d, 1.0f)), sn = sinf(th);
        wa = sinf((1.0f - t) * th) / sn;
        wb = sinf(t * th) / sn;
    }
    float n = 0.0f;
    for (int i = 0; i < 4; i++) {
        q[i] = qa[i] * wa + qb[i] * wb;
        n += q[i] * q[i];
    }
    n = 1.0f / sqrtf(n);
    for (int i = 0; i < 4; i++) {
        q[i] *= n;
    }
    float r[3][3];
    quat_to_rot(q, r);
    const float sc = sa + (sb - sa) * t;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            out[i][j] = r[i][j] * sc;
        }
    }
}

/* A camera cut: the two projections (with the camera in them) far apart */
static bool proj_cut(const float a[4][4], const float b[4][4]) {
    float diff = 0.0f, mag = 0.0f;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            diff = std::max(diff, fabsf(a[i][j] - b[i][j]));
            mag = std::max(mag, fabsf(b[i][j]));
        }
    }
    return diff > 0.25f * mag;
}

/* A teleport (a respawn): the model moved further than any car does in a frame */
static bool mv_jump(const float a[4][4], const float b[4][4]) {
    float dx = a[3][0] - b[3][0], dy = a[3][1] - b[3][1], dz = a[3][2] - b[3][2];
    return dx * dx + dy * dy + dz * dz > 50.0f * 50.0f;
}

static float mv_dist2(const InterpSlot &a, const InterpSlot &b) {
    float dx = a.mv[3][0] - b.mv[3][0], dy = a.mv[3][1] - b.mv[3][1], dz = a.mv[3][2] - b.mv[3][2];
    return dx * dx + dy * dy + dz * dz;
}

/* The model's size in a modelview matrix (the camera only rotates) */
static float mv_scale(const InterpSlot &a) {
    float n = 0.0f;
    for (int r = 0; r < 3; r++) {
        n += sqrtf(a.mv[r][0] * a.mv[r][0] + a.mv[r][1] * a.mv[r][1] + a.mv[r][2] * a.mv[r][2]);
    }
    return n / 3.0f;
}

/*
 * Each slot of cur paired with the slot of prev that drew the same thing. The
 * vertex data's address alone is not enough: a model drawn many times (a wheel
 * on every car) comes in another order when cars pass or leave the view, and
 * what the game builds each frame (the player's wheels, the tyre smoke) is in a
 * new buffer each time, which may be the one another object had the frame
 * before. So: among the slots with the same vertex data, the nearest pairs win;
 * the slots left over are paired the same way with prev's left over. A pair must
 * be under the same projection, at the same scale and less than a teleport apart.
 */
static void interp_match(const InterpFrame *cur, const InterpFrame *prev) {
    struct Pair {
        float d;
        int i, j;
    };
    static std::vector<Pair> pairs;
    static std::vector<uint8_t> taken;
    static std::vector<float> scale_cur, scale_prev;
    static std::unordered_map<uintptr_t, std::vector<int>> prev_by_addr;
    const size_t nc = cur->slots.size(), np = prev->slots.size();

    interp_matches.assign(nc, -1);
    taken.assign(np, 0);
    scale_cur.resize(nc);
    scale_prev.resize(np);
    for (size_t i = 0; i < nc; i++) {
        scale_cur[i] = mv_scale(cur->slots[i]);
    }
    for (size_t j = 0; j < np; j++) {
        scale_prev[j] = mv_scale(prev->slots[j]);
    }
    auto fits = [&](int i, int j, float &d) {
        const InterpSlot &a = cur->slots[i], &b = prev->slots[j];
        const float sa = scale_cur[i], sb = scale_prev[j];
        if (a.pkey != b.pkey || sa > sb * 1.2f || sb > sa * 1.2f) {
            return false;
        }
        d = mv_dist2(a, b);
        return d <= 50.0f * 50.0f;
    };
    auto assign = [&]() {
        std::sort(pairs.begin(), pairs.end(), [](const Pair &x, const Pair &y) { return x.d < y.d; });
        for (const Pair &p : pairs) {
            if (interp_matches[p.i] < 0 && !taken[p.j]) {
                interp_matches[p.i] = p.j;
                taken[p.j] = 1;
            }
        }
        pairs.clear();
    };

    /* the same vertex data */
    for (auto &e : prev_by_addr) {
        e.second.clear();
    }
    for (size_t j = 0; j < np; j++) {
        prev_by_addr[(uintptr_t)(prev->slots[j].key >> 16)].push_back((int)j);
    }
    pairs.clear();
    for (size_t i = 0; i < nc; i++) {
        auto it = prev_by_addr.find((uintptr_t)(cur->slots[i].key >> 16));
        if (it == prev_by_addr.end()) {
            continue;
        }
        for (int j : it->second) {
            float d;
            if (fits((int)i, j, d)) {
                pairs.push_back({ d, (int)i, j });
            }
        }
    }
    assign();

    /* the rest: built afresh each frame, or moved */
    std::vector<int> left;
    for (size_t j = 0; j < np; j++) {
        if (!taken[j]) {
            left.push_back((int)j);
        }
    }
    for (size_t i = 0; i < nc && !left.empty(); i++) {
        if (interp_matches[i] >= 0) {
            continue;
        }
        for (int j : left) {
            float d;
            if (fits((int)i, j, d)) {
                pairs.push_back({ d, (int)i, j });
            }
        }
    }
    assign();
}

/*
 * Draw the newest game frame again, its matrices blended from the frame before by
 * the time since it arrived, and show it. False if there is no recording yet.
 */
extern "C" bool gfx_interp_present(bool black) {
    InterpFrame *cur = interp_cur, *prev = interp_prev;

    if (!cur->valid || frame_started) {
        return false;
    }
    uint64_t now = SDL_GetTicksNS();
    float t = 1.0f;
    if (prev->valid && cur->t_ns > prev->t_ns) {
        float tick = (float)std::min<uint64_t>(std::max<uint64_t>(cur->t_ns - prev->t_ns, 4000000), 100000000);
        float since = (float)(now - cur->t_ns);
        t = gfx_interp_mode == 2 ? 1.0f + std::min(since / tick, 0.5f) : std::min(since / tick, 1.0f);
    }

    /* each slot's matrix for this moment */
    static std::vector<float> mp;
    static std::vector<FarRemap> far; /* each slot's, from its projection now ([video] draw_distance) */
    static unsigned n_presents, n_slots, n_blended, n_cut;
    static double behind_ms, ms_mtx, ms_replay, ms_present, ms_cpu_vtx, ms_gl_draw;
    uint64_t t0 = now;
    static const char *gfxlog = getenv("DD64_GFXLOG");
    mp.resize(cur->slots.size() * 16);
    far.resize(cur->slots.size());
    if (prev->valid && interp_matched_for != (const void *)cur) {
        interp_match(cur, prev);
        interp_matched_for = cur;
    }
    for (size_t i = 0; i < cur->slots.size(); i++) {
        const InterpSlot &s = cur->slots[i];
        float mv[4][4], p[4][4];
        memcpy(mv, s.mv, sizeof(mv));
        memcpy(p, s.p, sizeof(p));
        n_slots++;
        if (t != 1.0f && prev->valid) {
            auto pi = prev->by_pkey.find(s.pkey);
            if (pi != prev->by_pkey.end() && !proj_cut(prev->slots[pi->second].p, s.p)) {
                mtx_lerp(p, prev->slots[pi->second].p, s.p, t);
                int j = interp_matches[i];
                if (j >= 0 && !mv_jump(prev->slots[j].mv, s.mv)) {
                    mv_blend(mv, prev->slots[j].mv, s.mv, t);
                    n_blended++;
                }
            } else {
                n_cut++;
            }
        }
        float m[4][4];
        gfx_matrix_mul(m, mv, p);
        memcpy(&mp[i * 16], m, sizeof(m));
        far[i] = far_remap(p);
    }
    /* how far the picture is behind the newest game frame (negative: ahead of it) */
    if (prev->valid && cur->t_ns > prev->t_ns) {
        behind_ms += (1.0f - t) * (double)(cur->t_ns - prev->t_ns) / 1e6;
    }
    if (gfxlog != NULL && ++n_presents % 120 == 0) {
        static uint64_t since;
        static unsigned frames_then;
        double secs = since != 0 ? (now - since) / 1e9 : 0.0;
        sysLogPrintf(LOG_NOTE, "gfx: interpolation: %.0f frames/s shown, %.0f game frames/s; %u slots, %u blended, "
                     "%u cut; on average %.1f ms behind the newest game frame; %.2f ms matrices, %.2f ms replay "
                     "(%.2f vertices, %.2f draws), %.2f ms present",
                     secs > 0 ? 120 / secs : 0.0, secs > 0 ? (interp_game_frames - frames_then) / secs : 0.0, n_slots,
                     n_blended, n_cut, behind_ms / 120.0, ms_mtx / 120.0, ms_replay / 120.0, ms_cpu_vtx / 120.0,
                     ms_gl_draw / 120.0, ms_present / 120.0);
        since = now;
        frames_then = interp_game_frames;
        n_slots = n_blended = n_cut = 0;
        behind_ms = ms_mtx = ms_replay = ms_present = ms_cpu_vtx = ms_gl_draw = 0.0;
    }
    uint64_t t1 = SDL_GetTicksNS();
    ms_mtx += (t1 - t0) / 1e6;

    /* every draw's vertices, moved for this moment, in one array: each batch at a whole
       vertex of its own stride, so it can be drawn from where it lands */
    static std::vector<float> all;
    static std::vector<size_t> firsts;
    uint64_t tv0 = SDL_GetTicksNS();
    all.clear();
    firsts.clear();
    for (const InterpCmd &c : cur->cmds) {
        if (c.op != IOP_DRAW) {
            continue;
        }
        size_t verts = c.tris * 3, stride = c.vbo_len / verts;
        size_t first = (all.size() + stride - 1) / stride;
        all.resize(first * stride);
        firsts.push_back(first);
        all.insert(all.end(), cur->vbo.begin() + c.vbo_off, cur->vbo.begin() + c.vbo_off + c.vbo_len);
        float *out = &all[first * stride];
        for (size_t k = 0; k < verts; k++) {
            const InterpVtx &v = cur->vtx[c.vtx_off + k];
            if (v.slot < 0) {
                continue;
            }
            const float(*m)[4] = (const float(*)[4]) & mp[v.slot * 16];
            float x = v.ox * m[0][0] + v.oy * m[1][0] + v.oz * m[2][0] + m[3][0];
            float y = v.ox * m[0][1] + v.oy * m[1][1] + v.oz * m[2][1] + m[3][1];
            float z = v.ox * m[0][2] + v.oy * m[1][2] + v.oz * m[2][2] + m[3][2];
            float w = v.ox * m[0][3] + v.oy * m[1][3] + v.oz * m[2][3] + m[3][3];
            if (v.wide) {
                x *= wide_k;
            }
            z = std::isnan(v.zprim) ? z + v.zoff * w + far[v.slot].da * w + far[v.slot].db : v.zprim * w;
            float *o = &out[k * stride];
            o[0] = x, o[1] = y, o[2] = z, o[3] = w;
        }
    }
    ms_cpu_vtx += (SDL_GetTicksNS() - tv0) / 1e6;

    update_framebuffer_sizes();
    interp_real->start_frame();
    interp_real->start_draw_to_framebuffer(game_fb, 1.0f);
    uint64_t tu0 = SDL_GetTicksNS();
    interp_real->upload_vertices(all.data(), all.size());
    ms_gl_draw += (SDL_GetTicksNS() - tu0) / 1e6;
    size_t draw_i = 0;
    for (const InterpCmd &c : cur->cmds) {
        switch (c.op) {
            case IOP_LOAD_SHADER: interp_real->load_shader(c.prg); break;
            case IOP_UNLOAD_SHADER: interp_real->unload_shader(c.prg); break;
            case IOP_TEXTURE: interp_real->select_texture(c.a[0], (uint32_t)c.a[1], c.a[2]); break;
            case IOP_SAMPLER: interp_real->set_sampler_parameters(c.a[0], c.a[1], (uint32_t)c.a[2], (uint32_t)c.a[3]); break;
            case IOP_DEPTH: interp_real->set_depth_mode(c.a[0], c.a[1], c.a[2], c.a[3], (uint16_t)c.a[4]); break;
            case IOP_VIEWPORT: interp_real->set_viewport(c.a[0], c.a[1], c.a[2], c.a[3]); break;
            case IOP_SCISSOR: interp_real->set_scissor(c.a[0], c.a[1], c.a[2], c.a[3]); break;
            case IOP_ALPHA: interp_real->set_use_alpha(c.a[0], c.a[1]); break;
            case IOP_CLEAR: interp_real->clear_framebuffer(c.a[0], c.a[1]); break;
            case IOP_DRAW: {
                uint64_t td0 = SDL_GetTicksNS();
                interp_real->draw_uploaded(firsts[draw_i++], c.tris);
                ms_gl_draw += (SDL_GetTicksNS() - td0) / 1e6;
                break;
            }
        }
    }
    interp_real->end_frame();
    uint64_t t2 = SDL_GetTicksNS();
    present_to_window(black);
    ms_replay += (t2 - t1) / 1e6;
    ms_present += (SDL_GetTicksNS() - t2) / 1e6;
    return true;
}

/* ---- Frames ---- */

static void update_framebuffer_sizes(void) {
    uint32_t w = 0, h = 0;

    gfx_wapi->get_dimensions(&w, &h, NULL, NULL);
    if (w == 0 || h == 0) {
        w = DESIRED_SCREEN_WIDTH;
        h = DESIRED_SCREEN_HEIGHT;
    }
    window_width = w;
    window_height = h;

    /* The game is 4:3; it is drawn at the window's height (or the set line count) */
    fb_height = gfx_internal_height ? gfx_internal_height : h;
    fb_width = fb_height * 4 / 3;
    if (gfx_aspect_mode != 0 && w * 3 > h * 4) {
        fb_width = gfx_internal_height ? fb_height * w / h : w;
    }
    wide_k = (float)(fb_height * 4 / 3) / fb_width;
    gfx_rapi->update_framebuffer_parameters(0, window_width, window_height, 1, true);
    gfx_rapi->update_framebuffer_parameters(game_fb, fb_width, fb_height, gfx_msaa_level, true);
}

void gfx_init(const struct GfxInitSettings *settings) {
    gfx_wapi = settings->wapi;
    gfx_rapi = settings->rapi;
    gfx_wapi->init(&settings->window_settings);
    gfx_rapi->init();

    tex_upload_buffer = (uint8_t *)malloc(1024 * 1024 * 4 * 4);
    game_fb = gfx_rapi->create_framebuffer();
    for (int i = 0; i < 16; i++) {
        segment_base[i] = 0;
    }
    rsp.modelview_matrix_stack_size = 1;
    memset(rsp.modelview_matrix_stack[0], 0, sizeof(rsp.modelview_matrix_stack[0]));
    for (int i = 0; i < 4; i++) {
        rsp.modelview_matrix_stack[0][i][i] = 1.0f;
        rsp.P_matrix[i][i] = 1.0f;
        rsp.MP_matrix[i][i] = 1.0f;
    }
    rdp.viewport = { 0, 0, NATIVE_WIDTH, NATIVE_HEIGHT };
    rdp.scissor = { 0, 0, NATIVE_WIDTH, NATIVE_HEIGHT };
    rdp.viewport_or_scissor_changed = true;
    update_framebuffer_sizes();
}

void gfx_start_frame(void) {
    if (frame_started) {
        return;
    }
    update_framebuffer_sizes();
    gfx_rapi->start_frame();
    gfx_rapi->start_draw_to_framebuffer(game_fb, 1.0f);
    interp_begin_frame();
    rendering_state = RenderingState();
    gfx_rapi->set_depth_mode(false, false, false, false, 0);
    gfx_rapi->set_use_alpha(false, false);
    rendering_state.shader_program = NULL;
    rdp.viewport_or_scissor_changed = true;
    rdp.textures_changed[0] = rdp.textures_changed[1] = true;
    frame_started = true;
}

void gfx_run(const void *commands, uint32_t ucode_text) {
    stats.tasks++;
    gfx_start_frame();
    s2dex = (ucode_text & 0x1FFFFFFF) == UCODE_S2DEX2;
    gfx_run_dl((const uint8_t *)commands);
    FLUSH("end");
    flush_log_frame();
}

void gfx_end_frame(void) {
    frame_started = false;
    interp_end_frame();
}

static void present_to_window(bool black);

void gfx_present(bool black) {
    gfx_flush();
    if (!frame_started) {
        update_framebuffer_sizes();
    }
    present_to_window(black);
}

static void present_to_window(bool black) {
    /* Into the window: 4:3 in the middle, black bars beside it */
    gfx_rapi->start_draw_to_framebuffer(0, 1.0f);
    gfx_rapi->clear_framebuffer(true, true);
    if (!black) {
        uint32_t pw = window_height * 4 / 3;
        if (gfx_aspect_mode != 0 || pw > window_width) {
            pw = window_width;
        }
        g_GfxPresentX = (window_width - pw) / 2;
        g_GfxPresentW = pw;
        if (gfx_n64_aa && gfx_msaa_level > 1 && gfx_rapi->resolve_n64_aa != NULL) {
            gfx_rapi->resolve_n64_aa(0, game_fb);
        } else {
            gfx_rapi->resolve_msaa_color_buffer(0, game_fb);
        }
        g_GfxPresentX = g_GfxPresentW = 0;
    }
    {
        /* Frames presented per second, counted over the last second */
        static uint64_t since;
        static unsigned frames, shown;
        uint64_t now = SDL_GetTicksNS();
        frames++;
        if (since == 0) {
            since = now;
        } else if (now - since >= 1000000000ULL) {
            shown = (unsigned)((frames * 1000000000ULL + (now - since) / 2) / (now - since));
            frames = 0;
            since = now;
        }
        if (gfx_show_fps) {
            gfx_rapi->draw_counter((int)shown);
        }
    }
    {
        /* DD64_SHOT=n[,n...]: save presents number n as shot_<n>.bmp (for testing) */
        static const char *shots = getenv("DD64_SHOT");
        unsigned presents = ++gfx_presents;
        if (shots != NULL) {
            char buf[256];
            snprintf(buf, sizeof(buf), ",%s,", shots);
            char want[32];
            snprintf(want, sizeof(want), ",%u,", presents);
            if (strstr(buf, want) != NULL) {
                char path[64];
                snprintf(path, sizeof(path), "shot_%u.bmp", presents);
                gfx_rapi->screenshot(path);
            }
        }
        static const char *gfxlog = getenv("DD64_GFXLOG");
        if (gfxlog != NULL && presents % 60 == 0) {
            sysLogPrintf(LOG_NOTE, "gfx: present %u black %d: %u tasks %u cmds %u tris %u fills %u texrects %u s2dex",
                         presents, black, stats.tasks, stats.cmds, stats.tris, stats.fills, stats.texrects, stats.s2);
        }
        memset(&stats, 0, sizeof(stats));
        static const char *exit_after = getenv("DD64_EXIT_AFTER");
        if (exit_after != NULL && presents >= (unsigned)atoi(exit_after)) {
            exit(0);
        }
    }
    gfx_rapi->end_frame();
    gfx_wapi->swap_buffers_begin();
    gfx_wapi->swap_buffers_end();
    frame_started = false;
}

void gfx_set_vsync(int interval) {
    gfx_wapi->set_swap_interval(interval);
}

/* Three-point filtering is in the combiner programs (SHADER_OPT_THREE_POINT: a
   change of mode picks other programs) and turns the samplers' own filtering off,
   which each texture gets as it is loaded: so a change reloads the textures */
void gfx_set_texture_filter(enum FilteringMode mode) {
    if (mode != texture_filter_mode && gfx_rapi != NULL) {
        gfx_texture_cache_clear();
    }
    texture_filter_mode = mode;
    gfx_rapi->set_texture_filter(mode);
}
