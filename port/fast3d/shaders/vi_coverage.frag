#version 450
/* N64 AA pass 1 (gfx_opengl.cpp vi_init has the GL twin): per pixel the mean of
   the samples, and as alpha the coverage: the largest cluster of equal samples
   over the sample count */
layout(set = 2, binding = 0) uniform sampler2DMS uSrc;
layout(std140, set = 3, binding = 0) uniform ViCoverage {
    ivec4 uSamples; /* x */
};
layout(location = 0) out vec4 outColor;

void main() {
    ivec2 p = ivec2(gl_FragCoord.xy);
    vec3 c0 = texelFetch(uSrc, p, 0).rgb;
    vec3 sum = c0;
    int same = 1;
    for (int i = 1; i < uSamples.x; i++) {
        vec3 c = texelFetch(uSrc, p, i).rgb;
        sum += c;
        if (all(lessThan(abs(c - c0), vec3(1.5 / 255.0)))) same++;
    }
    float cov = float(max(same, uSamples.x - same)) / float(uSamples.x);
    outColor = vec4(sum / float(uSamples.x), cov);
}
