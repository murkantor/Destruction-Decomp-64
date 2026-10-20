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
 *   uNoise.x: the noise scale
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
#else
#define FS_IN(n) in
uniform sampler2D uTex0;
uniform sampler2D uTex1;
uniform ivec4 uCC[4];
uniform ivec4 uForm;
uniform ivec4 uOpt;
uniform vec4 uNoise;
out vec4 outColor;
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
#define TEX_OFFSET(tex, uv, texSize, off) texture(tex, uv - (off)/texSize)

float random(in vec3 value) {
    float random = dot(sin(value), vec3(12.9898, 78.233, 37.719));
    return fract(sin(random) * 143758.5453);
}

float noiseRandom() {
    return random(vec3(floor(gl_FragCoord.xy * uNoise.x), float(uOpt.z)));
}

vec4 filter3point(in sampler2D tex, in vec2 texCoord, in vec2 texSize) {
    vec2 offset = fract(texCoord*texSize - vec2(0.5));
    offset -= step(1.0, offset.x + offset.y);
    vec4 c0 = TEX_OFFSET(tex, texCoord, texSize, offset);
    vec4 c1 = TEX_OFFSET(tex, texCoord, texSize, vec2(offset.x - sign(offset.x), offset.y));
    vec4 c2 = TEX_OFFSET(tex, texCoord, texSize, vec2(offset.x, offset.y - sign(offset.y)));
    return c0 + abs(offset.x)*(c1-c0) + abs(offset.y)*(c2-c0);
}

vec4 hookTexture2D(in sampler2D tex, in vec2 uv, in int three_point_filter) {
    if ((uOpt.y & 4) != 0) {
        vec2 texSize = vec2(textureSize(tex, 0));
        return mix(texture(tex, uv), filter3point(tex, uv, texSize), float(three_point_filter));
    }
    return texture(tex, uv);
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
    int opts = uOpt.x;
    vec4 texVal0 = vec4(0.0), texVal1 = vec4(0.0);

    if ((uOpt.y & 1) != 0) {
        texVal0 = hookTexture2D(uTex0, vTexCoord0, (uOpt.y >> 3) & 1);
    }
    if ((uOpt.y & 2) != 0) {
        texVal1 = hookTexture2D(uTex1, vTexCoord1, (uOpt.y >> 4) & 1);
    }

    vec4 texel = vec4(0.0);
    int cycles = (opts & OPT_2CYC) != 0 ? 2 : 1;
    for (int c = 0; c < cycles; c++) {
        vec4 rgb = ccFormula(uCC[c * 2], uForm[c * 2], texVal0, texVal1, texel);
        vec4 alpha = ccFormula(uCC[c * 2 + 1], uForm[c * 2 + 1], texVal0, texVal1, texel);
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
