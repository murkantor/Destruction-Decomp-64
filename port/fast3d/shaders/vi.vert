#version 450
/* N64 AA (gfx_sdlgpu.cpp): one triangle over the whole target, no vertex buffer */
void main() {
    vec2 p = vec2(gl_VertexIndex == 1 ? 3.0 : -1.0, gl_VertexIndex == 2 ? 3.0 : -1.0);
    gl_Position = vec4(p, 0.0, 1.0);
}
