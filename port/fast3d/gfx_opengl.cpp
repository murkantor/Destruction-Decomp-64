/*
 * OpenGL backend for the display-list interpreter (gfx_pc.cpp): shader
 * generation from the colour combiner, textures and samplers, depth and blend
 * state, the game framebuffer with its MSAA resolve, and the N64-style VI
 * anti-aliasing resolve. From Project Midas's fast3d, with its GoldenEye-only
 * features removed.
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <map>
#include <vector>
#include <string>

#include <SDL3/SDL.h>

#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif
#include <PR/gbi.h>

#include "glad/glad.h"

#include "gfx_cc.h"
#include "gfx_rendering_api.h"
#include "gfx_pc.h"

using namespace std;

struct ShaderProgram {
    GLuint opengl_program_id;
    uint8_t num_floats;
    GLint attrib_locations[16];
    uint8_t attrib_sizes[16];
    uint8_t num_attribs;
    GLint frame_count_location;
    GLint noise_scale_location;
    GLint three_point_filter_locations[2];
};

struct Framebuffer {
    uint32_t width, height;
    bool has_depth_buffer;
    uint32_t msaa_level;

    GLuint fbo, clrbuf, clrbuf_msaa, rbo;
    GLuint clrtex_msaa;   /* N64 AA: the colour target as a multisample TEXTURE (readable per sample) */
    bool msaa_is_texture; /* which of the two is attached */
};

static std::map<pair<uint64_t, uint32_t>, struct ShaderProgram> shader_program_pool;
static GLuint opengl_vbo;
static GLuint opengl_vao;
static bool current_depth_mask;

static uint32_t frame_count;

static std::vector<Framebuffer> framebuffers;
static size_t current_framebuffer;
static float current_noise_scale;
static FilteringMode current_filter_mode = FILTER_LINEAR;
static bool current_textures_linear_filter[2] = {false, false};
/* What three_point_filter value the bound program holds, as opposed to what the
 * next draw wants: it is a uniform, so a draw that changes only the filter
 * (same program) has to push it, or the next draw on that program keeps the old
 * value. -1 = nothing pushed yet. */
static GLint gl_pushed_three[2] = {-1, -1};
static struct ShaderProgram* gl_current_prg = nullptr; /* gfx_opengl_load_shader's last program */
/* Which texture object each unit holds, plus a per-object "has a mip chain" bit
 * (indexed by GL name: small dense ints) so a chain is generated once per
 * upload, on demand, never sampled incomplete. */
static GLuint current_texture_ids[2] = {0, 0};
static int last_selected_tile = 0;
static std::vector<uint8_t> gl_tex_has_mips;
static void gl_set_has_mips(GLuint id, uint8_t v) {
    if (id >= gl_tex_has_mips.size()) {
        gl_tex_has_mips.resize((size_t)id + 64, 0);
    }
    gl_tex_has_mips[id] = v;
}
static bool gl_has_mips(GLuint id) {
    return id < gl_tex_has_mips.size() && gl_tex_has_mips[id] != 0;
}

static int gl_glsl_version = 130;
static char gl_glsl_version_str[16] = "130";
static GLenum gl_mirror_clamp = GL_MIRROR_CLAMP_TO_EDGE;
static bool gl_es = false;
static bool gl_core_profile = false;

/* The interpreter's framebuffer, bound again after work on another one */
static void gl_bind_current_framebuffer(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.empty() ? 0 : framebuffers[current_framebuffer].fbo);
}

static void gfx_opengl_vertex_array_set_attribs(struct ShaderProgram* prg) {
    size_t num_floats = prg->num_floats;
    size_t pos = 0;

    for (int i = 0; i < prg->num_attribs; i++) {
        if (prg->attrib_locations[i] >= 0) {
            glEnableVertexAttribArray(prg->attrib_locations[i]);
            glVertexAttribPointer(prg->attrib_locations[i], prg->attrib_sizes[i], GL_FLOAT, GL_FALSE,
                                num_floats * sizeof(float), (void*)(pos * sizeof(float)));
        }
        pos += prg->attrib_sizes[i];
    }
}

static void gfx_opengl_set_uniforms(struct ShaderProgram* prg) {
    if (prg->frame_count_location >= 0) {
        glUniform1i(prg->frame_count_location, frame_count);
    }
    if (prg->noise_scale_location >= 0) {
        glUniform1f(prg->noise_scale_location, current_noise_scale);
    }
    for (int t = 0; t < 2; t++) {
        if (prg->three_point_filter_locations[t] >= 0) {
            glUniform1i(prg->three_point_filter_locations[t], current_textures_linear_filter[t]);
            gl_pushed_three[t] = (GLint) current_textures_linear_filter[t];
        } else {
            gl_pushed_three[t] = -1;
        }
    }
}

/**
 * Push three_point_filter if this draw wants something other than what the
 * shader is already carrying. Called wherever the wanted value can change
 * without the program changing.
 */
static void gfx_opengl_sync_three_point(void) {
    struct ShaderProgram* prg = gl_current_prg;

    if (prg == NULL) {
        return;
    }
    for (int t = 0; t < 2; t++) {
        if (prg->three_point_filter_locations[t] >= 0
            && gl_pushed_three[t] != (GLint) current_textures_linear_filter[t]) {
            gl_pushed_three[t] = (GLint) current_textures_linear_filter[t];
            glUniform1i(prg->three_point_filter_locations[t], current_textures_linear_filter[t]);
        }
    }
}

static void gfx_opengl_unload_shader(struct ShaderProgram* old_prg) {
    if (old_prg != NULL) {
        for (int i = 0; i < old_prg->num_attribs; i++) {
            if (old_prg->attrib_locations[i] >= 0) {
                glDisableVertexAttribArray(old_prg->attrib_locations[i]);
            }
        }
    }
}

static void gfx_opengl_load_shader(struct ShaderProgram* new_prg) {
    gl_current_prg = new_prg;
    glUseProgram(new_prg->opengl_program_id);
    gfx_opengl_vertex_array_set_attribs(new_prg);
    gfx_opengl_set_uniforms(new_prg);
}

/* ---- Shader text ---- */

/* A shader's source, appended to with bounds checks */
struct ShaderText {
    char buf[8192];
    size_t len;
};

static void st_add(ShaderText* t, const char* fmt, ...) {
    size_t room = sizeof(t->buf) - t->len;
    va_list ap;

    va_start(ap, fmt);
    int n = vsnprintf(t->buf + t->len, room, fmt, ap);
    va_end(ap);
    if (n < 0 || (size_t)n >= room) {
        sysFatalError("Shader source longer than %u bytes", (unsigned)sizeof(t->buf));
    }
    t->len += (size_t)n;
}

static void st_line(ShaderText* t, const char* line) {
    st_add(t, "%s\n", line);
}

#define RAND_NOISE "((random(vec3(floor(gl_FragCoord.xy * noise_scale), float(frame_count))) + 1.0) / 2.0)"

static const char* shader_item_to_str(uint32_t item, bool with_alpha, bool only_alpha, bool inputs_have_alpha,
                                      bool hint_single_element) {
    if (!only_alpha) {
        switch (item) {
            case SHADER_0:
                return with_alpha ? "vec4(0.0, 0.0, 0.0, 0.0)" : "vec3(0.0, 0.0, 0.0)";
            case SHADER_1:
                return with_alpha ? "vec4(1.0, 1.0, 1.0, 1.0)" : "vec3(1.0, 1.0, 1.0)";
            case SHADER_INPUT_1:
                return with_alpha || !inputs_have_alpha ? "vInput1" : "vInput1.rgb";
            case SHADER_INPUT_2:
                return with_alpha || !inputs_have_alpha ? "vInput2" : "vInput2.rgb";
            case SHADER_INPUT_3:
                return with_alpha || !inputs_have_alpha ? "vInput3" : "vInput3.rgb";
            case SHADER_INPUT_4:
                return with_alpha || !inputs_have_alpha ? "vInput4" : "vInput4.rgb";
            case SHADER_INPUT_5:
                return with_alpha || !inputs_have_alpha ? "vInput5" : "vInput5.rgb";
            case SHADER_INPUT_6:
                return with_alpha || !inputs_have_alpha ? "vInput6" : "vInput6.rgb";
            case SHADER_INPUT_7:
                return with_alpha || !inputs_have_alpha ? "vInput7" : "vInput7.rgb";
            case SHADER_TEXEL0:
                return with_alpha ? "texVal0" : "texVal0.rgb";
            case SHADER_TEXEL0A:
                return hint_single_element ? "texVal0.a"
                                           : (with_alpha ? "vec4(texVal0.a, texVal0.a, texVal0.a, texVal0.a)"
                                                         : "vec3(texVal0.a, texVal0.a, texVal0.a)");
            case SHADER_TEXEL1A:
                return hint_single_element ? "texVal1.a"
                                           : (with_alpha ? "vec4(texVal1.a, texVal1.a, texVal1.a, texVal1.a)"
                                                         : "vec3(texVal1.a, texVal1.a, texVal1.a)");
            case SHADER_TEXEL1:
                return with_alpha ? "texVal1" : "texVal1.rgb";
            case SHADER_COMBINED:
                return with_alpha ? "texel" : "texel.rgb";
            case SHADER_NOISE:
                return with_alpha ? "vec4(" RAND_NOISE ", " RAND_NOISE ", " RAND_NOISE ", " RAND_NOISE ")"
                                  : "vec3(" RAND_NOISE ", " RAND_NOISE ", " RAND_NOISE ")";
        }
    } else {
        switch (item) {
            case SHADER_0:
                return "0.0";
            case SHADER_1:
                return "1.0";
            case SHADER_INPUT_1:
                return "vInput1.a";
            case SHADER_INPUT_2:
                return "vInput2.a";
            case SHADER_INPUT_3:
                return "vInput3.a";
            case SHADER_INPUT_4:
                return "vInput4.a";
            case SHADER_INPUT_5:
                return "vInput5.a";
            case SHADER_INPUT_6:
                return "vInput6.a";
            case SHADER_INPUT_7:
                return "vInput7.a";
            case SHADER_TEXEL0:
                return "texVal0.a";
            case SHADER_TEXEL0A:
                return "texVal0.a";
            case SHADER_TEXEL1A:
                return "texVal1.a";
            case SHADER_TEXEL1:
                return "texVal1.a";
            case SHADER_COMBINED:
                return "texel.a";
            case SHADER_NOISE:
                return RAND_NOISE;
        }
    }
    return "";
}

