/*
 * The N64 colour combiner as one program (the fragment half). The OpenGL
 * backend generates a shader for each combiner (gfx_opengl.cpp); this is the
 * same arithmetic with the combiner in uniforms, so it can be compiled ahead
 * of time (SPIR-V for the SDL_gpu backend, which has no shader compiler at run
 * time). It must give the generated shaders' pixels: the formulas and the
 * order of the steps follow gfx_opengl_create_and_load_new_shader.
 *
 * Uniforms (gfx_cc.h has the values):
 *   uCC[cycle * 2 + (0 rgb, 1 alpha)]: the a, b, c, d inputs, SHADER_* values
 *   uForm: for each uCC entry, the formula the generator writes for it:
 *          0 (a - b) * c + d, 1 d, 2 a * c, 3 mix(b, a, c)
 *   uOpt.x: the SHADER_OPT_* bits
 *   uOpt.y: 1 texel 0 used, 2 texel 1 used, 4 three-point filtering compiled
 *           in (the generator decides when it first sees the combiner),
 *           8 and 16 texel 0 and 1 three-point filtered now
 *   uOpt.z: the frame count (noise)
 *   uOpt.w: mirror-clamp, where the sampler cannot do it (SDL_gpu): 1 and 2
 *           texel 0's s and t, 4 and 8 texel 1's; the sampler clamps to the
 *           edge and the coordinate is mirrored here (GL leaves this 0)
 *   uNoise.x: the noise scale
 *
 * In SPIR-V the combiner can also be given as specialisation constants (SPEC 1,
 * set by the SDL_gpu backend patching the constants' values): the driver then
 * compiles a program for that one combiner, as the generated GL shaders are.
 */
#ifdef VULKAN
#define FS_IN(n) layout(location = n) in
layout(set = 2, binding = 0) uniform sampler2D uTex0;
layout(set = 2, binding = 1) uniform sampler2D uTex1;
layout(std140, set = 3, binding = 0) uniform CombinerUniforms {
    ivec4 uCC[4];
    ivec4 uForm;
    ivec4 uOpt;
    vec4 uNoise;
};
layout(location = 0) out vec4 outColor;

layout(constant_id = 0) const int SPEC = 0;
layout(constant_id = 1) const int S_CC0 = 0;
layout(constant_id = 2) const int S_CC1 = 0;
layout(constant_id = 3) const int S_CC2 = 0;
layout(constant_id = 4) const int S_CC3 = 0;
layout(constant_id = 5) const int S_CC4 = 0;
layout(constant_id = 6) const int S_CC5 = 0;
layout(constant_id = 7) const int S_CC6 = 0;
layout(constant_id = 8) const int S_CC7 = 0;
layout(constant_id = 9) const int S_CC8 = 0;
layout(constant_id = 10) const int S_CC9 = 0;
layout(constant_id = 11) const int S_CC10 = 0;
layout(constant_id = 12) const int S_CC11 = 0;
layout(constant_id = 13) const int S_CC12 = 0;
layout(constant_id = 14) const int S_CC13 = 0;
layout(constant_id = 15) const int S_CC14 = 0;
layout(constant_id = 16) const int S_CC15 = 0;
layout(constant_id = 17) const int S_FORM0 = 0;
layout(constant_id = 18) const int S_FORM1 = 0;
layout(constant_id = 19) const int S_FORM2 = 0;
layout(constant_id = 20) const int S_FORM3 = 0;
layout(constant_id = 21) const int S_OPT = 0; /* uOpt.x */
layout(constant_id = 22) const int S_TEX = 0; /* uOpt.y's bits 1, 2 and 4 */

