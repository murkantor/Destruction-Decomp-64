# Destruction Derby 64 PC port (docs/porting.md). The N64 ROM build stays on the
# Makefile; this file builds only the port.
#
# Needs the matching build first (make extract && make): the port takes its
# symbol addresses from build/dd64.elf and the data types from asm/data.
#
# Built 32-bit, so pointers are 4 bytes like on the N64 and the game's structs
# keep their offsets.
cmake_minimum_required(VERSION 3.20)
project(dd64pc C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(GEN_DIR ${CMAKE_BINARY_DIR}/gen)
file(MAKE_DIRECTORY ${GEN_DIR})

# ---- SDL3 ----
if(WIN32)
    set(SDL3_MINGW_ROOT "" CACHE PATH "SDL3 MinGW development package, i686-w64-mingw32 folder")
    if(NOT SDL3_MINGW_ROOT)
        message(FATAL_ERROR "Set SDL3_MINGW_ROOT to SDL3-<version>/i686-w64-mingw32")
    endif()
    add_library(SDL3 SHARED IMPORTED)
    set_target_properties(SDL3 PROPERTIES
        IMPORTED_IMPLIB ${SDL3_MINGW_ROOT}/lib/libSDL3.dll.a
        IMPORTED_LOCATION ${SDL3_MINGW_ROOT}/bin/SDL3.dll
        INTERFACE_INCLUDE_DIRECTORIES ${SDL3_MINGW_ROOT}/include)
else()
    find_package(PkgConfig REQUIRED)
    set(ENV{PKG_CONFIG_LIBDIR} "/usr/lib/i386-linux-gnu/pkgconfig:/usr/share/pkgconfig")
    pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
    add_library(SDL3 ALIAS PkgConfig::SDL3)
endif()

# ---- Generated: symbol placement and the function tables ----
set(GEN_SYMS ${GEN_DIR}/dd64_syms.ld)
set(GEN_FIX ${GEN_DIR}/dd64_fix.c)
if(WIN32)
    set(GEN_UNDERSCORE --underscore)
endif()
add_custom_command(
    OUTPUT ${GEN_SYMS} ${GEN_FIX}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/port/gen_data.py
            --elf ${CMAKE_SOURCE_DIR}/build/dd64.elf --out ${GEN_DIR} ${GEN_UNDERSCORE}
    DEPENDS ${CMAKE_SOURCE_DIR}/tools/port/gen_data.py ${CMAKE_SOURCE_DIR}/build/dd64.elf
    COMMENT "Generating the symbol and function tables")

# ---- Game code ----
include(port/sdk_excluded.cmake)
file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.c)
foreach(f ${PORT_SDK_EXCLUDED})
    list(REMOVE_ITEM GAME_SRC ${CMAKE_SOURCE_DIR}/${f})
endforeach()

set(COMMON_FLAGS
    -m32 -g -O2
    # SSE floats round to single precision like the N64 FPU; x87 excess precision would not
    -msse2 -mfpmath=sse
    # Do not count on a 16-byte aligned stack (Windows threads start 4-byte aligned):
    # functions keeping SSE values on the stack realign it themselves
    -mincoming-stack-boundary=2
    -fno-strict-aliasing -fwrapv -fno-omit-frame-pointer)