#undef RAND_NOISE

static void append_formula(ShaderText* t, uint8_t c[2][4], bool do_single, bool do_multiply, bool do_mix,
                           bool with_alpha, bool only_alpha, bool opt_alpha) {
    if (do_single) {
        st_add(t, "%s", shader_item_to_str(c[only_alpha][3], with_alpha, only_alpha, opt_alpha, false));
    } else if (do_multiply) {
        st_add(t, "%s * %s", shader_item_to_str(c[only_alpha][0], with_alpha, only_alpha, opt_alpha, false),
               shader_item_to_str(c[only_alpha][2], with_alpha, only_alpha, opt_alpha, true));
    } else if (do_mix) {
        st_add(t, "mix(%s, %s, %s)", shader_item_to_str(c[only_alpha][1], with_alpha, only_alpha, opt_alpha, false),
               shader_item_to_str(c[only_alpha][0], with_alpha, only_alpha, opt_alpha, false),
               shader_item_to_str(c[only_alpha][2], with_alpha, only_alpha, opt_alpha, true));
    } else {
        st_add(t, "(%s - %s) * %s + %s", shader_item_to_str(c[only_alpha][0], with_alpha, only_alpha, opt_alpha, false),
               shader_item_to_str(c[only_alpha][1], with_alpha, only_alpha, opt_alpha, false),
               shader_item_to_str(c[only_alpha][2], with_alpha, only_alpha, opt_alpha, true),
               shader_item_to_str(c[only_alpha][3], with_alpha, only_alpha, opt_alpha, false));
    }
}

static GLuint compile_shader(GLenum type, const ShaderText* src, const char* what, uint64_t shader_id0,
                             uint32_t shader_id1) {
    const GLchar* text = src->buf;
    const GLint length = (GLint)src->len;
    GLint success;
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &text, &length);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char error_log[1024];
        glGetShaderInfoLog(shader, sizeof(error_log), NULL, error_log);
        sysLogPrintf(LOG_ERROR, "Failed to compile this %s shader (ID %llx, %x):\n%.*s", what,
                     (unsigned long long)shader_id0, shader_id1, (int)src->len, src->buf);
        sysFatalError("%s shader compilation failed:\n%s", what, error_log);
    }
    return shader;
}

static struct ShaderProgram* gfx_opengl_create_and_load_new_shader(uint64_t shader_id0, uint32_t shader_id1) {
    struct CCFeatures cc_features = { 0 };
    gfx_cc_get_features(shader_id0, shader_id1, &cc_features);

    static ShaderText vs, fs;
    vs.len = fs.len = 0;
    size_t num_floats = 4;
    const int input_size = cc_features.opt_alpha ? 4 : 3;
    const bool three_point = current_filter_mode == FILTER_THREE_POINT;

    // Vertex shader