ivec4 ccOf(int i) {
    if (SPEC != 0) {
        return i == 0 ? ivec4(S_CC0, S_CC1, S_CC2, S_CC3) : i == 1 ? ivec4(S_CC4, S_CC5, S_CC6, S_CC7)
             : i == 2 ? ivec4(S_CC8, S_CC9, S_CC10, S_CC11) : ivec4(S_CC12, S_CC13, S_CC14, S_CC15);
    }
    return uCC[i];
}
int formOf(int i) {
    if (SPEC != 0) {
        return i == 0 ? S_FORM0 : i == 1 ? S_FORM1 : i == 2 ? S_FORM2 : S_FORM3;
    }
    return uForm[i];
}
int optsOf() {
    return SPEC != 0 ? S_OPT : uOpt.x;
}
int texOf() {
    return SPEC != 0 ? (S_TEX | (uOpt.y & 24)) : uOpt.y;
}
#else
#define FS_IN(n) in
uniform sampler2D uTex0;
uniform sampler2D uTex1;
uniform ivec4 uCC[4];
uniform ivec4 uForm;
uniform ivec4 uOpt;
uniform vec4 uNoise;
out vec4 outColor;

ivec4 ccOf(int i) {
    return uCC[i];
}
int formOf(int i) {
    return uForm[i];
}
int optsOf() {
    return uOpt.x;
}
int texOf() {
    return uOpt.y;
}
#endif

FS_IN(0) vec2 vTexCoord0;
FS_IN(1) vec2 vTexCoord1;
FS_IN(2) vec4 vFog;
FS_IN(3) vec4 vInput1;
FS_IN(4) vec4 vInput2;
FS_IN(5) vec4 vInput3;
FS_IN(6) vec4 vInput4;
FS_IN(7) vec4 vInput5;
FS_IN(8) vec4 vInput6;
FS_IN(9) vec4 vInput7;

#define OPT_ALPHA 1
#define OPT_FOG 2
#define OPT_TEXTURE_EDGE 4
#define OPT_NOISE 8
#define OPT_2CYC 16
#define OPT_ALPHA_THRESHOLD 32

// Reference approach to color wrapping as per GLideN64
// Return wrapped value of x in interval [low, high)
#define WRAP(x, low, high) mod((x)-(low), (high)-(low)) + (low)
#define TEX_OFFSET(tex, uv, texSize, off) texFetch(tex, uv - (off)/texSize, mc)

/* texture(), with mirror-clamp-to-edge on the axes in mc: the coordinate
   mirrored once about 0, then clamped by the sampler. The gradients are the
   unmirrored coordinate's, as the hardware mode's are (mc is uniform) */
vec4 texFetch(in sampler2D tex, in vec2 uv, in ivec2 mc) {
    if (mc == ivec2(0)) {
        return texture(tex, uv);
    }
    vec2 m = vec2(mc.x != 0 ? abs(uv.x) : uv.x, mc.y != 0 ? abs(uv.y) : uv.y);
    return textureGrad(tex, m, dFdx(uv), dFdy(uv));
}

float random(in vec3 value) {
    float random = dot(sin(value), vec3(12.9898, 78.233, 37.719));
    return fract(sin(random) * 143758.5453);
}

float noiseRandom() {
    return random(vec3(floor(gl_FragCoord.xy * uNoise.x), float(uOpt.z)));
}

vec4 filter3point(in sampler2D tex, in vec2 texCoord, in vec2 texSize, in ivec2 mc) {
    vec2 offset = fract(texCoord*texSize - vec2(0.5));
    offset -= step(1.0, offset.x + offset.y);
    vec4 c0 = TEX_OFFSET(tex, texCoord, texSize, offset);
    vec4 c1 = TEX_OFFSET(tex, texCoord, texSize, vec2(offset.x - sign(offset.x), offset.y));
    vec4 c2 = TEX_OFFSET(tex, texCoord, texSize, vec2(offset.x, offset.y - sign(offset.y)));
    return c0 + abs(offset.x)*(c1-c0) + abs(offset.y)*(c2-c0);
}

