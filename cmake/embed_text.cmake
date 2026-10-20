# Write text files into a C++ header as raw string literals, each named after
# its file (combiner.frag.glsl -> combiner_frag_glsl). The OpenGL backend
# compiles the combiner shader from this text (port/fast3d/shaders).
#   cmake -DOUT=header.h "-DIN=a.glsl;b.glsl" -P embed_text.cmake
set(text_out "/* Generated from port/fast3d/shaders by cmake/embed_text.cmake */\n")
foreach(f ${IN})
    get_filename_component(name ${f} NAME)
    string(MAKE_C_IDENTIFIER ${name} id)
    file(READ ${f} text)
    string(APPEND text_out "static const char ${id}[] = R\"glsl(${text})glsl\";\n")
endforeach()
file(WRITE ${OUT} "${text_out}")