    st_add(&vs, "#version %s\n", gl_glsl_version_str);
    if (gl_es) {
        st_line(&vs, "precision mediump float;");
    }
    if (gl_glsl_version >= 130) {
        st_line(&vs, "#define INPUT in");
        st_line(&vs, "#define OUTPUT out");
    } else {
        st_line(&vs, "#define INPUT attribute");
        st_line(&vs, "#define OUTPUT varying");
    }
    st_line(&vs, "INPUT vec4 aVtxPos;");
    for (int i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            st_add(&vs, "INPUT vec2 aTexCoord%d;\n", i);
            st_add(&vs, "OUTPUT vec2 vTexCoord%d;\n", i);
            num_floats += 2;
        }
    }
    if (cc_features.opt_fog) {
        st_line(&vs, "INPUT vec4 aFog;");
        st_line(&vs, "OUTPUT vec4 vFog;");
        num_floats += 4;
    }
    for (int i = 0; i < cc_features.num_inputs; i++) {
        st_add(&vs, "INPUT vec%d aInput%d;\n", input_size, i + 1);
        st_add(&vs, "OUTPUT vec%d vInput%d;\n", input_size, i + 1);
        num_floats += input_size;
    }
    st_line(&vs, "void main() {");
    for (int i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            st_add(&vs, "    vTexCoord%d = aTexCoord%d;\n", i, i);
        }
    }
    if (cc_features.opt_fog) {
        st_line(&vs, "    vFog = aFog;");
    }
    for (int i = 0; i < cc_features.num_inputs; i++) {
        st_add(&vs, "    vInput%d = aInput%d;\n", i + 1, i + 1);
    }
    st_line(&vs, "    gl_Position = aVtxPos;");
    if (!GLAD_GL_ARB_depth_clamp) {
        // HACK: workaround for no GL_DEPTH_CLAMP
        st_line(&vs, "    gl_Position.z *= 0.3f;");
    }
    st_line(&vs, "}");

    // Fragment shader

    st_add(&fs, "#version %s\n", gl_glsl_version_str);
    if (gl_es) {
        st_line(&fs, "precision mediump float;");
    }
    if (gl_glsl_version >= 130) {
        st_line(&fs, "#define INPUT in");
        st_line(&fs, "#define OUTPUT_COLOR outColor");
        st_line(&fs, "#define SAMPLE_TEX(tex, uv) texture(tex, uv)");
    } else {
        st_line(&fs, "#define INPUT varying");
        st_line(&fs, "#define OUTPUT_COLOR gl_FragColor");
        st_line(&fs, "#define SAMPLE_TEX(tex, uv) texture2D(tex, uv)");
    }
    // Reference approach to color wrapping as per GLideN64
    // Return wrapped value of x in interval [low, high)
    st_line(&fs, "#define WRAP(x, low, high) mod((x)-(low), (high)-(low)) + (low)");
    st_line(&fs, "#define TEX_OFFSET(tex, uv, texSize, off) SAMPLE_TEX(tex, uv - (off)/texSize)");

    for (int i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            st_add(&fs, "INPUT vec2 vTexCoord%d;\n", i);
        }
    }
    if (cc_features.opt_fog) {
        st_line(&fs, "INPUT vec4 vFog;");
    }
    for (int i = 0; i < cc_features.num_inputs; i++) {
        st_add(&fs, "INPUT vec%d vInput%d;\n", input_size, i + 1);
    }
    for (int i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            st_add(&fs, "uniform sampler2D uTex%d;\n", i);
            if (three_point) {
                st_add(&fs, "uniform int three_point_filter%d;\n", i);
            }
        }
    }
    st_line(&fs, "uniform int frame_count;");
    st_line(&fs, "uniform float noise_scale;");

    st_line(&fs, "float random(in vec3 value) {");
    st_line(&fs, "    float random = dot(sin(value), vec3(12.9898, 78.233, 37.719));");
    st_line(&fs, "    return fract(sin(random) * 143758.5453);");
    st_line(&fs, "}");

    if (three_point) {
        st_line(&fs, "vec4 filter3point(in sampler2D tex, in vec2 texCoord, in vec2 texSize) {");
        st_line(&fs, "    vec2 offset = fract(texCoord*texSize - vec2(0.5));");
        st_line(&fs, "    offset -= step(1.0, offset.x + offset.y);");
        st_line(&fs, "    vec4 c0 = TEX_OFFSET(tex, texCoord, texSize, offset);");
        st_line(&fs, "    vec4 c1 = TEX_OFFSET(tex, texCoord, texSize, vec2(offset.x - sign(offset.x), offset.y));");
        st_line(&fs, "    vec4 c2 = TEX_OFFSET(tex, texCoord, texSize, vec2(offset.x, offset.y - sign(offset.y)));");
        st_line(&fs, "    return c0 + abs(offset.x)*(c1-c0) + abs(offset.y)*(c2-c0);");
        st_line(&fs, "}");
        /* float(): GLSL ES has no implicit int-to-float conversion */
        st_line(&fs, "vec4 hookTexture2D(in sampler2D tex, in vec2 uv, in vec2 texSize, in int three_point_filter) {");
        st_line(&fs, "    return mix(SAMPLE_TEX(tex, uv), filter3point(tex, uv, texSize), float(three_point_filter));");
        st_line(&fs, "}");
    } else {
        st_line(&fs, "vec4 hookTexture2D(in sampler2D tex, in vec2 uv, in vec2 texSize) {");
        st_line(&fs, "    return SAMPLE_TEX(tex, uv);");
        st_line(&fs, "}");
    }

    if (gl_glsl_version >= 130) {
        st_line(&fs, "out vec4 outColor;");
    }

    st_line(&fs, "void main() {");
    for (int i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            st_add(&fs, "    vec2 texSize%d = vec2(textureSize(uTex%d, 0));\n", i, i);
            if (three_point) {
                st_add(&fs, "    vec4 texVal%d = hookTexture2D(uTex%d, vTexCoord%d, texSize%d, three_point_filter%d);\n",
                       i, i, i, i, i);
            } else {
                st_add(&fs, "    vec4 texVal%d = hookTexture2D(uTex%d, vTexCoord%d, texSize%d);\n", i, i, i, i);
            }
        }
    }

    st_line(&fs, cc_features.opt_alpha ? "    vec4 texel;" : "    vec3 texel;");
    for (int c = 0; c < (cc_features.opt_2cyc ? 2 : 1); c++) {
        st_add(&fs, "    texel = ");
        if (!cc_features.color_alpha_same[c] && cc_features.opt_alpha) {
            st_add(&fs, "vec4(");
            append_formula(&fs, cc_features.c[c], cc_features.do_single[c][0], cc_features.do_multiply[c][0],
                           cc_features.do_mix[c][0], false, false, true);
            st_add(&fs, ", ");
            append_formula(&fs, cc_features.c[c], cc_features.do_single[c][1], cc_features.do_multiply[c][1],
                           cc_features.do_mix[c][1], true, true, true);
            st_add(&fs, ")");
        } else {
            append_formula(&fs, cc_features.c[c], cc_features.do_single[c][0], cc_features.do_multiply[c][0],
                           cc_features.do_mix[c][0], cc_features.opt_alpha, false, cc_features.opt_alpha);
        }
        st_line(&fs, ";");
        if (c == 0) {
            st_line(&fs, "    texel = WRAP(texel, -1.01, 1.01);");
        }
    }
    st_line(&fs, "    texel = WRAP(texel, -0.51, 1.51);");
    st_line(&fs, "    texel = clamp(texel, 0.0, 1.0);");
    if (cc_features.opt_fog) {
        if (cc_features.opt_alpha) {
            st_line(&fs, "    texel = vec4(mix(texel.rgb, vFog.rgb, vFog.a), texel.a);");
        } else {
            st_line(&fs, "    texel = mix(texel, vFog.rgb, vFog.a);");
        }
    }
    if (cc_features.opt_texture_edge && cc_features.opt_alpha) {
        st_line(&fs, "    if (texel.a > 0.19) texel.a = 1.0; else discard;");
    }
    if (cc_features.opt_alpha && cc_features.opt_noise) {
        st_line(&fs, "    texel.a *= floor(clamp(random(vec3(floor(gl_FragCoord.xy * noise_scale), float(frame_count))) + "
                     "texel.a, 0.0, 1.0));");
    }
    if (cc_features.opt_alpha) {
        if (cc_features.opt_alpha_threshold) {
            st_line(&fs, "    if (texel.a < 8.0 / 256.0) discard;");
        }
        st_line(&fs, "    OUTPUT_COLOR = texel;");
    } else {
        st_line(&fs, "    OUTPUT_COLOR = vec4(texel, 1.0);");
    }
    st_line(&fs, "}");

    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, &vs, "vertex", shader_id0, shader_id1);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, &fs, "fragment", shader_id0, shader_id1);
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glDetachShader(shader_program, vertex_shader);
    glDetachShader(shader_program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    size_t cnt = 0;
    struct ShaderProgram* prg = &shader_program_pool[make_pair(shader_id0, shader_id1)];
    prg->attrib_locations[cnt] = glGetAttribLocation(shader_program, "aVtxPos");
    prg->attrib_sizes[cnt] = 4;
    ++cnt;
    for (int i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            char name[32];
            snprintf(name, sizeof(name), "aTexCoord%d", i);
            prg->attrib_locations[cnt] = glGetAttribLocation(shader_program, name);
            prg->attrib_sizes[cnt] = 2;
            ++cnt;
        }
    }
    if (cc_features.opt_fog) {
        prg->attrib_locations[cnt] = glGetAttribLocation(shader_program, "aFog");
        prg->attrib_sizes[cnt] = 4;
        ++cnt;
    }
    for (int i = 0; i < cc_features.num_inputs; i++) {
        char name[16];
        snprintf(name, sizeof(name), "aInput%d", i + 1);
        prg->attrib_locations[cnt] = glGetAttribLocation(shader_program, name);
        prg->attrib_sizes[cnt] = input_size;
        ++cnt;
    }
    prg->opengl_program_id = shader_program;
    prg->num_floats = num_floats;
    prg->num_attribs = cnt;

    gl_current_prg = prg;
    glUseProgram(shader_program);
    for (int i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            char name[8];
            snprintf(name, sizeof(name), "uTex%d", i);
            glUniform1i(glGetUniformLocation(shader_program, name), i);
        }
    }
    prg->frame_count_location = glGetUniformLocation(shader_program, "frame_count");
    prg->noise_scale_location = glGetUniformLocation(shader_program, "noise_scale");
    prg->three_point_filter_locations[0] = glGetUniformLocation(shader_program, "three_point_filter0");
    prg->three_point_filter_locations[1] = glGetUniformLocation(shader_program, "three_point_filter1");

    gfx_opengl_load_shader(prg);
    return prg;
}

static struct ShaderProgram* gfx_opengl_lookup_shader(uint64_t shader_id0, uint32_t shader_id1) {
    auto it = shader_program_pool.find(make_pair(shader_id0, shader_id1));
    return it == shader_program_pool.end() ? nullptr : &it->second;
}

/* ---- Textures ---- */

static GLuint gfx_opengl_new_texture(void) {
    GLuint ret;
    glGenTextures(1, &ret);
    return ret;
}

static void gfx_opengl_delete_texture(uint32_t texID) {
    glDeleteTextures(1, &texID);
    gl_set_has_mips(texID, 0);
}

static void gfx_opengl_select_texture(int tile, GLuint texture_id, bool linear_filter) {
    glActiveTexture(GL_TEXTURE0 + tile);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    current_textures_linear_filter[tile] = linear_filter;
    current_texture_ids[tile] = texture_id;
    last_selected_tile = tile;
    gfx_opengl_sync_three_point();
}

/* Anisotropic filtering (EXT_texture_filter_anisotropic; core-adjacent,
 * effectively universal). Needs a mip chain to act on, so anisotropy > 1
 * also switches minification to trilinear mips generated at upload. */
#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif
#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif
static float gl_max_anisotropy = 0.0f; /* 0 = unqueried, <=1 = unsupported */

static float gfx_opengl_anisotropy(void) {
    if (gfx_anisotropy <= 1) {
        return 0.0f;
    }
    if (gl_max_anisotropy == 0.0f) {
        glGetError(); /* clear */
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_max_anisotropy);
        if (glGetError() != GL_NO_ERROR || gl_max_anisotropy < 1.0f) {
            gl_max_anisotropy = 1.0f; /* unsupported */
        }
    }
    float aniso = (float)gfx_anisotropy;
    if (aniso > gl_max_anisotropy) {
        aniso = gl_max_anisotropy;
    }
    return (aniso > 1.0f) ? aniso : 0.0f;
}

static void gfx_opengl_upload_texture(const uint8_t* rgba32_buf, uint32_t width, uint32_t height) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba32_buf);
    /* a fresh level 0 invalidates any older chain on this object */
    gl_set_has_mips(current_texture_ids[last_selected_tile], 0);
    if (gfx_opengl_anisotropy() > 1.0f) {
        glGenerateMipmap(GL_TEXTURE_2D); /* aniso acts on minification mips */
        gl_set_has_mips(current_texture_ids[last_selected_tile], 1);
    }
}

static uint32_t gfx_cm_to_opengl(uint32_t val) {
    switch (val) {
        case G_TX_NOMIRROR | G_TX_CLAMP:
            return GL_CLAMP_TO_EDGE;
        case G_TX_MIRROR | G_TX_WRAP:
            return GL_MIRRORED_REPEAT;
        case G_TX_MIRROR | G_TX_CLAMP:
            return gl_mirror_clamp;
        case G_TX_NOMIRROR | G_TX_WRAP:
            return GL_REPEAT;
    }
    return 0;
}