vec4 hookTexture2D(in sampler2D tex, in vec2 uv, in int three_point_filter, in int mcBits) {
    ivec2 mc = ivec2(mcBits & 1, mcBits & 2);
    if ((texOf() & 4) != 0) {
        vec2 texSize = vec2(textureSize(tex, 0));
        return mix(texFetch(tex, uv, mc), filter3point(tex, uv, texSize, mc), float(three_point_filter));
    }
    return texFetch(tex, uv, mc);
}

/* One combiner input as a vec4: the alpha formula takes .a of it, which is
   what the generator's alpha-only inputs are */
vec4 ccInput(int s, vec4 texVal0, vec4 texVal1, vec4 texel) {
    switch (s) {
        case 0: return vec4(0.0, 0.0, 0.0, 0.0);
        case 1: return vInput1;
        case 2: return vInput2;
        case 3: return vInput3;
        case 4: return vInput4;
        case 5: return vInput5;
        case 6: return vInput6;
        case 7: return vInput7;
        case 8: return texVal0;
        case 9: return vec4(texVal0.a, texVal0.a, texVal0.a, texVal0.a);
        case 10: return texVal1;
        case 11: return vec4(texVal1.a, texVal1.a, texVal1.a, texVal1.a);
        case 12: return vec4(1.0, 1.0, 1.0, 1.0);
        case 13: return texel;
    }
    float n = (noiseRandom() + 1.0) / 2.0; /* SHADER_NOISE */
    return vec4(n, n, n, n);
}

vec4 ccFormula(ivec4 s, int form, vec4 texVal0, vec4 texVal1, vec4 texel) {
    if (form == 1) {
        return ccInput(s.w, texVal0, texVal1, texel);
    }
    vec4 a = ccInput(s.x, texVal0, texVal1, texel);
    vec4 c = ccInput(s.z, texVal0, texVal1, texel);
    if (form == 2) {
        return a * c;
    }
    vec4 b = ccInput(s.y, texVal0, texVal1, texel);
    if (form == 3) {
        return mix(b, a, c);
    }
    return (a - b) * c + ccInput(s.w, texVal0, texVal1, texel);
}

void main() {
    int opts = optsOf();
    int tex = texOf();
    vec4 texVal0 = vec4(0.0), texVal1 = vec4(0.0);

    if ((tex & 1) != 0) {
        texVal0 = hookTexture2D(uTex0, vTexCoord0, (tex >> 3) & 1, uOpt.w & 3);
    }
    if ((tex & 2) != 0) {
        texVal1 = hookTexture2D(uTex1, vTexCoord1, (tex >> 4) & 1, (uOpt.w >> 2) & 3);
    }

    vec4 texel = vec4(0.0);
    int cycles = (opts & OPT_2CYC) != 0 ? 2 : 1;
    for (int c = 0; c < cycles; c++) {
        vec4 rgb = ccFormula(ccOf(c * 2), formOf(c * 2), texVal0, texVal1, texel);
        vec4 alpha = ccFormula(ccOf(c * 2 + 1), formOf(c * 2 + 1), texVal0, texVal1, texel);
        texel = vec4(rgb.rgb, alpha.a);
        if (c == 0) {
            texel = WRAP(texel, -1.01, 1.01);
        }
    }
    texel = WRAP(texel, -0.51, 1.51);
    texel = clamp(texel, 0.0, 1.0);
    if ((opts & OPT_FOG) != 0) {
        texel = vec4(mix(texel.rgb, vFog.rgb, vFog.a), texel.a);
    }
    if ((opts & OPT_ALPHA) != 0) {
        if ((opts & OPT_TEXTURE_EDGE) != 0) {
            if (texel.a > 0.19) texel.a = 1.0; else discard;
        }
        if ((opts & OPT_NOISE) != 0) {
            texel.a *= floor(clamp(noiseRandom() + texel.a, 0.0, 1.0));
        }
        if ((opts & OPT_ALPHA_THRESHOLD) != 0) {
            if (texel.a < 8.0 / 256.0) discard;
        }
        outColor = texel;
    } else {
        outColor = vec4(texel.rgb, 1.0);
    }
}
