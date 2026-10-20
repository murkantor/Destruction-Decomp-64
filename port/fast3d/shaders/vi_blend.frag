#version 450
/* N64 AA pass 2 (gfx_opengl.cpp vi_init has the GL twin): the VI's edge blend,
   each pixel towards its 3x3 mean by the coverage it lacks */
layout(set = 2, binding = 0) uniform sampler2D uTex;
layout(std140, set = 3, binding = 0) uniform ViBlend {
    ivec4 uSize;    /* xy */
    vec4 uStrength; /* x */
};
layout(location = 0) out vec4 outColor;

void main() {
    ivec2 p = ivec2(gl_FragCoord.xy);
    vec4 c = texelFetch(uTex, p, 0);
    vec3 acc = vec3(0.0);
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            ivec2 q = clamp(p + ivec2(dx, dy), ivec2(0), uSize.xy - ivec2(1));
            acc += texelFetch(uTex, q, 0).rgb;
        }
    }
    float w = clamp((1.0 - c.a) * uStrength.x, 0.0, 1.0);
    outColor = vec4(mix(c.rgb, acc / 9.0, w), 1.0);
}