static void gfx_opengl_set_sampler_parameters(int tile, bool linear_filter, uint32_t cms, uint32_t cmt) {
    const GLint filter = linear_filter && (current_filter_mode == FILTER_LINEAR) ? GL_LINEAR : GL_NEAREST;
    /* This is the value the draw actually wants (select_texture only saw the
     * texture's cached flag): record it and push it to the shader. */
    if (tile >= 0 && tile < 2) {
        current_textures_linear_filter[tile] = linear_filter;
        gfx_opengl_sync_three_point();
    }
    const float aniso = gfx_opengl_anisotropy();
    glActiveTexture(GL_TEXTURE0 + tile);
    if (aniso > 1.0f) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
    } else if (gl_max_anisotropy > 1.0f) {
        /* Anisotropy is per texture object and survives a recycled name, so OFF
         * has to be written back or the old 2x-16x stays */
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
    }
    if (aniso > 1.0f && filter == GL_LINEAR) {
        /* Mips exist (generated at upload when aniso is on) and trilinear
         * minification is what the anisotropic samples run over. If aniso was
         * turned on after this texture's upload, build the chain here or the
         * texture is incomplete. */
        const GLuint id = current_texture_ids[tile];
        if (!gl_has_mips(id)) {
            glGenerateMipmap(GL_TEXTURE_2D);
            gl_set_has_mips(id, 1);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gfx_cm_to_opengl(cms));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gfx_cm_to_opengl(cmt));
}

/* ---- Depth, blend, viewport ---- */

/* ZMODE_DEC (decals: skid marks, shadows on the road): fast3d's one-sided LEQUAL
 * plus a polygon offset toward the camera. The stock -2,-2 is not enough when
 * the scene sits compressed at the far end of the depth range.
 * DD64_DEC_ZBIAS=factor,units overrides the default -4,-8. */
static float dec_bias_f = -4.0f, dec_bias_u = -8.0f;
static void read_dec_bias(void) {
    static bool done = false;
    if (done) {
        return;
    }
    done = true;
    const char* e = getenv("DD64_DEC_ZBIAS");
    if (e != NULL) {
        sscanf(e, "%f,%f", &dec_bias_f, &dec_bias_u);
    }
}

