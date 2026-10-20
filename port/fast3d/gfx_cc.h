#ifndef GFX_CC_H
#define GFX_CC_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#include <compare>
#endif

enum {
    SHADER_0,
    SHADER_INPUT_1,
    SHADER_INPUT_2,
    SHADER_INPUT_3,
    SHADER_INPUT_4,
    SHADER_INPUT_5,
    SHADER_INPUT_6,
    SHADER_INPUT_7,
    SHADER_TEXEL0,
    SHADER_TEXEL0A,
    SHADER_TEXEL1,
    SHADER_TEXEL1A,
    SHADER_1,
    SHADER_COMBINED,
    SHADER_NOISE,
    SHADER_LOD_FRAC,
};

#define SHADER_OPT_ALPHA (1 << 0)
#define SHADER_OPT_FOG (1 << 1)
#define SHADER_OPT_TEXTURE_EDGE (1 << 2)
#define SHADER_OPT_NOISE (1 << 3)
#define SHADER_OPT_2CYC (1 << 4)
#define SHADER_OPT_ALPHA_THRESHOLD (1 << 5)
#define SHADER_OPT_INVISIBLE (1 << 6)
#define SHADER_OPT_GRAYSCALE (1 << 7)
#define SHADER_OPT_TEXEL0_CLAMP_S (1 << 8)
#define SHADER_OPT_TEXEL0_CLAMP_T (1 << 9)
#define SHADER_OPT_TEXEL1_CLAMP_S (1 << 10)
#define SHADER_OPT_TEXEL1_CLAMP_T (1 << 11)
#define SHADER_OPT_BLUR (1 << 12)
#define SHADER_OPT_ALPHA_CVG_SEL (1 << 13) /* GE: RDP ALPHA_CVG_SEL without CVG_X_ALPHA - blend alpha is coverage (opaque interior), not combiner alpha */
#define SHADER_OPT_NOPERSP_SHADE (1 << 14) /* GE: shade varyings interpolate screen-linearly (noperspective) like the RDP - sky/water cells carrying real clip W */
#define SHADER_OPT_SKY_RGB5 (1 << 15)
/* CAUTION: bits 16-27 of the options word are cleared by gfx_sp_tri1's
 * !use_alpha normalization (a vestigial upstream mask) - new shader opts
 * must NOT live there. Bits 28-31 are free (nothing reads them; shader_id1 is
 * uint32). Placing the pixel-fog and far-clip opts at bits 16/17 made the mask
 * strip them from the shader key while the vertex emit still pushed the pixfog
 * floats, causing a stride mismatch and garbage rendering. */
#define SHADER_OPT_GE_FARCLIP (1u << 28) /* GE: fogless world draws clip at the projection far plane like the console - fragment discard where clip z > w */
#define SHADER_OPT_GE_PIXFOG (1u << 29)  /* GE: PER-PIXEL fog for G_FOG world draws like the RDP - vertex fog interpolated across GE's huge terrain tris under a steep fog ramp mid-fogs whole slabs (e.g. the olive band in Jungle); fragment recomputes clamp(z/w*mul+off) */      /* GE: sky/water draws land in the N64's RGBA5551 framebuffer - truncate to 5 bits so the faint near-horizon cloud gradient collapses to sky colour as it does on hardware */
#define SHADER_OPT_GE_SKY_PIXDIV (1u << 30) /* GE: sky/water do the perspective texture divide per pixel like the RDP (vertex carries u*q, v*q, q), replacing the CPU subdivision grid */

struct ColorCombinerKey {
    uint64_t combine_mode;
    uint64_t options;
    auto operator<=>(const ColorCombinerKey&) const = default;
};

#define SHADER_MAX_TEXTURES 2
#define SHADER_FIRST_TEXTURE 0

struct CCFeatures {
    uint8_t c[2][2][4];
    bool opt_alpha;
    bool opt_fog;
    bool opt_texture_edge;
    bool opt_noise;
    bool opt_2cyc;
    bool opt_alpha_threshold;
    bool opt_invisible;
    bool opt_grayscale;
    bool opt_blur;
    bool opt_alpha_cvg_sel;
    bool opt_nopersp_shade;
    bool opt_sky_rgb5;
    bool opt_ge_farclip;
    bool opt_ge_pixfog;
    bool opt_ge_sky_pixdiv;
    bool used_textures[2];
    bool clamp[2][2];
    int num_inputs;
    bool do_single[2][2];
    bool do_multiply[2][2];
    bool do_mix[2][2];
    bool color_alpha_same[2];
};

void gfx_cc_get_features(uint64_t shader_id0, uint32_t shader_id1, struct CCFeatures* cc_features);

#endif
