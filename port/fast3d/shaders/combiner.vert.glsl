/*
 * The N64 colour combiner as one program (the vertex half): see
 * combiner.frag.glsl. Every vertex carries the position, then only what its
 * combiner reads (gfx_pc.cpp emit_vertex: texture coordinates, fog, the
 * combiner inputs); the backend gives the attributes a layout lacks some
 * other data, and the fragment half never reads them.
 *
 * No #version: the OpenGL backend puts its own in front (and NO_DEPTH_CLAMP
 * where the driver cannot clamp depth); combiner_vk.vert includes this for
 * the SPIR-V build (glslangValidator -V defines VULKAN).
 */
#ifdef VULKAN
#define VS_IN(n) layout(location = n) in
#define VS_OUT(n) layout(location = n) out
#else
#define VS_IN(n) in
#define VS_OUT(n) out
#endif

VS_IN(0) vec4 aVtxPos;
VS_IN(1) vec2 aTexCoord0;
VS_IN(2) vec2 aTexCoord1;
VS_IN(3) vec4 aFog;
VS_IN(4) vec4 aInput1;
VS_IN(5) vec4 aInput2;
VS_IN(6) vec4 aInput3;
VS_IN(7) vec4 aInput4;
VS_IN(8) vec4 aInput5;
VS_IN(9) vec4 aInput6;
VS_IN(10) vec4 aInput7;

VS_OUT(0) vec2 vTexCoord0;
VS_OUT(1) vec2 vTexCoord1;
VS_OUT(2) vec4 vFog;
VS_OUT(3) vec4 vInput1;
VS_OUT(4) vec4 vInput2;
VS_OUT(5) vec4 vInput3;
VS_OUT(6) vec4 vInput4;
VS_OUT(7) vec4 vInput5;
VS_OUT(8) vec4 vInput6;
VS_OUT(9) vec4 vInput7;

void main() {
    vTexCoord0 = aTexCoord0;
    vTexCoord1 = aTexCoord1;
    vFog = aFog;
    vInput1 = aInput1;
    vInput2 = aInput2;
    vInput3 = aInput3;
    vInput4 = aInput4;
    vInput5 = aInput5;
    vInput6 = aInput6;
    vInput7 = aInput7;
    gl_Position = aVtxPos;
#ifdef NO_DEPTH_CLAMP
    // HACK: workaround for no GL_DEPTH_CLAMP
    gl_Position.z *= 0.3;
#endif
#ifdef VULKAN
    /* Vulkan's depth runs 0..w where GL's runs -w..w. And SDL_gpu turns
       Vulkan's y round (+1 is the top row of an image, the first in memory);
       the backend keeps its images bottom row first as GL does, so y is
       turned back */
    gl_Position.y = -gl_Position.y;
    gl_Position.z = (gl_Position.z + gl_Position.w) * 0.5;
#endif
}