static void gfx_opengl_set_depth_mode(bool depth_test, bool depth_update, bool depth_compare, bool depth_source_prim, uint16_t zmode) {
    read_dec_bias();
    if (depth_test) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(depth_update ? GL_TRUE : GL_FALSE);
        current_depth_mask = depth_update;

        if (depth_compare) {
            switch (zmode) {
                case ZMODE_INTER:
                    glDepthFunc(GL_LEQUAL);
                    glDisable(GL_POLYGON_OFFSET_FILL);
                    glPolygonOffset(0, 0);
                    break;

                case ZMODE_OPA:
                case ZMODE_XLU:
                    if (depth_source_prim) {
                        glDepthFunc(GL_LEQUAL);
                    } else {
                        glDepthFunc(GL_LESS);
                    }
                    glDisable(GL_POLYGON_OFFSET_FILL);
                    glPolygonOffset(0, 0);
                    break;

                case ZMODE_DEC:
                    glDepthFunc(GL_LEQUAL);
                    glEnable(GL_POLYGON_OFFSET_FILL);
                    glPolygonOffset(dec_bias_f, dec_bias_u);
                    break;
            }
        } else {
            glDepthFunc(GL_ALWAYS);
            glDisable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(0, 0);
        }
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

static void gl_set_depth_range(float znear, float zfar) {
    /* Far-plane geometry is depth-clamped (GL_DEPTH_CLAMP, no far clip, as the
     * RSP). Clamped fragments land on the range's far value; the buffer clears
     * to 1.0 and opaque tests GL_LESS, so keep the far end a hair inside 1.0 or
     * every clamped far triangle fails the test. */
    if (zfar > 0.9999995f) zfar = 0.9999995f;
    if (glDepthRangef) {
        glDepthRangef(znear, zfar);
    } else {
        glDepthRange(znear, zfar);
    }
}

static void gfx_opengl_set_viewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

static void gfx_opengl_set_scissor(int x, int y, int width, int height) {
    glScissor(x, y, width, height);
}

static void gfx_opengl_set_use_alpha(bool use_alpha, bool modulate) {
    if (use_alpha) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
    if (modulate) {
        glBlendFunc(GL_DST_COLOR, GL_ZERO);
    } else {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

/*
 * The vertex stream: each batch is appended to one large buffer and drawn from
 * where it landed (at a whole vertex, as the attributes start at 0), and the
 * buffer is given fresh storage when full. A frame is ~1300 small batches;
 * glBufferData on each made the driver allocate storage every time.
 */
#define STREAM_VBO_BYTES (8u * 1024 * 1024)
static size_t stream_vbo_off = STREAM_VBO_BYTES; /* full: the first batch allocates it */

static void gfx_opengl_draw_triangles(float buf_vbo[], size_t buf_vbo_len, size_t buf_vbo_num_tris) {
    size_t bytes = sizeof(float) * buf_vbo_len, verts = 3 * buf_vbo_num_tris;
    size_t stride = bytes / verts, first = (stream_vbo_off + stride - 1) / stride, off = first * stride;

    if (bytes > STREAM_VBO_BYTES) { /* (never in practice) */
        glBufferData(GL_ARRAY_BUFFER, bytes, buf_vbo, GL_STREAM_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, verts);
        stream_vbo_off = STREAM_VBO_BYTES;
        return;
    }
    if (off + bytes > STREAM_VBO_BYTES) {
        glBufferData(GL_ARRAY_BUFFER, STREAM_VBO_BYTES, NULL, GL_STREAM_DRAW);
        first = 0;
        off = 0;
    }
    glBufferSubData(GL_ARRAY_BUFFER, off, bytes, buf_vbo);
    glDrawArrays(GL_TRIANGLES, (GLint)first, (GLsizei)verts);
    stream_vbo_off = off + bytes;
}

/* The interpolated replay: the frame's vertices as one fresh buffer, then its draws */
static void gfx_opengl_upload_vertices(const float *data, size_t num_floats) {
    glBufferData(GL_ARRAY_BUFFER, num_floats * sizeof(float), data, GL_STREAM_DRAW);
    stream_vbo_off = STREAM_VBO_BYTES; /* the next draw_triangles starts a new stream */
}

static void gfx_opengl_draw_uploaded(size_t first_vertex, size_t num_tris) {
    glDrawArrays(GL_TRIANGLES, (GLint)first_vertex, (GLsizei)(3 * num_tris));
}

/* ---- Initialisation ---- */

static void APIENTRY gl_debug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *p) {
    sysLogPrintf(LOG_WARNING, "GL: (%05x) %s", id, msg);
}

static void gfx_opengl_enable_debug(void) {
    if (GLAD_GL_KHR_debug) {
        glEnable(GL_DEBUG_OUTPUT);
    }
    if (glDebugMessageControl != NULL) {
        // enable everything except some specific spam messages
        const GLuint disable[] = {
            0x20061, /* "Framebuffer detailed info" */
            0x20071  /* "Buffer detailed info" */
        };
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 2, disable, GL_FALSE);
    }
    if (glDebugMessageCallback != NULL) {
        glDebugMessageCallback(gl_debug, NULL);
    }
}

static bool gfx_opengl_supports_framebuffers(void) {
    if (GLVersion.major > 2) {
        // GL3.0+ supports everything we need, but we'll still check it for sanity
        return (glad_glFramebufferRenderbuffer && glad_glBlitFramebuffer && glad_glRenderbufferStorageMultisample);
    }
    if (GLAD_GL_ARB_framebuffer_object) {
        // some implementations might be missing these
        return (glad_glBlitFramebuffer && glad_glRenderbufferStorageMultisample);
    }
    if (GLAD_GL_EXT_framebuffer_object && GLAD_GL_EXT_framebuffer_blit && GLAD_GL_EXT_framebuffer_multisample) {
        // sanity check
        return (glad_glFramebufferRenderbuffer && glad_glBlitFramebuffer && glad_glRenderbufferStorageMultisample);
    }
    // nothing
    return false;
}

static bool gfx_opengl_supports_shaders(void) {
    if (GLVersion.major > 2) {
        // should support GLSL130+
        return true;
    }

    // check supported GLSL version
    const char *ver = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (ver) {
        int maj = 0, min = 0;
        sscanf(ver, "%d.%d", &maj, &min);
        if (maj > 1 || (maj == 1 && min > 20)) {
            // above 120, should be fine
            return true;
        }
    }

    // check for extension that adds textureSize
    return GLAD_GL_EXT_gpu_shader4;
}

static void gfx_opengl_log_info(void) {
    const char *version = (const char *)glGetString(GL_VERSION);
    const char *vendor = (const char *)glGetString(GL_VENDOR);
    const char *renderer = (const char *)glGetString(GL_RENDERER);
    const char *glsl_version = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    sysLogPrintf(LOG_NOTE, "GL: version: %s", version ? version : "unknown");
    sysLogPrintf(LOG_NOTE, "GL: vendor: %s", vendor ? vendor : "unknown");
    sysLogPrintf(LOG_NOTE, "GL: renderer: %s", renderer ? renderer : "unknown");
    /* no GPU driver at all: Windows' GL 1.1 fallback, or a CPU rasteriser */
    if (renderer && (strstr(renderer, "GDI Generic") || strstr(renderer, "llvmpipe") ||
                     strstr(renderer, "softpipe") || strstr(renderer, "Microsoft Basic Render"))) {
        sysLogPrintf(LOG_WARNING, "GL: WARNING: '%s' is a SOFTWARE renderer - no GPU driver is serving OpenGL", renderer);
    }
    sysLogPrintf(LOG_NOTE, "GL: GLSL version: %s", glsl_version ? glsl_version : "unknown");
    sysLogPrintf(LOG_NOTE, "GL: ARB_framebuffer_object: %s", gfx_opengl_supports_framebuffers() ? "yes" : "no");
    sysLogPrintf(LOG_NOTE, "GL: ARB_depth_clamp: %s", GLAD_GL_ARB_depth_clamp ? "yes" : "no");
    sysLogPrintf(LOG_NOTE, "GL: ARB_texture_mirror_clamp_to_edge: %s", GLAD_GL_ARB_texture_mirror_clamp_to_edge ? "yes" : "no");
}

static void *gl_load_proc(const char *name) {
    void *ret = (void *)SDL_GL_GetProcAddress(name);
    if (ret) {
        return ret;
    }

    // try with postfixes
    static const char *post[] = { "ARB", "EXT" };
    char tmp[256] = { 0 };
    for (size_t i = 0; i < sizeof(post) / sizeof(*post); ++i) {
        snprintf(tmp, sizeof(tmp), "%s%s", name, post[i]);
        ret = (void *)SDL_GL_GetProcAddress(tmp);
        if (ret) {
            return ret;
        }
    }

    sysLogPrintf(LOG_ERROR, "GL: could not find function: %s", name);

    return NULL;
}

static void gfx_opengl_init_extensions(void) {
    // patch up some extension values and pointers
    if (!GLAD_GL_ARB_depth_clamp) {
        if (GLAD_GL_EXT_depth_clamp || GLAD_GL_NV_depth_clamp) {
            GLAD_GL_ARB_depth_clamp = 1;
        } else if (!gl_es && GLVersion.major >= 3) {
            // GL3.2+ should have depth_clamp as part of the spec, but some devices don't report it for some reason
            GLAD_GL_ARB_depth_clamp = (GLVersion.major > 3 || (GLVersion.major == 3 && GLVersion.minor >= 2));
        }
    }

    if (!GLAD_GL_ARB_texture_mirror_clamp_to_edge) {
        GLAD_GL_ARB_texture_mirror_clamp_to_edge = GLAD_GL_EXT_texture_mirror_clamp_to_edge;
    }

    if (GLVersion.major < 3 && !GLAD_GL_ARB_framebuffer_object) {
        if (GLAD_GL_EXT_framebuffer_object && GLAD_GL_EXT_framebuffer_blit && GLAD_GL_EXT_framebuffer_multisample) {
            // because of the way glad works we'll have to copy the pointers over
            glad_glGenFramebuffers = glad_glGenFramebuffersEXT;
            glad_glGenRenderbuffers = glad_glGenRenderbuffersEXT;
            glad_glDeleteFramebuffers = glad_glDeleteFramebuffersEXT;
            glad_glDeleteRenderbuffers = glad_glDeleteRenderbuffersEXT;
            glad_glBindFramebuffer = glad_glBindFramebufferEXT;
            glad_glBindRenderbuffer = glad_glBindRenderbufferEXT;
            glad_glFramebufferRenderbuffer = glad_glFramebufferRenderbufferEXT;
            glad_glFramebufferTexture2D = glad_glFramebufferTexture2DEXT;
            glad_glRenderbufferStorage = glad_glRenderbufferStorageEXT;
            glad_glRenderbufferStorageMultisample = glad_glRenderbufferStorageMultisampleEXT;
            glad_glBlitFramebuffer = glad_glBlitFramebufferEXT;
        }
    }
}

static void gfx_opengl_init(void) {
    if (!gladLoadGLLoader(gl_load_proc) || glGetString == NULL || glEnable == NULL) {
        sysFatalError("Could not load OpenGL.\nReported SDL error: %s", SDL_GetError());
    }

    // check if we're using ES or core, which have more limited feature sets
    int val = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &val);
    gl_core_profile = (val == SDL_GL_CONTEXT_PROFILE_CORE);
    gl_es = (val == SDL_GL_CONTEXT_PROFILE_ES);

    gfx_opengl_init_extensions();

    if (sysArgCheck("--debug-gl") || getenv("DD64_DEBUG_GL") != NULL) {
        gfx_opengl_enable_debug();
    }
    // dump version info as early as possible, always: a user's log has to
    // name the GPU (iGPU vs dGPU is the first question on any fps report)
    gfx_opengl_log_info();

    if (GLVersion.major < 2 || (GLVersion.major == 2 && GLVersion.minor < 1)) {
        const char *ver = (const char *)glGetString(GL_VERSION);
        sysFatalError("Could not load OpenGL 2.1.\nReported version: %d.%d (%s)",
            GLVersion.major, GLVersion.minor, ver ? ver : "unknown");
    }

    if (!gfx_opengl_supports_shaders()) {
        sysLogPrintf(LOG_WARNING, "GL: GLSL 1.30 may be unsupported");
        // maybe replace this with sysFatalError, though the GLSL compiler will cause that later anyway
    }

    if (!gfx_framebuffers_enabled) {
        sysLogPrintf(LOG_WARNING, "GL: framebuffer effects disabled by user");
    } else if (!gfx_opengl_supports_framebuffers()) {
        sysLogPrintf(LOG_WARNING, "GL: GL_ARB_framebuffer_object unsupported, framebuffer effects disabled");
        gfx_framebuffers_enabled = false;
    }

    if ((GLVersion.major < 4 || GLVersion.minor < 4) && !GLAD_GL_ARB_texture_mirror_clamp_to_edge) {
        // GL_MIRROR_CLAMP_TO_EDGE unsupported
        gl_mirror_clamp = GL_MIRRORED_REPEAT;
    }

    // determine GLSL version
    if (gl_es) {
        // ES has its own numbering scheme, but it should support 300 even on 3.1 and 3.2
        gl_glsl_version = 300;
        snprintf(gl_glsl_version_str, sizeof(gl_glsl_version_str), "%d es", gl_glsl_version);
    } else if (!gl_core_profile) {
        // in compat profile we can just request the lowest possible
        gl_glsl_version = 130;
        snprintf(gl_glsl_version_str, sizeof(gl_glsl_version_str), "%d", gl_glsl_version);
    } else {
        // otherwise we have to pick a specific version
        if (GLVersion.major == 3 && GLVersion.minor == 2) {
            // 3.2core is the earliest core version and it follows the old numbering scheme
            gl_glsl_version = 150;
        } else {
            // 3.3+ follow the new numbering scheme
            gl_glsl_version = GLVersion.major * 100 + GLVersion.minor * 10;
        }
        snprintf(gl_glsl_version_str, sizeof(gl_glsl_version_str), "%d core", gl_glsl_version);
    }
    sysLogPrintf(LOG_NOTE, "GL: using GLSL version %s", gl_glsl_version_str);

    glGenBuffers(1, &opengl_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, opengl_vbo);

    if (gl_core_profile || gl_es) {
        // warn user that funny things can happen
        sysLogPrintf(LOG_WARNING, "GL: using core profile or ES, watch out for errors");
        // core/es will explode if we don't use a VAO for our VBO
        glGenVertexArrays(1, &opengl_vao);
        glBindVertexArray(opengl_vao);
    }

    if (GLAD_GL_ARB_depth_clamp) {
        glEnable(GL_DEPTH_CLAMP);
    }
    gl_set_depth_range(0.0f, 1.0f);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    framebuffers.resize(1); // for the default screen buffer
}

static void gfx_opengl_start_frame(void) {
    frame_count++;
}

static void gfx_opengl_end_frame(void) {
    glFlush();
}

/* ---- Framebuffers ---- */

/* Allocate a plain colour texture's storage without disturbing the texture
 * the interpreter has bound on the active unit (it tracks its bindings itself) */
static void gl_alloc_rgb8(GLuint tex, uint32_t width, uint32_t height) {
    GLint had_tex = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &had_tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, (GLuint)had_tex);
}

