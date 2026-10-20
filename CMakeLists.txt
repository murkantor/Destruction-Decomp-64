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
set(CMAKE_CXX_STANDARD 17)

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

# ---- Generated: symbol placement and the data fix-up table ----
set(GEN_SYMS ${GEN_DIR}/dd64_syms.ld)
set(GEN_FIX ${GEN_DIR}/dd64_fix.c)
if(WIN32)
    set(GEN_UNDERSCORE --underscore)
endif()
file(GLOB DATA_ASM CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/asm/data/*.s)
add_custom_command(
    OUTPUT ${GEN_SYMS} ${GEN_FIX}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/port/gen_data.py
            --elf ${CMAKE_SOURCE_DIR}/build/dd64.elf --asm ${CMAKE_SOURCE_DIR}/asm/data
            --out ${GEN_DIR} ${GEN_UNDERSCORE}
    DEPENDS ${CMAKE_SOURCE_DIR}/tools/port/gen_data.py ${CMAKE_SOURCE_DIR}/build/dd64.elf ${DATA_ASM}
    COMMENT "Generating the RAM symbol table")

# ---- Game code ----
include(port/sdk_excluded.cmake)
file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.c)
foreach(f ${PORT_SDK_EXCLUDED})
    list(REMOVE_ITEM GAME_SRC ${CMAKE_SOURCE_DIR}/${f})
endforeach()
# The last function not yet matched; this C behaves like the original.
list(APPEND GAME_SRC ${CMAKE_SOURCE_DIR}/nonmatching/Wheel_ComputeTireForce.c)
set_source_files_properties(${CMAKE_SOURCE_DIR}/nonmatching/Wheel_ComputeTireForce.c
    PROPERTIES COMPILE_OPTIONS "-include;common.h")

set(COMMON_FLAGS
    -m32 -g -O2
    # SSE floats round to single precision like the N64 FPU; x87 excess precision would not
    -msse2 -mfpmath=sse
    -fno-strict-aliasing -fwrapv -fno-omit-frame-pointer)

add_library(game OBJECT ${GAME_SRC} ${GEN_FIX})
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
    -fgnu89-inline)

# ---- Port ----
file(GLOB PORT_SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/port/src/*.c ${CMAKE_SOURCE_DIR}/port/src/*.cpp
                                     ${CMAKE_SOURCE_DIR}/port/fast3d/*.cpp ${CMAKE_SOURCE_DIR}/port/fast3d/*.c)
add_executable(dd64 ${PORT_SRC} $<TARGET_OBJECTS:game>)
target_include_directories(dd64 PRIVATE include port/include port/fast3d)
target_compile_definitions(dd64 PRIVATE TARGET_PC)
target_compile_options(dd64 PRIVATE ${COMMON_FLAGS} -Wall -Wno-unused-parameter -Wno-unused-function)
target_link_libraries(dd64 PRIVATE SDL3)
set_target_properties(dd64 PROPERTIES OUTPUT_NAME DestructionDerby64 LINK_DEPENDS ${GEN_SYMS})

if(WIN32)
    # The image sits at 0x10000000 so no host pointer looks like an N64 segment
    # address (0x00000000-0x0FFFFFFF) in a display list.
    target_link_options(dd64 PRIVATE -m32 -static-libgcc -static-libstdc++
        -Wl,--image-base=0x10000000 -Wl,--disable-dynamicbase -Wl,--stack,16777216 -mwindows
        ${GEN_SYMS})
    add_custom_command(TARGET dd64 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL3_MINGW_ROOT}/bin/SDL3.dll $<TARGET_FILE_DIR:dd64>)
else()
    target_link_options(dd64 PRIVATE -m32 -no-pie -Wl,-Ttext-segment=0x10000000 ${GEN_SYMS})
    target_link_libraries(dd64 PRIVATE m pthread dl)
endif()