# Game code the port adds (pieces of the ROM that are not in the decompilation),
# built like the rest of the game
file(GLOB PORT_GAME_SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/port/game/*.c)
add_library(game OBJECT ${GAME_SRC} ${PORT_GAME_SRC} ${GEN_FIX})
target_include_directories(game PRIVATE include . port/include)
target_compile_definitions(game PRIVATE TARGET_PC NON_MATCHING
    # Game globals named like host libc ones (tools/port/gen_data.py HOST_RENAMES)
    errno=dd64_errno _ctype=dd64_ctype
    # The game's libm and libc, kept for N64 results; renamed so shared
    # libraries (SDL, the GL driver) keep calling the host's
    acos=dd64_acos asin=dd64_asin atan=dd64_atan atan2=dd64_atan2 log=dd64_log log10=dd64_log10
    pow=dd64_pow exp=dd64_exp frexp=dd64_frexp modf=dd64_modf sinf=dd64_sinf
    cosf=dd64_cosf rand=dd64_rand atoi=dd64_atoi ldiv=dd64_ldiv lldiv=dd64_lldiv)
target_compile_options(game PRIVATE ${COMMON_FLAGS}
    # 8-byte doubles and u64s in structs, as on the N64. Not used for port code,
    # whose SDL structs follow the platform ABI.
    -malign-double
    # Each m2c file declares its own prototypes and tentative definitions
    -fcommon -fpermissive -w
    # GCC 2.7.2 semantics: a plain inline function still gets an external copy
    -fgnu89-inline
    # Every memory access calls a hook in port/src/shadow.c, which converts
    # big-endian ROM data to host order where it is first read
    # (asan-globals=1: without it GCC skips accesses it can see land inside a
    # global, and the game's globals are exactly the data that needs converting)
    -fsanitize=kernel-address "SHELL:--param asan-stack=0" "SHELL:--param asan-globals=1"
    "SHELL:--param asan-instrumentation-with-call-threshold=0")

# Static data converted at boot, by the types the game's C gives it
# (tools/port/gen_widths.py reads them from the game objects' debug info)
set(GEN_WIDTHS ${GEN_DIR}/dd64_widths.c)
file(GENERATE OUTPUT ${GEN_DIR}/game_objects.txt CONTENT "$<JOIN:$<TARGET_OBJECTS:game>,\n>\n")
if(NOT CMAKE_OBJDUMP)
    set(CMAKE_OBJDUMP objdump)
endif()
add_custom_command(
    OUTPUT ${GEN_WIDTHS}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/port/gen_widths.py
            --elf ${CMAKE_SOURCE_DIR}/build/dd64.elf --objdump ${CMAKE_OBJDUMP} --out ${GEN_WIDTHS}
            @${GEN_DIR}/game_objects.txt
    DEPENDS ${CMAKE_SOURCE_DIR}/tools/port/gen_widths.py ${CMAKE_SOURCE_DIR}/tools/port/gen_data.py
            ${CMAKE_SOURCE_DIR}/build/dd64.elf $<TARGET_OBJECTS:game>
    COMMENT "Generating the static data conversion table")

# ---- Port ----
file(GLOB PORT_SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/port/src/*.c ${CMAKE_SOURCE_DIR}/port/src/*.cpp
                                     ${CMAKE_SOURCE_DIR}/port/fast3d/*.cpp ${CMAKE_SOURCE_DIR}/port/fast3d/glad/*.c)
# The combiner shader's GLSL, as text for the OpenGL backend
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/port/fast3d/shaders)
set(GEN_SHADER_SRC ${GEN_DIR}/shader_src.h)
set(SHADER_GLSL ${SHADER_DIR}/combiner.vert.glsl ${SHADER_DIR}/combiner.frag.glsl)
add_custom_command(
    OUTPUT ${GEN_SHADER_SRC}
    COMMAND ${CMAKE_COMMAND} -DOUT=${GEN_SHADER_SRC} "-DIN=${SHADER_GLSL}" -P ${CMAKE_SOURCE_DIR}/cmake/embed_text.cmake
    DEPENDS ${SHADER_GLSL} ${CMAKE_SOURCE_DIR}/cmake/embed_text.cmake
    COMMENT "Embedding the combiner shader's GLSL" VERBATIM)

add_executable(dd64 ${PORT_SRC} ${GEN_WIDTHS} ${GEN_SHADER_SRC} $<TARGET_OBJECTS:game>)
target_include_directories(dd64 PRIVATE include port/include port/fast3d ${GEN_DIR})
target_compile_definitions(dd64 PRIVATE TARGET_PC)
target_compile_options(dd64 PRIVATE ${COMMON_FLAGS} -Wall -Wno-unused-parameter -Wno-unused-function
    # Fast3D from Project Midas: keep its own warnings quiet
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-variable -Wno-unused-but-set-variable -Wno-sign-compare
                              -Wno-missing-field-initializers -Wno-format-truncation>)
target_link_libraries(dd64 PRIVATE SDL3)
# Block copies keep the big-endian marks with the bytes (port/src/shadow.c)
target_link_options(dd64 PRIVATE -Wl,--wrap=memcpy,--wrap=memmove,--wrap=memset)
set_target_properties(dd64 PROPERTIES OUTPUT_NAME DestructionDerby64 LINK_DEPENDS ${GEN_SYMS})

if(WIN32)
    # The image sits at 0x10000000 so no host pointer looks like an N64 segment
    # address (0x00000000-0x0FFFFFFF) in a display list.
    target_link_options(dd64 PRIVATE -m32 -static-libgcc -static-libstdc++
        -Wl,--image-base=0x10000000 -Wl,--disable-dynamicbase -Wl,--stack,16777216 -mwindows
        # the N64 code addresses (0x80000000 up) are mapped for jump stubs (port/src/ram.c)
        -Wl,--large-address-aware
        ${GEN_SYMS})
    # raphnet adapters: the Win32 HID API (port/src/raphnet.c)
    target_link_libraries(dd64 PRIVATE hid setupapi)
    # timeBeginPeriod (the DD64_PROFILE sampler, port/src/profile.c)
    target_link_libraries(dd64 PRIVATE winmm)
    add_custom_command(TARGET dd64 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL3_MINGW_ROOT}/bin/SDL3.dll $<TARGET_FILE_DIR:dd64>)
else()
    target_link_options(dd64 PRIVATE -m32 -no-pie -Wl,-Ttext-segment=0x10000000 ${GEN_SYMS})
    target_link_libraries(dd64 PRIVATE m pthread dl)
endif()