static int gfx_opengl_create_framebuffer() {
    size_t i = framebuffers.size();
    framebuffers.resize(i + 1);

    GLuint clrbuf;
    GLint had_tex = 0;
    glGenTextures(1, &clrbuf);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &had_tex);
    glBindTexture(GL_TEXTURE_2D, clrbuf);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, (GLuint)had_tex);
    framebuffers[i].clrbuf = clrbuf;
    framebuffers[i].clrtex_msaa = 0;
    framebuffers[i].msaa_is_texture = false;

    if (!gfx_framebuffers_enabled) {
        return i;
    }

    GLuint clrbuf_msaa;
    glGenRenderbuffers(1, &clrbuf_msaa);
    framebuffers[i].clrbuf_msaa = clrbuf_msaa;

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1, 1);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    framebuffers[i].rbo = rbo;

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    framebuffers[i].fbo = fbo;

    return i;
}

static void gfx_opengl_update_framebuffer_parameters(int fb_id, uint32_t width, uint32_t height, uint32_t msaa_level,
                                                     bool has_depth_buffer) {
    Framebuffer& fb = framebuffers[fb_id];
    /* The GPU's sample limit, lowered when a driver rejects a level, so an MSAA
     * pick it cannot do falls back instead of leaving a black screen. */
    static GLint msaa_cap = -1;
    static bool msaa_tex_ok = true; /* cleared when N64 AA's multisample texture is rejected */
    bool realloc = false;

    width = max(width, 1U);
    height = max(height, 1U);

    if (gfx_framebuffers_enabled && fb_id != 0 && msaa_level > 1) {
        if (msaa_cap < 0) {
            GLint s = 0;
            glGetIntegerv(GL_MAX_SAMPLES, &s);
            if (glTexImage2DMultisample != NULL) {
                GLint t = 0;
                glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &t);
                if (t > 0 && t < s) {
                    s = t;
                }
            }
            msaa_cap = max(s, 1);
            sysLogPrintf(LOG_NOTE, "GL: up to %dx MSAA", (int)msaa_cap);
        }
        while (msaa_level > 1 && msaa_level > (uint32_t)msaa_cap) {
            msaa_level >>= 1;
        }
    }

    if (gfx_framebuffers_enabled) {
        glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);

        if (fb_id != 0) {
            /* N64 AA: the VI resolve reads samples, so the multisampled colour
             * target is a texture while gfx_n64_aa is set (a renderbuffer otherwise) */
            const bool want_tex = msaa_level > 1 && gfx_n64_aa != 0 && glTexImage2DMultisample != NULL && msaa_tex_ok;
            if (fb.width != width || fb.height != height || fb.msaa_level != msaa_level || fb.msaa_is_texture != want_tex) {
                realloc = true;
                if (msaa_level <= 1) {
                    gl_alloc_rgb8(fb.clrbuf, width, height);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb.clrbuf, 0);
                } else if (want_tex) {
                    if (fb.clrtex_msaa == 0) {
                        glGenTextures(1, &fb.clrtex_msaa);
                    }
                    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fb.clrtex_msaa);
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaa_level, GL_RGB8, width, height, GL_TRUE);
                    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fb.clrtex_msaa, 0);
                } else {
                    glBindRenderbuffer(GL_RENDERBUFFER, fb.clrbuf_msaa);
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa_level, GL_RGB8, width, height);
                    glBindRenderbuffer(GL_RENDERBUFFER, 0);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, fb.clrbuf_msaa);
                }
                if (!want_tex && fb.clrtex_msaa != 0) {
                    /* not attached any more (N64 AA off, or its texture rejected) */
                    glDeleteTextures(1, &fb.clrtex_msaa);
                    fb.clrtex_msaa = 0;
                }
                fb.msaa_is_texture = want_tex;
            }

            if (has_depth_buffer &&
                (fb.width != width || fb.height != height || fb.msaa_level != msaa_level || !fb.has_depth_buffer)) {
                realloc = true;
                glBindRenderbuffer(GL_RENDERBUFFER, fb.rbo);
                if (msaa_level <= 1) {
                    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
                } else {
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa_level, GL_DEPTH24_STENCIL8, width, height);
                }
                glBindRenderbuffer(GL_RENDERBUFFER, 0);
            }

            if (!fb.has_depth_buffer && has_depth_buffer) {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb.rbo);
            } else if (fb.has_depth_buffer && !has_depth_buffer) {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
            }

            if (realloc && msaa_level > 1) {
                const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                if (status != GL_FRAMEBUFFER_COMPLETE) {
                    /* record what is attached now, so the retry reallocates everything */
                    fb.width = width;
                    fb.height = height;
                    fb.has_depth_buffer = has_depth_buffer;
                    fb.msaa_level = msaa_level;
                    if (fb.msaa_is_texture) {
                        /* N64 AA's multisample texture next to the multisample depth
                         * renderbuffer: keep the level with a renderbuffer instead
                         * (N64 AA then uses the plain resolve) */
                        sysLogPrintf(LOG_WARNING, "GL: %ux MSAA texture framebuffer rejected (0x%x), "
                                     "N64 AA falls back to the plain resolve", msaa_level, (unsigned)status);
                        msaa_tex_ok = false;
                        gfx_opengl_update_framebuffer_parameters(fb_id, width, height, msaa_level, has_depth_buffer);
                        return;
                    }
                    sysLogPrintf(LOG_WARNING, "GL: %ux MSAA framebuffer rejected (0x%x), trying %ux", msaa_level,
                                 (unsigned)status, msaa_level >> 1);
                    msaa_cap = (GLint)(msaa_level >> 1);
                    gfx_opengl_update_framebuffer_parameters(fb_id, width, height, msaa_level >> 1, has_depth_buffer);
                    return;
                }
            }
        }
    } else {
        has_depth_buffer = false;
    }

    fb.width = width;
    fb.height = height;
    fb.has_depth_buffer = has_depth_buffer;
    fb.msaa_level = msaa_level;
}

static bool gfx_opengl_start_draw_to_framebuffer(int fb_id, float noise_scale) {
    if (gfx_framebuffers_enabled && fb_id < (int)framebuffers.size()) {
        Framebuffer& fb = framebuffers[fb_id];
        if (noise_scale != 0.0f) {
            current_noise_scale = 1.0f / noise_scale;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
        current_framebuffer = fb_id;
        return true;
    } else {
        return false;
    }
}

static void gfx_opengl_clear_framebuffer(bool clear_color, bool clear_depth) {
    glDisable(GL_SCISSOR_TEST);

    GLbitfield mask = 0;
    if (clear_color) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        mask |= GL_COLOR_BUFFER_BIT;
    }
    if (clear_depth) {
        glDepthMask(GL_TRUE);
        mask |= GL_DEPTH_BUFFER_BIT;
    }
    glClear(mask);
    if (clear_depth) {
        glDepthMask(current_depth_mask ? GL_TRUE : GL_FALSE);
    }

    glEnable(GL_SCISSOR_TEST);
}

/* The window rect a present into framebuffer 0 lands in (gfx_present: 4:3 in
 * the middle, or the whole window) - x and width; 0 width = the whole window */
int g_GfxPresentX = 0;
int g_GfxPresentW = 0;

static void gfx_opengl_present_x(int fb_id, GLint full_w, GLint* x0, GLint* x1) {
    if (fb_id == 0 && g_GfxPresentW > 0) {
        *x0 = (GLint) g_GfxPresentX;
        *x1 = (GLint) (g_GfxPresentX + g_GfxPresentW);
    } else {
        *x0 = 0;
        *x1 = full_w;
    }
}

/* Blit the bound READ framebuffer's (0,0)-(sw,sh) to the rect (dx0,dy0)-(dx1,dy1)
 * of draw framebuffer `dst_fbo`. A driver's "override" antialiasing makes the
 * window multisampled, and GL refuses a scaled blit into it, which left the
 * picture black. Into such a window a scaled blit goes through a plain texture
 * of the rect's size first, so the last step is a same-size copy. */
static void gl_blit_to(GLuint dst_fbo, GLint sw, GLint sh, GLint dx0, GLint dy0, GLint dx1, GLint dy1, GLenum filter) {
    static int win_ms = -1;
    static GLuint ws_fbo = 0, ws_tex = 0;
    static GLint ws_w = 0, ws_h = 0;
    const GLint rw = dx1 - dx0, rh = dy1 - dy0;

    if (win_ms < 0) {
        GLint read_fbo = 0, sb = 0, samples = 0;
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glGetIntegerv(GL_SAMPLE_BUFFERS, &sb);
        glGetIntegerv(GL_SAMPLES, &samples);
        win_ms = (sb > 0) ? 1 : 0;
        if (win_ms) {
            sysLogPrintf(LOG_WARNING, "GL: the window is multisampled (%d samples, a driver antialiasing override?); "
                         "scaled frames reach it through a plain texture", (int)samples);
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, (GLuint)read_fbo);
    }
    if (dst_fbo != 0 || !win_ms || (rw == sw && rh == sh) || rw <= 0 || rh <= 0) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst_fbo);
        glBlitFramebuffer(0, 0, sw, sh, dx0, dy0, dx1, dy1, GL_COLOR_BUFFER_BIT, filter);
        return;
    }
    if (ws_fbo == 0) {
        glGenFramebuffers(1, &ws_fbo);
        glGenTextures(1, &ws_tex);
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ws_fbo);
    if (ws_w != rw || ws_h != rh) {
        gl_alloc_rgb8(ws_tex, rw, rh);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ws_tex, 0);
        ws_w = rw;
        ws_h = rh;
    }
    glBlitFramebuffer(0, 0, sw, sh, 0, 0, rw, rh, GL_COLOR_BUFFER_BIT, filter);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, ws_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, rw, rh, dx0, dy0, dx1, dy1, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

static void gfx_opengl_resolve_msaa_color_buffer(int fb_id_target, int fb_id_source) {
    if (!gfx_framebuffers_enabled) {
        return;
    }

    Framebuffer& fb_dst = framebuffers[fb_id_target];
    Framebuffer& fb_src = framebuffers[fb_id_source];
    GLint dx0, dx1;

    gfx_opengl_present_x(fb_id_target, (GLint) fb_dst.width, &dx0, &dx1);
    glDisable(GL_SCISSOR_TEST);
    if (fb_src.msaa_level > 1 && (fb_id_target == 0 || fb_src.width != fb_dst.width || fb_src.height != fb_dst.height)) {
        /* A multisample blit needs identical rects (and, on strict drivers, formats),
         * so the window (RGBA, maybe pillarboxed) went black on AMD/Intel. Resolve at
         * the source size into a plain RGB8 texture first, then blit that. */
        static GLuint rs_fbo = 0, rs_tex = 0;
        static uint32_t rs_w = 0, rs_h = 0;
        if (rs_fbo == 0) {
            glGenFramebuffers(1, &rs_fbo);
            glGenTextures(1, &rs_tex);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, rs_fbo);
        if (rs_w != fb_src.width || rs_h != fb_src.height) {
            gl_alloc_rgb8(rs_tex, fb_src.width, fb_src.height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rs_tex, 0);
            rs_w = fb_src.width;
            rs_h = fb_src.height;
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_src.fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rs_fbo);
        glBlitFramebuffer(0, 0, fb_src.width, fb_src.height, 0, 0, fb_src.width, fb_src.height, GL_COLOR_BUFFER_BIT,
                          GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, rs_fbo);
    } else {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_src.fbo);
    }
    gl_blit_to(fb_dst.fbo, fb_src.width, fb_src.height, dx0, 0, dx1, fb_dst.height, GL_NEAREST);
    gl_bind_current_framebuffer();
    glEnable(GL_SCISSOR_TEST);
}

/*
 * Draw a number (the FPS counter) in the top-left corner of the window's back
 * buffer, in a 3x5 pixel font scaled to the window, as scissored clears: a dark
 * box with a one-pixel margin round the digits.
 */
extern "C" void gfx_opengl_draw_counter(int value) {
    static const uint16_t font[10] = {
        /* 3x5 digits, 15 bits each, row-major from the top */
        0x7B6F, 0x2C97, 0x73E7, 0x73CF, 0x5BC9, 0x79CF, 0x79EF, 0x7249, 0x7BEF, 0x7BCF,
    };
    char text[12];
    GLint vp[4];
    int h, scale, n, i, r, c;

    glGetIntegerv(GL_VIEWPORT, vp);
    h = framebuffers.empty() ? vp[3] : (int)framebuffers[0].height;
    scale = h / 240 > 1 ? h / 240 : 1;
    n = snprintf(text, sizeof(text), "%d", value < 0 ? 0 : value);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_SCISSOR_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    /* a dark backing box (2 pixels from the corner, 7 high), then the digits */
    glScissor(2 * scale, h - 2 * scale - 7 * scale, (4 * n + 1) * scale, 7 * scale);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 0.2f, 1.0f);
    for (i = 0; i < n; i++) {
        uint16_t bits = font[text[i] - '0'];
        for (r = 0; r < 5; r++) {
            for (c = 0; c < 3; c++) {
                if (bits & (1 << (14 - (r * 3 + c)))) {
                    glScissor((3 + 4 * i + c) * scale, h - (4 + r) * scale, scale, scale);
                    glClear(GL_COLOR_BUFFER_BIT);
                }
            }
        }
    }
    gl_bind_current_framebuffer();
}

/* Save the window's back buffer as a BMP (before the swap) */
extern "C" void gfx_opengl_screenshot(const char* path) {
    GLint vp[4];
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glGetIntegerv(GL_VIEWPORT, vp);
    int w = framebuffers.empty() ? vp[2] : (int)framebuffers[0].width;
    int h = framebuffers.empty() ? vp[3] : (int)framebuffers[0].height;
    std::vector<uint8_t> px((size_t)w * h * 4), flipped((size_t)w * h * 4);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, px.data());
    for (int y = 0; y < h; y++) {
        memcpy(&flipped[(size_t)y * w * 4], &px[(size_t)(h - 1 - y) * w * 4], (size_t)w * 4);
    }
    SDL_Surface* s = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_ABGR8888, flipped.data(), w * 4);
    if (s != NULL) {
        SDL_SaveBMP(s, path);
        SDL_DestroySurface(s);
    }
    gl_bind_current_framebuffer();
}

static void gfx_opengl_set_texture_filter(FilteringMode mode) {
    current_filter_mode = mode;
}

/* ==== N64 AA: a Video-Interface-style resolve ==============================
 *
 * The N64 anti-aliased in two places: the RDP blended silhouette pixels by
 * their subpixel COVERAGE (our MSAA stands in for that), then the VI
 * post-filtered the frame on scan-out, blending each partially covered pixel
 * with its 3x3 neighbourhood by the coverage it lacked and resampling the
 * 320x240 image softly to the TV. This approximates that second half on the
 * multisampled game framebuffer:
 *   pass 1 (src size): per pixel, the mean of the samples (the RDP blend) and
 *          a coverage = the size of the largest cluster of equal samples /
 *          the sample count (MSAA shades a triangle once per pixel, so
 *          samples differ only where two triangles meet - exactly an edge);
 *   pass 2 (src size): VI blend = mix(pixel, 3x3 mean, 1 - coverage);
 *   present: a LINEAR blit to the destination (the VI's soft resample).
 * Alpha-cutout edges stay hard, as they did on the console. The game
 * framebuffer must be a multisample TEXTURE for pass 1 to read samples:
 * gfx_opengl_update_framebuffer_parameters allocates one while gfx_n64_aa is
 * set (needs ARB_texture_multisample, desktop GL; without it the plain
 * resolve runs and a warning names it).
 * DD64_N64AA_STRENGTH (default 1.0) scales the edge blend; DD64_N64AA_SOFT=0
 * presents with nearest instead of linear filtering. */
static GLuint vi_prog[2] = { 0, 0 };
static GLint vi_u_src[2] = { -1, -1 }, vi_u_samples = -1, vi_u_size = -1, vi_u_strength = -1;
static GLuint vi_tex[2] = { 0, 0 }, vi_fbo[2] = { 0, 0 };
static uint32_t vi_w = 0, vi_h = 0;
static GLuint vi_vbo = 0, vi_vao = 0;
static bool vi_failed = false;

static GLuint vi_compile(const char* vs, const char* fs) {
    GLuint v = glCreateShader(GL_VERTEX_SHADER), f = glCreateShader(GL_FRAGMENT_SHADER);
    GLint ok;
    char log[1024];

    glShaderSource(v, 1, &vs, NULL);
    glCompileShader(v);
    glGetShaderiv(v, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(v, sizeof(log), NULL, log);
        sysLogPrintf(LOG_ERROR, "N64 AA vertex shader: %s", log);
        glDeleteShader(v);
        glDeleteShader(f);
        return 0;
    }
    glShaderSource(f, 1, &fs, NULL);
    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(f, sizeof(log), NULL, log);
        sysLogPrintf(LOG_ERROR, "N64 AA fragment shader: %s\n%s", log, fs);
        glDeleteShader(v);
        glDeleteShader(f);
        return 0;
    }
    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    if (gl_glsl_version < 130) {
        glBindAttribLocation(p, 0, "aPos");
    }
    glLinkProgram(p);
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    glDetachShader(p, v);
    glDetachShader(p, f);
    glDeleteShader(v);
    glDeleteShader(f);
    if (!ok) {
        glGetProgramInfoLog(p, sizeof(log), NULL, log);
        sysLogPrintf(LOG_ERROR, "N64 AA link: %s", log);
        glDeleteProgram(p);
        return 0;
    }
    return p;
}

static bool vi_init(void) {
    if (vi_prog[0] != 0) return true;
    if (vi_failed) return false;
    if (gl_es || glTexImage2DMultisample == NULL || glGenVertexArrays == NULL) {
        /* GLSL ES 3.00 has no sampler2DMS */
        sysLogPrintf(LOG_WARNING, "N64 AA: multisample textures / VAOs unavailable on this GL - plain MSAA resolve instead");
        vi_failed = true;
        return false;
    }
    char hdr[256];
    snprintf(hdr, sizeof(hdr), "#version %s\n#extension GL_ARB_texture_multisample : enable\n", gl_glsl_version_str);
    std::string vs = std::string(hdr) +
        "in vec2 aPos;\n"
        "void main() { gl_Position = vec4(aPos, 0.0, 1.0); }\n";
    std::string fs1 = std::string(hdr) +
        "uniform sampler2DMS uSrc;\n"
        "uniform int uSamples;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "    ivec2 p = ivec2(gl_FragCoord.xy);\n"
        "    vec3 c0 = texelFetch(uSrc, p, 0).rgb;\n"
        "    vec3 sum = c0;\n"
        "    int same = 1;\n"
        "    for (int i = 1; i < uSamples; i++) {\n"
        "        vec3 c = texelFetch(uSrc, p, i).rgb;\n"
        "        sum += c;\n"
        "        if (all(lessThan(abs(c - c0), vec3(1.5 / 255.0)))) same++;\n"
        "    }\n"
        "    float cov = float(max(same, uSamples - same)) / float(uSamples);\n"
        "    outColor = vec4(sum / float(uSamples), cov);\n"
        "}\n";
    std::string fs2 = std::string(hdr) +
        "uniform sampler2D uTex;\n"
        "uniform ivec2 uSize;\n"
        "uniform float uStrength;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "    ivec2 p = ivec2(gl_FragCoord.xy);\n"
        "    vec4 c = texelFetch(uTex, p, 0);\n"
        "    vec3 acc = vec3(0.0);\n"
        "    for (int dy = -1; dy <= 1; dy++) {\n"
        "        for (int dx = -1; dx <= 1; dx++) {\n"
        "            ivec2 q = clamp(p + ivec2(dx, dy), ivec2(0), uSize - ivec2(1));\n"
        "            acc += texelFetch(uTex, q, 0).rgb;\n"
        "        }\n"
        "    }\n"
        "    float w = clamp((1.0 - c.a) * uStrength, 0.0, 1.0);\n"
        "    outColor = vec4(mix(c.rgb, acc / 9.0, w), 1.0);\n"
        "}\n";
    vi_prog[0] = vi_compile(vs.c_str(), fs1.c_str());
    vi_prog[1] = vi_prog[0] ? vi_compile(vs.c_str(), fs2.c_str()) : 0;
    if (vi_prog[0] == 0 || vi_prog[1] == 0) {
        if (vi_prog[0]) glDeleteProgram(vi_prog[0]);
        vi_prog[0] = vi_prog[1] = 0;
        vi_failed = true;
        return false;
    }
    vi_u_src[0] = glGetUniformLocation(vi_prog[0], "uSrc");
    vi_u_samples = glGetUniformLocation(vi_prog[0], "uSamples");
    vi_u_src[1] = glGetUniformLocation(vi_prog[1], "uTex");
    vi_u_size = glGetUniformLocation(vi_prog[1], "uSize");
    vi_u_strength = glGetUniformLocation(vi_prog[1], "uStrength");
    static const float tri[6] = { -1.0f, -1.0f, 3.0f, -1.0f, -1.0f, 3.0f };
    glGenBuffers(1, &vi_vbo);
    glGenVertexArrays(1, &vi_vao);
    glBindVertexArray(vi_vao);
    glBindBuffer(GL_ARRAY_BUFFER, vi_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);
    GLint loc = glGetAttribLocation(vi_prog[0], "aPos");
    if (loc < 0) loc = 0;
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindVertexArray((gl_core_profile || gl_es) ? opengl_vao : 0);
    glBindBuffer(GL_ARRAY_BUFFER, opengl_vbo);
    glGenTextures(2, vi_tex);
    glGenFramebuffers(2, vi_fbo);
    sysLogPrintf(LOG_NOTE, "N64 AA: VI resolve ready");
    return true;
}

/* On texture unit 0 (the caller's): the passes' render targets at the frame size */
static void vi_size(uint32_t w, uint32_t h) {
    if (vi_w == w && vi_h == h) return;
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, vi_tex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindFramebuffer(GL_FRAMEBUFFER, vi_fbo[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vi_tex[i], 0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    vi_w = w; vi_h = h;
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

static void gfx_opengl_resolve_n64_aa(int fb_dst, int fb_src) {
    if (!gfx_framebuffers_enabled || fb_src <= 0 || fb_src >= (int)framebuffers.size() ||
        fb_dst < 0 || fb_dst >= (int)framebuffers.size()) {
        return;
    }
    Framebuffer& src = framebuffers[fb_src];
    Framebuffer& dst = framebuffers[fb_dst];
    /* attached, not merely allocated */
    if (src.msaa_level <= 1 || !src.msaa_is_texture || !vi_init()) {
        gfx_opengl_resolve_msaa_color_buffer(fb_dst, fb_src); /* the plain resolve */
        return;
    }
    /* the state the passes disturb */
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    const GLboolean had_depth = glIsEnabled(GL_DEPTH_TEST), had_blend = glIsEnabled(GL_BLEND),
                    had_scissor = glIsEnabled(GL_SCISSOR_TEST), had_cull = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glActiveTexture(GL_TEXTURE0); /* the passes bind on unit 0 only, restored below */
    vi_size(src.width, src.height);
    glViewport(0, 0, (GLsizei)src.width, (GLsizei)src.height);
    glBindVertexArray(vi_vao);
    /* pass 1: samples -> mean colour + coverage */
    glBindFramebuffer(GL_FRAMEBUFFER, vi_fbo[0]);
    glUseProgram(vi_prog[0]);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, src.clrtex_msaa);
    glUniform1i(vi_u_src[0], 0);
    glUniform1i(vi_u_samples, (GLint)src.msaa_level);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    /* pass 2: the VI's edge blend */
    glBindFramebuffer(GL_FRAMEBUFFER, vi_fbo[1]);
    glBindTexture(GL_TEXTURE_2D, vi_tex[0]);
    glUseProgram(vi_prog[1]);
    glUniform1i(vi_u_src[1], 0);
    glUniform2i(vi_u_size, (GLint)src.width, (GLint)src.height);
    glUniform1f(vi_u_strength, vi_strength());
    glDrawArrays(GL_TRIANGLES, 0, 3);
    /* present: the soft resample to the destination */
    glBindFramebuffer(GL_READ_FRAMEBUFFER, vi_fbo[1]);
    {
        static int soft = -1;
        GLint dx0, dx1;

        if (soft < 0) {
            const char* e = getenv("DD64_N64AA_SOFT");
            soft = (e && e[0] == '0') ? 0 : 1;
        }
        gfx_opengl_present_x(fb_dst, (GLint) dst.width, &dx0, &dx1);
        gl_blit_to(dst.fbo, (GLint)src.width, (GLint)src.height, dx0, 0, dx1, (GLint)dst.height,
                   soft ? GL_LINEAR : GL_NEAREST);
    }
    /* back to the interpreter's state */
    gl_bind_current_framebuffer();
    glBindVertexArray((gl_core_profile || gl_es) ? opengl_vao : 0);
    glBindBuffer(GL_ARRAY_BUFFER, opengl_vbo);
    glBindTexture(GL_TEXTURE_2D, current_texture_ids[0]);
    glActiveTexture(GL_TEXTURE0 + last_selected_tile);
    if (gl_current_prg != nullptr) {
        glUseProgram(gl_current_prg->opengl_program_id);
        gfx_opengl_vertex_array_set_attribs(gl_current_prg);
        gfx_opengl_set_uniforms(gl_current_prg);
    }
    glViewport(vp[0], vp[1], vp[2], vp[3]);
    if (had_depth) glEnable(GL_DEPTH_TEST);
    if (had_blend) glEnable(GL_BLEND);
    if (had_scissor) glEnable(GL_SCISSOR_TEST);
    if (had_cull) glEnable(GL_CULL_FACE);
    glDepthMask(current_depth_mask ? GL_TRUE : GL_FALSE);
}

struct GfxRenderingAPI gfx_opengl_api = {
    gfx_opengl_unload_shader,
    gfx_opengl_load_shader,
    gfx_opengl_create_and_load_new_shader,
    gfx_opengl_lookup_shader,
    gfx_opengl_new_texture,
    gfx_opengl_select_texture,
    gfx_opengl_upload_texture,
    gfx_opengl_set_sampler_parameters,
    gfx_opengl_set_depth_mode,
    gfx_opengl_set_viewport,
    gfx_opengl_set_scissor,
    gfx_opengl_set_use_alpha,
    gfx_opengl_draw_triangles,
    gfx_opengl_init,
    gfx_opengl_start_frame,
    gfx_opengl_end_frame,
    gfx_opengl_create_framebuffer,
    gfx_opengl_update_framebuffer_parameters,
    gfx_opengl_start_draw_to_framebuffer,
    gfx_opengl_clear_framebuffer,
    gfx_opengl_resolve_msaa_color_buffer,
    gfx_opengl_delete_texture,
    gfx_opengl_set_texture_filter,
    gfx_opengl_resolve_n64_aa,
    gfx_opengl_upload_vertices,
    gfx_opengl_draw_uploaded,
};
